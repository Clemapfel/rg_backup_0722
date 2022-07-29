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

            void queue_render();

        protected:
            virtual void on_realize(GtkGLArea*);
            virtual gboolean on_render(GtkGLArea*, GdkGLContext*);
            virtual void on_shutdown(GtkGLArea*);
            virtual void on_resize(GtkGLArea* area, gint width, gint height);

        private:
            GtkWidget* _native;

            static inline size_t current_id = 0;
            size_t _id;

            static inline Shader* noop_shader = nullptr;
            static inline Transform* noop_transform = nullptr;
            static inline Shape* background_shape = nullptr;
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

    void GLCanvas::queue_render()
    {
        gtk_gl_area_queue_render(GTK_GL_AREA(_native));
    }

    void GLCanvas::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        if (noop_shader == nullptr)
            noop_shader = new Shader();

        if (noop_transform == nullptr)
            noop_transform = new Transform();

        if (background_shape == nullptr)
        {
            background_shape = new Shape();
            background_shape->set_color(RGBA(1, 0, 1, 1));
            background_shape->as_rectangle({0, 0}, {1, 1});
        }

        gtk_gl_area_queue_render(area);
    }

    void GLCanvas::on_shutdown(GtkGLArea*)
    {}

    gboolean GLCanvas::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        gtk_gl_area_make_current(area);

        glUseProgram(noop_shader->get_program_id());
        glViewport(0, 0, 1, 1);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        background_shape->render(*noop_shader, *noop_transform);

        glFlush();
        return FALSE;
    }

    void GLCanvas::on_resize(GtkGLArea *area, gint width, gint height)
    {
        gtk_gl_area_queue_render(area);
    }
}