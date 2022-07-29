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

    struct ColorGradientElement
    {
        ColorGradientElement(float x_size, const std::string& fragment_shader_path = "");

        GtkOverlay* _overlay;
        GtkScale* _scale;
        ColorGradientRectangle _gradient;
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

        float x_pos = (data._value * 2) - 1;
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

        static const float gradient_y_margin = 0.1;
        static const float gradient_x_margin = 0.03;
        _gradient_shape = new Shape();
        _gradient_shape->as_rectangle({gradient_x_margin, gradient_y_margin*1.5}, {1 - 2*gradient_x_margin, 1 - 2*gradient_y_margin});

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

    ///

    gboolean on_value_change(GtkRange* range, GtkScrollType* _, gdouble value, ColorGradientRectangle* gradient)
    {
        gradient->set_value(gtk_range_get_value(range));
        return FALSE;
    }

    ColorGradientElement::ColorGradientElement(float x_size, const std::string& fragment_shader_path)
        : _gradient({x_size, 30}, fragment_shader_path)
    {
        _scale = GTK_SCALE(gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.01));
        gtk_scale_set_draw_value(_scale, FALSE);
        gtk_widget_set_margin_top(GTK_WIDGET(_scale), 0);

        g_signal_connect(_scale, "change-value", G_CALLBACK(on_value_change), &_gradient);

        _overlay = GTK_OVERLAY(gtk_overlay_new());

        gtk_container_add(GTK_CONTAINER(_overlay), GTK_WIDGET(_gradient.get_native()));
        gtk_widget_set_opacity(GTK_WIDGET(_scale), 0.25);
        gtk_overlay_add_overlay(_overlay, GTK_WIDGET(_scale));
    }

}