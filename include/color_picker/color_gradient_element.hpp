// 
// Copyright 2022 Clemens Cords
// Created on 7/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_canvas.hpp>

#include <map>

namespace rat
{
    class ColorGradientRectangle : public GLCanvas
    {
        static inline std::map<size_t, std::pair<RGBA, RGBA>> _colors = {};

        public:
            ColorGradientRectangle(Vector2f size);

            void set_left_color(RGBA);
            void set_right_color(RGBA);

        protected:
            void on_realize(GtkGLArea*) override;
            gboolean on_render(GtkGLArea*, GdkGLContext*) override;
            void on_shutdown(GtkGLArea*) override;
            void on_resize(GtkGLArea* area, gint width, gint height) override;

        private:
            Shader* _shader;
            Shape* _shape = nullptr;
            void update_shape_color();
            
            static inline const Transform _identity_transform = Transform();
    };
}

// ##############################################################################################

namespace rat
{
    ColorGradientRectangle::ColorGradientRectangle(Vector2f size)
        : GLCanvas(size)
    {
        _colors.emplace(std::piecewise_construct,
            std::forward_as_tuple(get_id()),
            std::forward_as_tuple(RGBA(), RGBA())
        );
    }

    void ColorGradientRectangle::set_left_color(RGBA color)
    {
        _colors.at(get_id()).first = color;
    }

    void ColorGradientRectangle::set_right_color(RGBA color)
    {
        _colors.at(get_id()).second = color;
    }

    void ColorGradientRectangle::update_shape_color()
    {
        auto it = _colors.find(get_id());
        auto left_color = it->second.first;
        auto right_color = it->second.second;

        if ((glm::vec4) _shape->get_vertex_color(0) != (glm::vec4) left_color)
        {
            _shape->set_vertex_color(0, left_color);
            _shape->set_vertex_color(3, left_color);
        }

        if ((glm::vec4) _shape->get_vertex_color(1) != (glm::vec4) right_color)
        {
            _shape->set_vertex_color(1, right_color);
            _shape->set_vertex_color(2, right_color);
        }
    }

    void ColorGradientRectangle::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        _shader = new Shader();
        _shape = new Shape();
        _shape->as_rectangle({0.0, 0.0}, {1, 1});

        update_shape_color();

        gtk_gl_area_queue_render(area);
    }

    void ColorGradientRectangle::on_shutdown(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        delete _shader;
        delete _shape;
    }

    gboolean ColorGradientRectangle::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        gtk_gl_area_make_current(area);

        update_shape_color();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(1, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        _shape->render(*_shader, _identity_transform);

        glFlush();
        return FALSE;
    }

    void ColorGradientRectangle::on_resize(GtkGLArea* area, gint _0, gint _1)
    {
        gtk_gl_area_queue_render(area);
    }
}