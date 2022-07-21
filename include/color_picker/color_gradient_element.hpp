// 
// Copyright 2022 Clemens Cords
// Created on 7/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <map>

#include <include/gl_canvas.hpp>
#include <gtk/gtk.h>

namespace rat
{
    static RGBA primary_color = RGBA(1, 1, 1, 1);

    class ColorGradientRectangle : public GLCanvas
    {
        struct ColorGradientData {
            RGBA _left;
            RGBA _right;
            float _value = 0;
            std::string _shader_path = "";
        };

        static inline std::map<size_t, ColorGradientData> _data = {};

        public:
            ColorGradientRectangle(Vector2f size, const std::string& fragment_shader_path = "");
            ~ColorGradientRectangle();

            void set_left_color(RGBA);
            void set_right_color(RGBA);
            void set_value(float zero_to_one);

        protected:
            void on_realize(GtkGLArea*) override;
            gboolean on_render(GtkGLArea*, GdkGLContext*) override;
            void on_shutdown(GtkGLArea*) override;
            void on_resize(GtkGLArea* area, gint width, gint height) override;

        private:
            size_t _current_color_location = -1;

            Shader* _shader = nullptr;
            Shape* _gradient_shape = nullptr;
            Shape* _cursor_shape = nullptr;
            Shape* _cursor_frame_shape = nullptr;

            void update_shape();
    };
}

// ##############################################################################################

namespace rat
{
    ColorGradientRectangle::ColorGradientRectangle(Vector2f size, const std::string &fragment_shader_path)
        : GLCanvas(size)
    {
        _data.emplace(get_id(), ColorGradientData());
        _data.at(get_id())._shader_path = fragment_shader_path;
    }

    ColorGradientRectangle::~ColorGradientRectangle()
    {}

    void ColorGradientRectangle::set_left_color(RGBA color)
    {
        _data.at(get_id())._left = color;
    }

    void ColorGradientRectangle::set_right_color(RGBA color)
    {
        _data.at(get_id())._right = color;
    }

    void ColorGradientRectangle::set_value(float zero_to_one)
    {
        zero_to_one = glm::clamp<float>(zero_to_one, 0, 1);
        _data.at(get_id())._value = zero_to_one;
    }

    void ColorGradientRectangle::update_shape()
    {
        auto& data = _data.at(get_id());

        //if ((glm::vec4) _gradient_shape->get_vertex_color(0) != (glm::vec4) data._left)
        {
            _gradient_shape->set_vertex_color(0, data._left);
            _gradient_shape->set_vertex_color(3, data._left);
        }

        //if ((glm::vec4) _gradient_shape->get_vertex_color(1) != (glm::vec4) data._right)
        {
            _gradient_shape->set_vertex_color(1, data._right);
            _gradient_shape->set_vertex_color(2, data._right);
        }

        int x, y;
        gtk_widget_get_size_request(get_native(), &x, &y);

        float x_pos = data._value;
        auto current_page = _cursor_shape->get_centroid();
        _cursor_shape->set_centroid({x_pos, 0});
        _cursor_frame_shape->set_centroid({x_pos, 0});
    }

    void ColorGradientRectangle::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        auto& data = _data.at(get_id());

        _shader = new Shader();
        if (not data._shader_path.empty())
        {
            _shader->create_from_file(data._shader_path, ShaderType::FRAGMENT);
            _current_color_location = glGetUniformLocation(_shader->get_program_id(), "_current_color");
        }

        static const float gradient_margin = 0.05;
        _gradient_shape = new Shape();
        _gradient_shape->as_rectangle({0.0, gradient_margin}, {1, 1 - 2*gradient_margin});

        _cursor_shape = new Shape();
        _cursor_shape->as_rectangle({0, 0}, {0.01, 1});

        auto viewport = get_viewport_size();
        float frame_width = 0.01;


        _cursor_frame_shape = new Shape();
        _cursor_frame_shape->as_frame(_cursor_shape->get_top_left() - Vector2f(frame_width), _cursor_shape->get_size() + Vector2f(frame_width*2), 0.01 * (viewport.x / viewport.y), 0.01);
        _cursor_frame_shape->set_color(RGBA(0, 0, 0, 1));

        update_shape();
        gtk_gl_area_queue_render(area);
    }

    void ColorGradientRectangle::on_shutdown(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        delete _shader;
        delete _gradient_shape;
        delete _cursor_frame_shape;
    }

    gboolean ColorGradientRectangle::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        gtk_gl_area_make_current(area);

        update_shape();

        glUseProgram(_shader->get_program_id());
        glUniform4f(_current_color_location, primary_color.r, primary_color.g, primary_color.b, primary_color.a);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        static auto identity_transform = Transform();
        _gradient_shape->render(*_shader, identity_transform);

        static auto noop_shader = Shader();
        _cursor_shape->render(noop_shader, identity_transform);
        _cursor_frame_shape->render(noop_shader, identity_transform);

        glFlush();
        return FALSE;
    }

    void ColorGradientRectangle::on_resize(GtkGLArea* area, gint width, gint height)
    {
        gtk_gl_area_queue_render(area);
    }
}