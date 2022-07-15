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

            // TODO
            Shader* _shader;
            int _vertex_position_location = -1,
                _vertex_color_location = -1,
                _vertex_texture_coordinates_location = -1;

            int _vertex_transform_location = -1;

            GLNativeHandle _vertex_array_id = 0,
                           _vertex_buffer_id = 0;

            struct vertex_info
            {
                float position[3];
                float color[3];
            };

            static constexpr struct vertex_info _vertex_data[] = {
                { {  0.0f,  0.500f, 0.0f }, { 1.f, 0.f, 0.f } },
                { {  0.5f, -0.366f, 0.0f }, { 0.f, 1.f, 0.f } },
                { { -0.5f, -0.366f, 0.0f }, { 0.f, 0.f, 1.f } },
            };

            glm::mat4 _transform = {
                1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1.f
            };
            // TODO
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

        _shader = new Shader();
        auto program = _shader->get_program_id();

        _vertex_position_location = glGetAttribLocation(program, "_vertex_position_in");
        _vertex_color_location = glGetAttribLocation(program, "_vertex_color_in");
        _vertex_texture_coordinates_location = glGetAttribLocation(program, "_vertex_texture_coordinates_in");

        _vertex_transform_location = glGetUniformLocation(program, "_transform");

        gtk_gl_area_make_current(area);

        GError* error_maybe = gtk_gl_area_get_error(area);
        if (error_maybe != NULL)
            std::cerr << "In GLCanvas::on_realize: " << error_maybe->message << std::endl;

        glGenVertexArrays(1, &_vertex_array_id);
        glBindVertexArray(_vertex_array_id);

        glGenBuffers(1, &_vertex_buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_vertex_data), _vertex_data, GL_STATIC_DRAW);

        glEnableVertexAttribArray(_vertex_position_location);
        glVertexAttribPointer(_vertex_position_location,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof (struct vertex_info),
            (GLvoid *) (G_STRUCT_OFFSET (struct vertex_info, position))
        );

        glEnableVertexAttribArray(_vertex_color_location);
        glVertexAttribPointer(_vertex_color_location,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof (struct vertex_info),
            (GLvoid *) (G_STRUCT_OFFSET (struct vertex_info, color))
        );

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray (0);
    }

    void GLCanvas::on_shutdown(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        GError* error_maybe = gtk_gl_area_get_error(area);
        if (error_maybe != NULL)
            std::cerr << "In GLCanvas::on_realize: " << error_maybe->message << std::endl;

        glDeleteVertexArrays(1, &_vertex_array_id);
        glDeleteBuffers(1, &_vertex_buffer_id);
        delete _shader;
    }

    gboolean GLCanvas::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        std::cout << "rendered" << std::endl;

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(_shader->get_program_id());
        glUniformMatrix4fv(_vertex_transform_location, 1, GL_FALSE, &_transform[0][0]);
        glBindVertexArray(_vertex_array_id);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray (0);
        glUseProgram (0);

        glFlush();
        return FALSE;
    }
}