//
// Created by clem on 6/26/22.
//

//#include <gtk-3.0/gtk/gtk.h>

#include <functional>

#include <iostream>
#include <string>

#include <include/gl_canvas.hpp>
#include <gtk/gtk.h>

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // setup window
    auto *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
    gtk_widget_show(window);

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

    // setup render area
    using namespace rat;
    auto canvas = GLCanvas({400, 300});

    auto native = canvas.get_native();
    gtk_widget_set_margin_top(native, 10);
    gtk_widget_set_margin_bottom(native, 10);
    gtk_widget_set_margin_start(native, 10);
    gtk_widget_set_margin_end(native, 10);

    gtk_widget_set_halign(native, GtkAlign::GTK_ALIGN_CENTER);
    gtk_widget_set_valign(native, GtkAlign::GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), native);
    gtk_gl_area_set_auto_render((GtkGLArea *) native, FALSE);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}