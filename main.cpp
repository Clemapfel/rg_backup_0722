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
#include <include/shader_area.hpp>
#include <include/color_picker/color_gradient_element.hpp>
#include <gtk/gtk.h>

#include <map>


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

    Vector2f gradient_size = {300, 50};
    std::map<std::string, ColorGradientRectangle> gradients =
    {
        {"ORIGINAL", ColorGradientRectangle(gradient_size)},
        // no H, sic
        {"S", ColorGradientRectangle(gradient_size)},
        {"V", ColorGradientRectangle(gradient_size)},

        {"R", ColorGradientRectangle(gradient_size)},
        {"G", ColorGradientRectangle(gradient_size)},
        {"B", ColorGradientRectangle(gradient_size)},

        {"C", ColorGradientRectangle(gradient_size)},
        {"Y", ColorGradientRectangle(gradient_size)},
        {"M", ColorGradientRectangle(gradient_size)},
        {"K", ColorGradientRectangle(gradient_size)}
    };

    auto color = RGBA(1, 0, 0, 1);

    auto update_gradients = [&]()
    {
        gradients.at("ORIGINAL").set_left_color(color);
        gradients.at("ORIGINAL").set_right_color(color);

        // HSVA
        const HSVA as_hsva = color.operator HSVA();

        auto hsva_0 = as_hsva;
        auto hsva_1 = as_hsva;

        hsva_0 = as_hsva;
        hsva_1 = as_hsva;

        hsva_0.s = 0;
        hsva_1.s = 1;

        gradients.at("S").set_left_color(hsva_0);
        gradients.at("S").set_right_color(hsva_1);

        hsva_0 = as_hsva;
        hsva_1 = as_hsva;

        hsva_0.v = 0;
        hsva_1.v = 1;

        gradients.at("V").set_left_color(hsva_0);
        gradients.at("V").set_right_color(hsva_1);

        // RGBA
        const auto as_rgba = color;

        auto rgba_0 = as_rgba;
        auto rgba_1 = as_rgba;

        rgba_0.r = 0;
        rgba_1.r = 1;

        gradients.at("R").set_left_color(rgba_0);
        gradients.at("R").set_right_color(rgba_1);

        rgba_0 = as_rgba;
        rgba_1 = as_rgba;

        rgba_0.g = 0;
        rgba_1.g = 1;

        gradients.at("G").set_left_color(rgba_0);
        gradients.at("G").set_right_color(rgba_1);

        rgba_0 = as_rgba;
        rgba_1 = as_rgba;

        rgba_0.b = 0;
        rgba_1.b = 1;

        gradients.at("B").set_left_color(rgba_0);
        gradients.at("B").set_right_color(rgba_1);

        // CYMK
        const auto as_cymk = color.operator CYMK();

        auto cymk_0 = as_cymk;
        auto cymk_1 = as_cymk;

        cymk_0.c = 0;
        cymk_1.c = 1;

        gradients.at("C").set_left_color(cymk_0);
        gradients.at("C").set_right_color(cymk_1);

        cymk_0 = as_cymk;
        cymk_1 = as_cymk;

        cymk_0.y = 0;
        cymk_1.y = 1;

        gradients.at("Y").set_left_color(cymk_0);
        gradients.at("Y").set_right_color(cymk_1);

        cymk_0 = as_cymk;
        cymk_1 = as_cymk;

        cymk_0.m = 0;
        cymk_1.m = 1;

        gradients.at("M").set_left_color(cymk_0);
        gradients.at("M").set_right_color(cymk_1);

        cymk_0 = as_cymk;
        cymk_1 = as_cymk;

        cymk_0.k = 0;
        cymk_1.k = 1;

        gradients.at("K").set_left_color(cymk_0);
        gradients.at("K").set_right_color(cymk_1);
    };
    update_gradients();

    auto hue_select = ShaderArea("/home/clem/Workspace/mousetrap/resources/shaders/color_picker_hue_gradient.frag", {100, 50});

    auto vbox_hsv = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    auto vbox_rgb = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    auto vbox_cymk = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    auto vbox_outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    gtk_container_add(GTK_CONTAINER(window), vbox_outer);

    gtk_box_pack_start(GTK_BOX(vbox_hsv), hue_select.get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_hsv), gradients.at("S").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_hsv), gradients.at("V").get_native(), TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox_rgb), gradients.at("R").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_rgb), gradients.at("G").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_rgb), gradients.at("B").get_native(), TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("C").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("Y").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("M").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("K").get_native(), TRUE, TRUE, 0);

    auto hline0 = gtk_separator_menu_item_new();
    auto hline1 = gtk_separator_menu_item_new();

    gtk_widget_set_size_request(hline0, 0, 10);
    gtk_widget_set_size_request(hline1, 0, 10);

    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(vbox_hsv), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(hline0), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(vbox_rgb), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(hline1), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(vbox_cymk), TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_window_present((GtkWindow*) window);
    gtk_main();
    return 0;
}