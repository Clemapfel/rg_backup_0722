// 
// Copyright 2022 Clemens Cords
// Created on 7/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>
#include <include/gl_canvas.hpp>
#include <include/shader_area.hpp>

#include <map>

namespace rat
{
    static inline RGBA current_color = RGBA(1, 1, 1, 1);
        // currently selected color at pallete index 1

    static inline RGBA last_color = RGBA(1, 1, 1, 1);
        // last selected color, only used as UI element in color picker for comparison
    
    /// \brief verbose color picker, sliders for every component
    union ColorPicker
    {
        public:
            /// \brief allocate
            static void initialize(float width);

            /// \brief update current_color and all ui elements
            static void update_color(char which_component, float value);

            /// \brief expose
            static GtkWidget* get_native();
            
        private:
            // top left area that display current color and compares it with last one
            struct CurrentColorArea : public GLCanvas
            {
                CurrentColorArea(float width, float height);

                void on_realize(GtkGLArea*) override;
                void on_shutdown(GtkGLArea*) override {};

                GtkWidget* get_native()
                {
                    return GLCanvas::get_native();
                }

                Shape* _last_color_shape;
                Shape* _current_color_shape;

                Shape* _transparency_tiling;
                Shader* _transparency_tiling_shader;

                Shape* _frame;
            };

            static inline CurrentColorArea* _current_color_area;
            
            // scale + gradient + entry
            struct Gradient : public GLCanvas
            {
                Gradient(float width, float height);

                void on_realize(GtkGLArea*) override;
                void on_shutdown(GtkGLArea*) override {};

                GtkWidget* get_native() {
                    return GLCanvas::get_native();
                }

                Shape* _shape;
                Shape* _frame;
            };

            struct SliderElement
            {
                SliderElement(float width, char component);
                
                Gradient* _gradient;
                GtkOverlay* _overlay;
                GtkBox* _hbox;
                GtkScale* _scale;
                GtkSpinButton* _entry;

                GtkWidget* get_native() {
                    return GTK_WIDGET(_hbox);
                }
            };
            
            static inline std::map<char, SliderElement*> _elements = {
                {'A', nullptr},

                {'H', nullptr},
                {'S', nullptr},
                {'V', nullptr},

                {'R', nullptr},
                {'G', nullptr},
                {'B', nullptr},

                {'C', nullptr},
                {'M', nullptr},
                {'Y', nullptr},
                {'K', nullptr},
            };
            
            // label + spacer
            struct LabeledSpacer
            {
                LabeledSpacer(float width, std::string label);
                
                GtkBox* _hbox;
                GtkLabel* _label;
                GtkSeparatorMenuItem* _hline;

                GtkWidget* get_native() {
                    return GTK_WIDGET(_hbox);
                };
            };
            
            static inline LabeledSpacer* _opacity_label;
            static inline LabeledSpacer* _hsv_label;
            static inline LabeledSpacer* _rgb_label;
            static inline LabeledSpacer* _cmyk_label;
            
            // close / dock dialogue
            struct CloseDialogue
            {
                CloseDialogue();
                GtkButton* _button;

                GtkWidget* get_native()
                {
                    return GTK_WIDGET(_button);
                }
            };

            static inline CloseDialogue* _close_dialogue;

            // signals

            // scale::value-changed
            static void scale_on_value_changed(GtkRange* self, gpointer user_data)
            {
                auto component = *((char*) user_data);
                auto value = gtk_range_get_value(self);
                update_color(component, value);
            }

            // entry::activate
            static void entry_on_activate(GtkEntry* entry, gpointer user_data)
            {
                auto component = *((char*) user_data);
                auto value = std::stof(gtk_entry_get_text(entry));
                update_color(component, value);
            }

            // entry::change-value
            static void entry_on_value_changed(GtkSpinButton* self, gpointer user_data)
            {
                auto component = *((char*) user_data);
                auto value = gtk_spin_button_get_value(self);
                update_color(component, value);
            }

            // global containers

            static inline GtkBox* _window;
                // main window, return by get_native

            static inline GtkBox* _opacity_region;
            static inline GtkBox* _hsv_region;
            static inline GtkBox* _rgb_region;
            static inline GtkBox* _cmyk_region;

            static inline GtkBox* _all_slider_regions;
                // spacer + opacity + hsv + rgb + cymk region

            static inline GtkOverlay* _current_color_element_overlay;
                // all_slider_regions under, current_color_element in top right

            static inline GtkOverlay* _close_dialogue_overlay;
                // window underneath, close dialogue top left
            
            // global config
            static inline float margin = 10;

            static inline const float left_to_slider_left_margin = 1.5 * margin;
                // left of window to left of slider

            static inline const float left_to_label_left_margin = 1 * margin;
                // left of window to left of label

            static inline const float outer_margin = 2 * margin;
                // around outermost box element

            static inline const float scale_to_entry_spacer = 1 * margin;
                // right of scale to left of entry for each slider

            static inline const float scale_to_entry_x_ratio = 0.8;
                // scale.width = factor * (box.width - scale_to_entry_spacer)
                // entry.width = (1 - factor) * (box.width - scale_to_entry_spacer)

            static inline const float slider_height = 1.5 * margin;
                // height of slider + gradient + entry
                
            static inline const float slider_to_slider_spacer = 0.5 * margin;
                // bottom of slider to top of next slider within same region (HSV, RGBA, CYMK)
                // also space from bottom of last slider in region to top of label of next region
                // also space from bottom of label to top of first slider in that region

            static inline const float last_color_to_current_color_x_ratio = 1 / 12.f;
                // width of "last color" = factor * width of current_color_area

            static inline const float current_color_y_height = 10 * margin;
                // height of last color

            static inline const float close_dialogue_height = 0.5 * margin;
                // closed dialogue = 3*h x h
    };
}

// ##############################################################################################

#include <include/shader.hpp>
#include <include/gtk_common.hpp>

namespace rat
{
    ColorPicker::CurrentColorArea::CurrentColorArea(float width, float height)
        : GLCanvas({width, height}), _last_color_shape(), _current_color_shape()
    {}

    void ColorPicker::CurrentColorArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        auto last_width = last_color_to_current_color_x_ratio * 1;

        _last_color_shape = new Shape();
        _current_color_shape = new Shape();
        _transparency_tiling = new Shape();
        _frame = new Shape();

        _transparency_tiling_shader = new Shader();
        _transparency_tiling_shader->create_from_file("/home/clem/Workspace/mousetrap/resources/shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        _last_color_shape->as_rectangle({0, 0}, {last_width, 1});
        _last_color_shape->set_color(RGBA(1, 0, 1, 0));

        _current_color_shape->as_rectangle({last_width, 0}, {1 - last_width, 1});
        _current_color_shape->set_color(RGBA(0, 1, 1, 0));

        _transparency_tiling->as_rectangle({0, 0}, {1, 1});
        _transparency_tiling->set_color(RGBA(1, 1, 1, 1));

        auto size = gtk_widget_get_true_size(area);

        _frame = new Shape();
        float target_frame_size = 0.025;
        Vector2f frame_size = Vector2f(target_frame_size * (size.y / size.x), target_frame_size);
        _frame->as_frame({0, 0}, {1, 1}, frame_size.x, frame_size.y);
        _frame->set_color(RGBA(0, 0, 0, 1));

        register_render_task(_transparency_tiling, _transparency_tiling_shader);
        register_render_task(_last_color_shape);
        register_render_task(_current_color_shape);
        register_render_task(_frame);
    }

    ColorPicker::Gradient::Gradient(float width, float height)
        : GLCanvas({width, height}), _shape(), _frame()
    {}

    void ColorPicker::Gradient::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        _shape = new Shape();
        _shape->as_rectangle({0, 0}, {1, 1});
        _shape->set_vertex_color(0, RGBA(0, 0, 0, 1));
        _shape->set_vertex_color(3, RGBA(0, 0, 0, 1));
        _shape->set_vertex_color(1, RGBA(1, 1, 1, 1));
        _shape->set_vertex_color(2, RGBA(1, 1, 1, 1));

        auto size = gtk_widget_get_true_size(area);

        _frame = new Shape();
        float target_frame_size = 0.025;
        Vector2f frame_size = Vector2f(target_frame_size * (size.y / size.x), target_frame_size);
        _frame->as_frame({0, 0}, {1 - frame_size.x, 1}, frame_size.x, frame_size.y);
        _frame->set_color(RGBA(0, 0, 0, 1));

        register_render_task(_shape);
        register_render_task(_frame);
    }

    static void test()
    {
        std::cout << "called" << std::endl;
    }
    
    ColorPicker::SliderElement::SliderElement(float width, char component)
    {
        _gradient = new Gradient(width, margin);

        _scale = GTK_SCALE(gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 1, 0.001));
        gtk_scale_set_draw_value(_scale, false);
        gtk_scale_set_has_origin(_scale, false);
        gtk_widget_set_opacity(GTK_WIDGET(_scale), 0.5);

        _entry = (GtkSpinButton*) gtk_spin_button_new_with_range(0, 1, 0.01);
        gtk_spin_button_set_digits(_entry, 3);

        _overlay = GTK_OVERLAY(gtk_overlay_new());

        static const float gradient_margin_factor = 0.5;
        gtk_widget_set_margin_top(_gradient->get_native(), gradient_margin_factor * margin);
        gtk_widget_set_margin_bottom(_gradient->get_native(), gradient_margin_factor * margin);
        gtk_widget_set_margin_start(_gradient->get_native(), gradient_margin_factor * 0.5 * margin);
        gtk_widget_set_margin_end(_gradient->get_native(), gradient_margin_factor * 0.5 * margin);

        gtk_container_add(GTK_CONTAINER(_overlay), GTK_WIDGET(_gradient->get_native()));
        gtk_overlay_add_overlay(_overlay, GTK_WIDGET(_scale));
        gtk_widget_set_margin_start(GTK_WIDGET(_overlay), left_to_slider_left_margin);
        gtk_widget_set_margin_end(GTK_WIDGET(_overlay), scale_to_entry_spacer);

        _hbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        gtk_container_add(GTK_CONTAINER(_hbox), GTK_WIDGET(_overlay));
        gtk_container_add(GTK_CONTAINER(_hbox), GTK_WIDGET(_entry));

        g_signal_connect(GTK_WIDGET(_scale), "value-changed", G_CALLBACK(scale_on_value_changed), (void*) new char(component));
        g_signal_connect(GTK_WIDGET(_entry), "activate", G_CALLBACK(entry_on_activate), (void*) new char(component));
        g_signal_connect(GTK_WIDGET(_entry), "value-changed", G_CALLBACK(entry_on_value_changed), (void*) new char(component));
    }
    
    ColorPicker::LabeledSpacer::LabeledSpacer(float width, std::string label)
    {
        _label = (GtkLabel*) gtk_label_new(label.c_str());
        _hline = (GtkSeparatorMenuItem*) gtk_separator_menu_item_new();
        _hbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
        
        gtk_container_add(GTK_CONTAINER(_hbox), GTK_WIDGET(_label));
        gtk_container_add(GTK_CONTAINER(_hbox), GTK_WIDGET(_hline));
    }

    ColorPicker::CloseDialogue::CloseDialogue()
    {
        _button = (GtkButton*) gtk_button_new_with_label("todo");
        //g_signal_connect(_button, "clicked", G_CALLBACK(ColorPicker::trigger_recompile_shader), nullptr);
    }

    void ColorPicker::initialize(float width)
    {
        margin = 0.05 * width;
        _window = (GtkBox*) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

        // current color
        _current_color_area = new CurrentColorArea(width, 5 * margin);
        int color_area_height;
        gtk_widget_get_size_request(_current_color_area->get_native(), nullptr, &color_area_height);

        // labels
        _opacity_label = new LabeledSpacer(width, "Opacity");
        _hsv_label = new LabeledSpacer(width, "HSV");
        _rgb_label = new LabeledSpacer(width, "RGB");
        _cmyk_label = new LabeledSpacer(width, "CMYK");

        // sliders
        for (char c : {'A', 'H', 'S', 'V', 'R', 'G', 'B', 'C', 'M', 'Y', 'K'})
            _elements[c] = new SliderElement(width, c);

        // regions
        _opacity_region = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer));
        gtk_container_add(GTK_CONTAINER(_opacity_region), GTK_WIDGET(_opacity_label->get_native()));
        gtk_container_add(GTK_CONTAINER(_opacity_region), GTK_WIDGET(_elements.at('A')->get_native()));

        _hsv_region = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer));
        gtk_container_add(GTK_CONTAINER(_hsv_region), GTK_WIDGET(_hsv_label->get_native()));
        for (char c : {'H', 'S', 'V'})
            gtk_container_add(GTK_CONTAINER(_hsv_region), GTK_WIDGET(_elements.at(c)->get_native()));

        _rgb_region = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer));
        gtk_container_add(GTK_CONTAINER(_rgb_region), GTK_WIDGET(_rgb_label->get_native()));
        for (char c : {'R', 'G', 'B'})
            gtk_container_add(GTK_CONTAINER(_rgb_region), GTK_WIDGET(_elements.at(c)->get_native()));

        _cmyk_region = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer));
        gtk_container_add(GTK_CONTAINER(_cmyk_region), GTK_WIDGET(_cmyk_label->get_native()));
        for (char c : {'C', 'M', 'Y', 'K'})
            gtk_container_add(GTK_CONTAINER(_cmyk_region), GTK_WIDGET(_elements.at(c)->get_native()));

        _all_slider_regions = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer));

        gtk_widget_set_margin_top(GTK_WIDGET(_opacity_region), color_area_height - margin);
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_opacity_region));
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_hsv_region));
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_rgb_region));
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_cmyk_region));

        // current color

        _current_color_element_overlay = GTK_OVERLAY(gtk_overlay_new());
        gtk_widget_set_margin_start(_current_color_area->get_native(), 0.45 * width); // unable to align without hardcoding
        gtk_widget_set_hexpand(_current_color_area->get_native(), TRUE);
        gtk_widget_set_valign(_current_color_area->get_native(), GtkAlign::GTK_ALIGN_END);
        gtk_widget_set_valign(_current_color_area->get_native(), GtkAlign::GTK_ALIGN_START);

        gtk_container_add(GTK_CONTAINER(_current_color_element_overlay), GTK_WIDGET(_all_slider_regions));
        gtk_overlay_add_overlay(_current_color_element_overlay, _current_color_area->get_native());

        gtk_widget_set_margin_top(GTK_WIDGET(_current_color_element_overlay), 0.5 * outer_margin);
        gtk_widget_set_margin_bottom(GTK_WIDGET(_current_color_element_overlay), 0.5 * outer_margin);
        gtk_widget_set_margin_start(GTK_WIDGET(_current_color_element_overlay), outer_margin);
        gtk_widget_set_margin_end(GTK_WIDGET(_current_color_element_overlay), outer_margin);

        // closing dialogue

        _close_dialogue = new CloseDialogue();
        gtk_widget_set_size_request(_close_dialogue->get_native(), 3 * close_dialogue_height, close_dialogue_height);

        _close_dialogue_overlay = GTK_OVERLAY(gtk_overlay_new());
        gtk_container_add(GTK_CONTAINER(_close_dialogue_overlay), GTK_WIDGET(_current_color_element_overlay));
        gtk_widget_set_halign(_close_dialogue->get_native(), GtkAlign::GTK_ALIGN_START);
        gtk_widget_set_valign(_close_dialogue->get_native(), GtkAlign::GTK_ALIGN_START);
        gtk_overlay_add_overlay(_close_dialogue_overlay, _close_dialogue->get_native());

        gtk_container_add(GTK_CONTAINER(_window), GTK_WIDGET(_close_dialogue_overlay));
    }

    void ColorPicker::update_color(char which_component, float value)
    {
        std::cout << "set" << std::endl;
        auto update_slider_element = [](SliderElement* slider, RGBA left, RGBA right, float value) {

            auto* gradient = slider->_gradient;
            gradient->_shape->set_vertex_color(0, left);
            gradient->_shape->set_vertex_color(3, left);
            gradient->_shape->set_vertex_color(1, right);
            gradient->_shape->set_vertex_color(2, right);
            gradient->queue_render();

            gtk_spin_button_set_value(slider->_entry, value);
        };
        
        auto current = current_color;
        
        RGBA rgba;
        HSVA hsva;
        CMYK cmyk;
        
        switch (which_component)
        {
            case 'A':
                rgba = current_color;
                rgba.a = value;
                hsva = rgba.operator HSVA();
                cmyk = rgba.operator CMYK();
                break;
                
            case 'H':
                hsva = current_color.operator HSVA();
                hsva.h = value;
                rgba = hsva.operator RGBA();
                cmyk = hsva.operator CMYK();
                break;
                
            case 'S':
                hsva = current_color.operator HSVA();
                hsva.s = value;
                rgba = hsva.operator RGBA();
                cmyk = hsva.operator CMYK();
                break;
                
            case 'V':
                hsva = current_color.operator HSVA();
                hsva.v = value;
                rgba = hsva.operator RGBA();
                cmyk = hsva.operator CMYK();
                break;
                
            case 'R':
                rgba = current_color;
                rgba.r = value;
                hsva = rgba.operator HSVA();
                cmyk = rgba.operator CMYK();
                break;

            case 'G':
                rgba = current_color;
                rgba.g = value;
                hsva = rgba.operator HSVA();
                cmyk = rgba.operator CMYK();
                break;

            case 'B':
                rgba = current_color;
                rgba.b = value;
                hsva = rgba.operator HSVA();
                cmyk = rgba.operator CMYK();
                break;
                
            case 'C':
                cmyk = current_color.operator CMYK();
                cmyk.c = value;
                rgba = cmyk.operator RGBA();
                hsva = rgba.operator HSVA();
                break;

            case 'M':
                cmyk = current_color.operator CMYK();
                cmyk.m = value;
                rgba = cmyk.operator RGBA();
                hsva = rgba.operator HSVA();
                break;

            case 'Y':
                cmyk = current_color.operator CMYK();
                cmyk.y = value;
                rgba = cmyk.operator RGBA();
                hsva = rgba.operator HSVA();
                break;

            case 'K':
                cmyk = current_color.operator CMYK();
                cmyk.k = value;
                rgba = cmyk.operator RGBA();
                hsva = rgba.operator HSVA();
                break;
        }

        last_color = current_color;
        current_color = rgba;

        auto alpha_0 = rgba;
        alpha_0.a = 0;

        auto alpha_1 = rgba;
        alpha_1.a = 1;

        update_slider_element(_elements.at('A'), alpha_0, alpha_1, rgba.a);

        auto h_0 = hsva;
        h_0.h = 0;
        auto h_1 = hsva;
        h_1.h = 1;

        update_slider_element(_elements.at('H'), h_0, h_1, hsva.h);

        auto s_0 = hsva;
        s_0.s = 0;
        auto s_1 = hsva;
        s_0.s = 1;

        update_slider_element(_elements.at('S'), s_0, s_1, hsva.s);

        auto v_0 = hsva;
        v_0.v = 0;
        auto v_1 = hsva;
        v_1.v = 0;

        update_slider_element(_elements.at('V'), v_0, v_1, hsva.v);

        auto r_0 = rgba;
        r_0.r = 0;
        auto r_1 = rgba;
        r_1.r = 1;

        update_slider_element(_elements.at('R'), r_0, r_1, rgba.r);

        auto g_0 = rgba;
        g_0.g = 0;
        auto g_1 = rgba;
        g_1.g = 1;

        update_slider_element(_elements.at('G'), g_0, g_1, rgba.g);

        auto b_0 = rgba;
        b_0.b = 0;
        auto b_1 = rgba;
        b_1.b = 1;

        update_slider_element(_elements.at('B'), b_0, b_1, rgba.b);

        auto c_0 = cmyk;
        c_0.c = 0;
        auto c_1 = cmyk;
        c_1.c = 1;

        update_slider_element(_elements.at('C'), c_0, c_1, cmyk.c);

        auto m_0 = cmyk;
        m_0.m = 0;
        auto m_1 = cmyk;
        m_1.m = 1;

        update_slider_element(_elements.at('M'), m_0, m_1, cmyk.m);

        auto y_0 = cmyk;
        y_0.y = 0;
        auto y_1 = cmyk;
        y_1.y = 1;

        update_slider_element(_elements.at('Y'), y_0, y_1, cmyk.y);

        auto k_0 = cmyk;
        k_0.k = 0;
        auto k_1 = cmyk;
        k_1.k = 1;

        update_slider_element(_elements.at('K'), k_0, k_1, cmyk.k);

        // update color field

        _current_color_area->_last_color_shape->set_color(last_color);
        _current_color_area->_current_color_shape->set_color(current_color);
        _current_color_area->queue_render();
    }

    GtkWidget* ColorPicker::get_native()
    {
        return GTK_WIDGET(_window);
    }
}