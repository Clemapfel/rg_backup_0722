// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtkglarea.h>

#include <include/vector.hpp>
#include <include/colors.hpp>
#include <include/shader.hpp>
#include <include/shape.hpp>

namespace rat
{
    namespace detail
    {
        static void on_realize_wrapper(void* _, void*);
        static gboolean on_render_wrapper(void* _, void*);
        static void on_shutdown_wrapper(void* _, void*);
    }

    class GLCanvas : public GtkGLArea, public RenderTarget
    {
        friend void detail::on_realize_wrapper(void* _, void*);
        friend gboolean detail::on_render_wrapper(void* _, void*);
        friend void detail::on_shutdown_wrapper(void* _, void*);

        public:
            GLCanvas(Vector2f size);
            operator GtkGLArea*();

            Transform get_global_transform() const override;
            void set_global_transform(Transform) override;
            void render(const Renderable*, Transform, Shader*) const override;

            virtual SDL_Renderer* get_renderer() {return nullptr;};



        private:
            void on_realize();
            gboolean on_render();
            void on_shutdown();

            GtkWidget* _native;
            Vector2f _size;

            Shader _shader;
            Shape _shape;
            Transform _transform;
    };
}

// TODO

namespace rat
{
    namespace detail
    {
        static void on_realize_wrapper(void* _, void* instance)
        {
            ((GLCanvas*) instance)->on_realize();
        }

        static gboolean on_render_wrapper(void* _, void* instance)
        {
            return ((GLCanvas*) instance)->on_render();
        }

        static void on_shutdown_wrapper(void* _, void* instance)
        {
            ((GLCanvas*) instance)->on_shutdown();
        }
    }

    GLCanvas::GLCanvas(Vector2f size)
        : _size(size), _native(gtk_gl_area_new()), _shader(), _transform()
    {
        gtk_widget_set_size_request(_native, size.x, size.y);
        gtk_gl_area_set_has_alpha(GTK_GL_AREA(_native), TRUE);
        gtk_gl_area_set_auto_render(GTK_GL_AREA(_native), TRUE);

        auto shape = Shape();
        shape.as_rectangle({10, 10}, {100, 100});
        shape.set_color(RGBA(1, 0, 1, 1));

        g_signal_connect(_native, "render", G_CALLBACK(detail::on_render_wrapper), this);
        g_signal_connect(_native, "realize", G_CALLBACK(detail::on_realize_wrapper), this);
        g_signal_connect(_native, "unrealize", G_CALLBACK(detail::on_shutdown_wrapper), this);
    }

    Transform GLCanvas::get_global_transform() const
    {
        return _transform;
    }

    void GLCanvas::set_global_transform(Transform transform)
    {
        _transform = transform;
    }

    void GLCanvas::render(const Renderable* renderable, Transform transform, Shader* shader) const
    {
        renderable->render(this, transform, shader);
    }

    void GLCanvas::on_realize()
    {
        std::cout << "realized" << std::endl;
    }

    gboolean GLCanvas::on_render()
    {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        _shape.render(this, _transform, &_shader);
        return TRUE;
    }

    void GLCanvas::on_shutdown()
    {
        std::cout << "shutdown" << std::endl;
    }
}