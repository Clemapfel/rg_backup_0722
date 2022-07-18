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
    namespace gl_canvas_wrapper
    {
        static void on_realize(void* area, void* instance);
        static gboolean on_render(void* area, void* context, void* instance);
        static void on_shutdown(void* area, void* instance);
        static void on_resize(GtkGLArea* area, gint width, gint height, void* instance);
    }

    class GLCanvas
    {
        friend void gl_canvas_wrapper::on_realize(void* area, void* instance);
        friend gboolean gl_canvas_wrapper::on_render(void* area, void* context, void* instance);
        friend void gl_canvas_wrapper::on_shutdown(void* area, void* instance);
        friend void gl_canvas_wrapper::on_resize(GtkGLArea* area, gint width, gint height, void* instance);

        public:
            GLCanvas(Vector2f size);
            GtkWidget* get_native();
            size_t get_id() const;

        protected:
            virtual void on_realize(GtkGLArea*) = 0;
            virtual gboolean on_render(GtkGLArea*, GdkGLContext*) = 0;
            virtual void on_shutdown(GtkGLArea*) = 0;
            virtual void on_resize(GtkGLArea* area, gint width, gint height) = 0;

        private:
            static inline size_t current_id = 0;
            size_t _id;

            GtkWidget* _native;
    };
}

// ##############################################################################################

#include <iostream>

namespace rat
{
    namespace gl_canvas_wrapper
    {
        static void on_realize(void* area, void* instance)
        {
            ((GLCanvas*) instance)->on_realize(GTK_GL_AREA(area));
        }

        static gboolean on_render(void* area, void* context, void* instance)
        {
            return ((GLCanvas*) instance)->on_render(GTK_GL_AREA(area), GDK_GL_CONTEXT(context));
        }

        static void on_shutdown(void* area, void* instance)
        {
            ((GLCanvas*) instance)->on_shutdown(GTK_GL_AREA(area));
        }

        static void on_resize(GtkGLArea* area, gint width, gint height, void* instance)
        {
            ((GLCanvas*) instance)->on_resize(area, width, height);
        }
    }

    GLCanvas::GLCanvas(Vector2f size)
        : _native(gtk_gl_area_new())
    {
        _id = current_id++;

        gtk_widget_set_size_request(_native, size.x, size.y);
        gtk_gl_area_set_has_alpha(GTK_GL_AREA(_native), TRUE);
        gtk_gl_area_set_auto_render(GTK_GL_AREA(_native), TRUE);

        g_signal_connect(_native, "realize", G_CALLBACK(gl_canvas_wrapper::on_realize), this);
        g_signal_connect(_native, "render", G_CALLBACK(gl_canvas_wrapper::on_render), this);
        g_signal_connect(_native, "unrealize", G_CALLBACK(gl_canvas_wrapper::on_shutdown), this);
        g_signal_connect(_native, "resize", G_CALLBACK(gl_canvas_wrapper::on_resize), this);
    }

    size_t GLCanvas::get_id() const
    {
        return _id;
    }

    GtkWidget *GLCanvas::get_native()
    {
        return _native;
    }
}