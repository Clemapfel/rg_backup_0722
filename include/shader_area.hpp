// 
// Copyright 2022 Clemens Cords
// Created on 7/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_canvas.hpp>

namespace rat
{
    class ShaderArea : public GLCanvas
    {
        public:
            ShaderArea(const std::string& fragment_shader_path, Vector2f size);
            GLNativeHandle get_shader_program_id() const;
            Shape* get_shape();

        protected:
            void on_realize(GtkGLArea*) override;
            void on_shutdown(GtkGLArea*) override;

        private:
            std::string _shader_path;
            Shader* _shader;
            Shape* _shape;
            static inline const Transform _identity_transform = Transform();
    };
}

// ##############################################################################################

namespace rat
{
    ShaderArea::ShaderArea(const std::string& fragment_shader_path, Vector2f size)
        : GLCanvas(size), _shader_path(fragment_shader_path)
    {}

    GLNativeHandle ShaderArea::get_shader_program_id() const
    {
        return _shader->get_program_id();
    }

    void ShaderArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        _shader = new Shader();
        _shader->create_from_file(_shader_path, ShaderType::FRAGMENT);

        _shape = new Shape();
        _shape->as_rectangle({0.0, 0.0}, {2, 2});
        _shape->set_centroid({0, 0});
        _shape->set_color(RGBA(1, 1, 1, 1));

        register_render_task(_shape, _shader);
    }

    void ShaderArea::on_shutdown(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        delete _shader;
        delete _shape;
    }

    Shape *ShaderArea::get_shape()
    {
        return _shape;
    }
}
