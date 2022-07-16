//
// Created by clem on 6/26/22.
//

//#include <gtk-3.0/gtk/gtk.h>

#include <functional>

#include <iostream>
#include <string>

#include <include/gl_canvas.hpp>
#include <include/shader.hpp>
#include <include/shape.hpp>
#include <gtk/gtk.h>

gboolean animate(GtkWidget* widget, GdkFrameClock* frame_clock, gpointer _)
{
    gint64 frame_time = gdk_frame_clock_get_frame_time(frame_clock);
    return G_SOURCE_CONTINUE;
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // setup window
    auto *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
    //gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
    gtk_widget_realize(window);

    // setup gl
    auto *gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

    GError *error_maybe = nullptr;
    auto *context = gdk_window_create_gl_context(gdk_window, &error_maybe);
    if (error_maybe != nullptr)
        std::cerr << "[ERROR] In gdk_window_create_gl_context: " << error_maybe->message << std::endl;

    gdk_gl_context_set_required_version(context, 3, 2);
    gdk_gl_context_set_use_es(context, TRUE);

    gdk_gl_context_realize(context, &error_maybe);
    if (error_maybe != nullptr)
        std::cerr << "[ERROR] In gdk_gl_context_realize: " << error_maybe->message << std::endl;

    gdk_gl_context_make_current(context);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK)
        std::cerr << "[ERROR] In glewInit: " << glewGetErrorString(glewError) << std::endl;

    GL_INITIALIZED = true;

    // setup render area
    using namespace rat;
    auto canvas = GLCanvas({300, 300}, GTK_WINDOW(window));

    auto native = canvas.get_native();

    gint width, height;
    gtk_window_get_size(GTK_WINDOW(window), &width, &height);
    gtk_widget_set_size_request(native, width, height);

    gtk_widget_set_halign(native, GtkAlign::GTK_ALIGN_CENTER);
    gtk_widget_set_valign(native, GtkAlign::GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), native);

    gtk_widget_show_all(window);
    gtk_window_present((GtkWindow*) window);
    gtk_main();
    return 0;
}