// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <include/colors.hpp>
#include <include/vector.hpp>
#include <include/gl_common.hpp>
#include <include/shader.hpp>
#include <include/transform.hpp>
#include <include/shape.hpp>

namespace rat
{
    namespace detail
    {
        static void on_realize_wrapper(void* area, void* instance);
        static gboolean on_render_wrapper(void* area, void* context, void* instance);
        static void on_shutdown_wrapper(void* area, void* instance);
    }

    class GLCanvas
    {
        friend void detail::on_realize_wrapper(void* area, void* instance);
        friend gboolean detail::on_render_wrapper(void* area, void* context, void* instance);
        friend void detail::on_shutdown_wrapper(void* area, void* instance);

        public:
            GLCanvas(Vector2f size);
            GtkWidget* get_native();

        private:
            void on_realize(GtkGLArea*);
            gboolean on_render(GtkGLArea*, GdkGLContext*);
            void on_shutdown(GtkGLArea*);

            GtkWidget* _native;
            Vector2f _size;

            Shader* _shader;
            Transform* _transform;
            Shape* _shape;
    };
}

// ##############################################################################################

#include <iostream>

namespace rat
{
    namespace detail
    {
        static void on_realize_wrapper(void* area, void* instance)
        {
            ((GLCanvas*) instance)->on_realize(GTK_GL_AREA(area));
        }

        static gboolean on_render_wrapper(void* area, void* context, void* instance)
        {
            return ((GLCanvas*) instance)->on_render(GTK_GL_AREA(area), GDK_GL_CONTEXT(context));
        }

        static void on_shutdown_wrapper(void* area, void* instance)
        {
            ((GLCanvas*) instance)->on_shutdown(GTK_GL_AREA(area));
        }
    }

    GLCanvas::GLCanvas(Vector2f size)
        : _native(gtk_gl_area_new()), _size(size)
    {
        gtk_widget_set_size_request(_native, size.x, size.y);
        gtk_gl_area_set_has_alpha(GTK_GL_AREA(_native), TRUE);
        gtk_gl_area_set_auto_render(GTK_GL_AREA(_native), FALSE);

        g_signal_connect(_native, "realize", G_CALLBACK(detail::on_realize_wrapper), this);
        g_signal_connect(_native, "render", G_CALLBACK(detail::on_render_wrapper), this);
        g_signal_connect(_native, "unrealize", G_CALLBACK(detail::on_shutdown_wrapper), this);
    }

    GtkWidget *GLCanvas::get_native()
    {
        return _native;
    }

    void GLCanvas::on_realize(GtkGLArea* area)
    {
        std::cout << "realized" << std::endl;

        gtk_gl_area_make_current(area);

        _shader = new Shader();
        _transform = new Transform();
        _shape = new Shape();
        _shape->as_circle({0, 0}, 0.5, 3);
    }

    void GLCanvas::on_shutdown(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        delete _shader;
        delete _transform;
        delete _shape;
    }

    gboolean GLCanvas::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        std::cout << "rendered" << std::endl;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        _shape->render(*_shader, *_transform);

        glFlush();
        return FALSE;
    }
}