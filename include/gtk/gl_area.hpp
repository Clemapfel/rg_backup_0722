// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

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

    class GLCanvas : public RenderTarget
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

            GtkWidget* get_native();

            static void apply_signals(GLCanvas* instance);

        private:
            void on_realize(GtkGLArea*);
            gboolean on_render(GtkGLArea*);
            void on_shutdown(GtkGLArea*);

            GtkWidget* _native;
            Vector2f _size = {12, 14};

            Shader _shader;
            Shape _shape;
            Transform _transform;
    };
}

namespace rat
{
    namespace detail
    {
        static void on_realize_wrapper(void* area, void* instance)
        {
            ((GLCanvas*) instance)->on_realize(GTK_GL_AREA(area));
        }

        static gboolean on_render_wrapper(void* area, void* instance)
        {
            return ((GLCanvas*) instance)->on_render(GTK_GL_AREA(area));
        }

        static void on_shutdown_wrapper(void* area, void* instance)
        {
            ((GLCanvas*) instance)->on_shutdown(GTK_GL_AREA(area));
        }
    }

    GLCanvas::GLCanvas(Vector2f size)
        : _native(gtk_gl_area_new()), _size(size), _transform(), _shader()
    {
        gtk_widget_set_size_request(_native, size.x, size.y);
        gtk_gl_area_set_has_alpha(GTK_GL_AREA(_native), TRUE);
        gtk_gl_area_set_auto_render(GTK_GL_AREA(_native), TRUE);

        static std::string test = "test";
        g_signal_connect(_native, "realize", G_CALLBACK(detail::on_realize_wrapper), &test);
        g_signal_connect(_native, "render", G_CALLBACK(detail::on_render_wrapper), &test);
        g_signal_connect(_native, "unrealize", G_CALLBACK(detail::on_shutdown_wrapper), &test);

        _shape = Shape();
        _shape.as_rectangle({10, 10}, {100, 100});
        _shape.set_color(RGBA(1, 0, 1, 1));
    }

    GtkWidget *GLCanvas::get_native()
    {
        return _native;
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

    void GLCanvas::on_realize(GtkGLArea* area)
    {}

    gboolean GLCanvas::on_render(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        _shape.render(this, _transform, &_shader);
        return TRUE;
    }

    void GLCanvas::on_shutdown(GtkGLArea* area)
    {}
}