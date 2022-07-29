// 
// Copyright 2022 Clemens Cords
// Created on 7/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_canvas.hpp>

#include <map>

namespace rat
{
    static RGBA primary_color = RGBA(1, 1, 1, 1);

    namespace color_gradient_rectangle_wrapper
    {
        static void on_click(GtkWidget* self, GdkEventButton* event, void* instance);
    }

    class ColorGradientRectangle : public GLCanvas
    {
        friend void color_gradient_rectangle_wrapper::on_click(GtkWidget*, GdkEventButton*, void*);
        static inline std::map<size_t, std::pair<RGBA, RGBA>> _left_right_colors = {};

        public:
            ColorGradientRectangle(Vector2f size, const std::string& fragment_shader_path = "");

            void set_left_color(RGBA);
            void set_right_color(RGBA);

        protected:
            void on_realize(GtkGLArea*) override;
            gboolean on_render(GtkGLArea*, GdkGLContext*) override;
            void on_shutdown(GtkGLArea*) override;
            void on_resize(GtkGLArea* area, gint width, gint height) override;
            void on_click(GtkWidget* self, GdkEventButton* button);

        private:

            std::string _shader_path; // optional shader
            size_t _current_color_location = -1;

            Shader* _shader = nullptr;
            Shape* _gradient_shape = nullptr;
            Shape* _gradient_shape_frame = nullptr;
            void update_gradient_shape_color();

            static inline Shader* _noop_shader = nullptr;
            Shape* _cursor_shape_inner = nullptr;
            Shape* _cursor_shape_frame = nullptr;

            static inline const Transform _identity_transform = Transform();
    };
}

// ##############################################################################################

namespace rat
{
    namespace color_gradient_rectangle_wrapper
    {
        void on_click(GtkWidget* self, GdkEventButton* event, void* instance)
        {
            ((ColorGradientRectangle*) instance)->on_click(self, event);
        }
    }

    ColorGradientRectangle::ColorGradientRectangle(Vector2f size, const std::string& fragment_shader_path)
            : GLCanvas(size), _shader_path(fragment_shader_path)
    {
        gtk_widget_set_events(GTK_WIDGET(get_native()),
             GDK_ALL_EVENTS_MASK
        );

        g_signal_connect(get_native(), "button-press-event", G_CALLBACK(color_gradient_rectangle_wrapper::on_click), this);
        g_signal_connect(get_native(), "button-release-event", G_CALLBACK(color_gradient_rectangle_wrapper::on_click), this);

        if (_noop_shader == nullptr)
            _noop_shader = new Shader();

        if (not fragment_shader_path.empty())
        {
            _shader = new Shader();
            _shader->create_from_file("/home/clem/Workspace/mousetrap/resources/shaders/color_picker_hue_gradient.frag", ShaderType::FRAGMENT);

            _current_color_location = glGetUniformLocation(_shader->get_program_id(), "_current_color");
        }

        _left_right_colors.emplace(std::piecewise_construct,
                                   std::forward_as_tuple(get_id()),
                                   std::forward_as_tuple(RGBA(), RGBA())
        );
    }

    void ColorGradientRectangle::set_left_color(RGBA color)
    {
        _left_right_colors.at(get_id()).first = color;
    }

    void ColorGradientRectangle::set_right_color(RGBA color)
    {
        _left_right_colors.at(get_id()).second = color;
    }

    void ColorGradientRectangle::update_gradient_shape_color()
    {
        auto it = _left_right_colors.find(get_id());
        auto left_color = it->second.first;
        auto right_color = it->second.second;

        if ((glm::vec4) _gradient_shape->get_vertex_color(0) != (glm::vec4) left_color)
        {
            _gradient_shape->set_vertex_color(0, left_color);
            _gradient_shape->set_vertex_color(3, left_color);
        }

        if ((glm::vec4) _gradient_shape->get_vertex_color(1) != (glm::vec4) right_color)
        {
            _gradient_shape->set_vertex_color(1, right_color);
            _gradient_shape->set_vertex_color(2, right_color);
        }
    }

    void ColorGradientRectangle::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        if (_shader == nullptr)
            _shader = new Shader();

        static const float gradient_margin = 0.1;
        _gradient_shape = new Shape();
        _gradient_shape->as_rectangle({0.0, gradient_margin}, {1, 1 - 2*gradient_margin});

        update_gradient_shape_color();

        static const float gradient_frame = 0.05;
        static const float cursor_width = 0.01;
        static const float cursor_frame = 0.25 * cursor_width;

        int x = 0, y = 0;
        gtk_widget_get_size_request(GTK_WIDGET(get_native()), &x, &y);

        float initial_x = 0.5;
        _cursor_shape_inner = new Shape();
        _cursor_shape_inner->as_rectangle({initial_x, 0}, {cursor_width, 1});

        _cursor_shape_frame = new Shape();
        _cursor_shape_frame->as_frame({initial_x, 0}, {cursor_width, 1}, cursor_frame, cursor_frame);
        _cursor_shape_frame->set_color(RGBA(0, 0, 0, 1));

        _gradient_shape_frame = new Shape();
        _gradient_shape_frame->as_frame({0.0, gradient_margin}, {1, 1 - 2*gradient_margin}, gradient_frame * (y / x), gradient_frame);
        _gradient_shape_frame->set_color(RGBA(0, 0, 0, 1));

        gtk_gl_area_queue_render(area);
    }

    void ColorGradientRectangle::on_shutdown(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        delete _shader;
        delete _gradient_shape;
    }

    gboolean ColorGradientRectangle::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        gtk_gl_area_make_current(area);

        update_gradient_shape_color();

        glUseProgram(_shader->get_program_id());
        glUniform4f(_current_color_location, primary_color.r, primary_color.g, primary_color.b, primary_color.a);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        _gradient_shape->render(*_shader, _identity_transform);
        _gradient_shape_frame->render(*_noop_shader, _identity_transform);
        _cursor_shape_inner->render(*_noop_shader, _identity_transform);
        _cursor_shape_frame->render(*_noop_shader, _identity_transform);

        glFlush();
        return FALSE;
    }

    void ColorGradientRectangle::on_resize(GtkGLArea* area, gint _0, gint _1)
    {
        gtk_gl_area_queue_render(area);
    }

    void ColorGradientRectangle::on_click(GtkWidget *self, GdkEventButton* event)
    {
        int width, height;
        gtk_widget_get_size_request(self, &width, &height);

        auto position = Vector2f(event->x / width, event->y / height);
        _cursor_shape_inner->set_top_left(position);

        gtk_gl_area_queue_render(GTK_GL_AREA(self));
    }
}