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
#include <include/color_picker/color_component_entry.hpp>
#include <include/color_picker/color_html_code_entry.hpp>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <map>

void on_activate (GtkEntry* entry, void* _) {
    std::cout << gtk_entry_get_text(entry) << std::endl;
    gtk_entry_set_text(entry, "");
};

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    // setup window
    auto *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_widget_set_events(window, GDK_BUTTON_PRESS_MASK);

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

    float bar_height = 20;

    std::string value;
    value.resize(4);

    Vector2f gradient_size = {300, 20};
    std::map<std::string, ColorGradientRectangle> gradients =
    {
        {"ORIGINAL", ColorGradientRectangle(gradient_size)},
        {"H", ColorGradientRectangle(gradient_size, "/home/clem/Workspace/mousetrap/resources/shaders/color_picker_hue_gradient.frag")},
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

    primary_color = HSVA(0.5, 0.75, 1, 1);

    auto update_gradients = [&]()
    {
        gradients.at("ORIGINAL").set_left_color(primary_color);
        gradients.at("ORIGINAL").set_right_color(primary_color);

        // HSVA
        const HSVA as_hsva = primary_color.operator HSVA();

        auto hsva_0 = as_hsva;
        auto hsva_1 = as_hsva;

        hsva_0 = as_hsva;
        hsva_1 = as_hsva;

        hsva_0.s = 0;
        hsva_1.s = 1;

        gradients.at("S").set_left_color(hsva_0);
        gradients.at("S").set_right_color(hsva_1);
        gradients.at("S").set_value(as_hsva.s);

        hsva_0 = as_hsva;
        hsva_1 = as_hsva;

        hsva_0.v = 0;
        hsva_1.v = 1;

        gradients.at("V").set_left_color(hsva_0);
        gradients.at("V").set_right_color(hsva_1);
        gradients.at("V").set_value(as_hsva.v);

        // RGBA
        const auto as_rgba = primary_color;

        auto rgba_0 = as_rgba;
        auto rgba_1 = as_rgba;

        rgba_0.r = 0;
        rgba_1.r = 1;

        gradients.at("R").set_left_color(rgba_0);
        gradients.at("R").set_right_color(rgba_1);
        gradients.at("R").set_value(as_rgba.r);

        rgba_0 = as_rgba;
        rgba_1 = as_rgba;

        rgba_0.g = 0;
        rgba_1.g = 1;

        gradients.at("G").set_left_color(rgba_0);
        gradients.at("G").set_right_color(rgba_1);
        gradients.at("G").set_value(as_rgba.g);

        rgba_0 = as_rgba;
        rgba_1 = as_rgba;

        rgba_0.b = 0;
        rgba_1.b = 1;

        gradients.at("B").set_left_color(rgba_0);
        gradients.at("B").set_right_color(rgba_1);
        gradients.at("B").set_value(as_rgba.b);

        // CYMK
        const auto as_cymk = primary_color.operator CYMK();

        auto cymk_0 = as_cymk;
        auto cymk_1 = as_cymk;

        cymk_0.c = 0;
        cymk_1.c = 1;

        gradients.at("C").set_left_color(cymk_0);
        gradients.at("C").set_right_color(cymk_1);
        gradients.at("C").set_value(as_cymk.c);

        cymk_0 = as_cymk;
        cymk_1 = as_cymk;

        cymk_0.y = 0;
        cymk_1.y = 1;

        gradients.at("Y").set_left_color(cymk_0);
        gradients.at("Y").set_right_color(cymk_1);
        gradients.at("Y").set_value(as_cymk.y);


        cymk_0 = as_cymk;
        cymk_1 = as_cymk;

        cymk_0.m = 0;
        cymk_1.m = 1;

        gradients.at("M").set_left_color(cymk_0);
        gradients.at("M").set_right_color(cymk_1);
        gradients.at("M").set_value(as_cymk.m);

        cymk_0 = as_cymk;
        cymk_1 = as_cymk;

        cymk_0.k = 0;
        cymk_1.k = 1;

        gradients.at("K").set_left_color(cymk_0);
        gradients.at("K").set_right_color(cymk_1);
        gradients.at("K").set_value(as_cymk.k);
    };
    update_gradients();

    auto vbox_hsv = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    auto vbox_rgb = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    auto vbox_cymk = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    auto vbox_outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_margin_top(GTK_WIDGET(vbox_outer), 50);
    gtk_widget_set_margin_bottom(GTK_WIDGET(vbox_outer), 50);
    gtk_widget_set_margin_start(GTK_WIDGET(vbox_outer), 20);
    gtk_widget_set_margin_end(GTK_WIDGET(vbox_outer), 20);

    gtk_container_add(GTK_CONTAINER(window), vbox_outer);

    auto hsv_separator = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(hsv_separator), 10);

    float label_right_margin = 10;

    auto hsv_label = gtk_label_new("HSV");
    gtk_widget_set_margin_end(GTK_WIDGET(hsv_label), label_right_margin);
    gtk_box_pack_start(GTK_BOX(hsv_separator), hsv_label, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hsv_separator), GTK_WIDGET(gtk_separator_menu_item_new()), TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox_hsv), hsv_separator, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_hsv), gradients.at("H").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_hsv), gradients.at("S").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_hsv), gradients.at("V").get_native(), TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox_rgb), gradients.at("R").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_rgb), gradients.at("G").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_rgb), gradients.at("B").get_native(), TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("C").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("Y").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("M").get_native(), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_cymk), gradients.at("K").get_native(), TRUE, TRUE, 0);

    auto rgb_separator = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(rgb_separator), 25);

    auto rgb_label = gtk_label_new("RGB");
    gtk_widget_set_margin_end(GTK_WIDGET(rgb_label), label_right_margin);
    gtk_box_pack_start(GTK_BOX(rgb_separator), rgb_label, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(rgb_separator), GTK_WIDGET(gtk_separator_menu_item_new()), TRUE, TRUE, 0);

    auto cymk_separator = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(cymk_separator), 25);

    auto cymk_label = gtk_label_new("CYMK");
    gtk_widget_set_margin_end(GTK_WIDGET(cymk_label), label_right_margin);
    gtk_box_pack_start(GTK_BOX(cymk_separator), cymk_label, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(cymk_separator), GTK_WIDGET(gtk_separator_menu_item_new()), TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(vbox_hsv), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(rgb_separator), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(vbox_rgb), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(cymk_separator), TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox_outer), GTK_WIDGET(vbox_cymk), TRUE, TRUE, 0);

    gtk_widget_show_all(window);
    gtk_window_present((GtkWindow*) window);
    gtk_main();
    return 0;
}