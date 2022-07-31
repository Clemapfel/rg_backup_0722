// 
// Copyright 2022 Clemens Cords
// Created on 7/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>
#include <include/gl_canvas.hpp>
#include <include/shader_area.hpp>

#include <map>
#include <atomic>

namespace rat
{
    static inline HSVA current_color = HSVA(0.3, 1, 1, 1);
        // currently selected color at pallete index 1

    static inline HSVA last_color = HSVA(0, 0, 0, 1);
        // last selected color, only used as UI element in color picker for comparison
    
    /// \brief verbose color picker, sliders for every component
    union ColorPicker
    {
        public:
            /// \brief allocate
            static void initialize(float width);

            /// \brief update current_color and all ui elements
            static bool update_color(char which_component, float value);
            static void update_gui();

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
            
            struct Gradient : public GLCanvas
            {
                Gradient(float width, float height, std::string shader_path_maybe = "");

                void on_realize(GtkGLArea*) override;
                void on_shutdown(GtkGLArea*) override {};

                GtkWidget* get_native()
                {
                    return GLCanvas::get_native();
                }

                Shape* _shape;
                Shape* _frame;

                std::string _shader_path;
                Shader* _shader;
            };

            // scale + gradient + entry
            struct SliderElement
            {
                SliderElement(float width, char component, std::string shader_path = "");
                
                Gradient* _gradient;
                GtkOverlay* _overlay;
                GtkBox* _hbox;
                GtkScale* _scale;
                GtkSpinButton* _entry;

                GtkWidget* get_native() {
                    return GTK_WIDGET(_hbox);
                }

                char _component;
                std::vector<size_t> _entry_signal_handlers;
                std::vector<size_t> _scale_signal_handlers;

                void connect_signals();
                void set_signals_blocked(bool);
            };
            
            static inline std::map<char, SliderElement*> _elements = {
                {'A', nullptr},

                {'H', nullptr},
                {'S', nullptr},
                {'V', nullptr},

                {'R', nullptr},
                {'G', nullptr},
                {'B', nullptr},
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
            static inline LabeledSpacer* _html_label;

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

            // html code entry

            struct HexCodeEntry
            {
                HexCodeEntry();

                GtkEntry* _entry;
                GtkButton* _warning_notification;
                GtkBox* _hbox;

                GtkWidget* get_native()
                {
                    return GTK_WIDGET(_hbox);
                }
            };

            static inline HexCodeEntry* _hex_code_entry = nullptr;

            // signals

            static inline bool currently_updating = false;

            // window::realize
            static void widget_on_realize (GtkWidget* widget)
            {
                //update_color('R', current_color.r);
            }

            static void scale_on_value_changed(GtkRange* self, gpointer user_data);
            static void entry_on_activate(GtkEntry* entry, gpointer user_data);
            static void entry_on_value_changed(GtkSpinButton* self, gpointer user_data);
            static void hex_entry_on_activate(GtkEntry* entry, gpointer user_data);
            static void hex_entry_on_paste_clipboard(GtkEntry* entry, gpointer user_data);

            // global containers

            static inline GtkBox* _window;
                // main window, return by get_native

            static inline GtkBox* _opacity_region;
            static inline GtkBox* _hsv_region;
            static inline GtkBox* _rgb_region;
            static inline GtkBox* _html_region;

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

    ColorPicker::Gradient::Gradient(float width, float height, std::string shader_path)
        : GLCanvas({width, height}), _shape(), _frame(), _shader_path(shader_path)
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
        _frame->as_frame({frame_size.x, 0}, {1 - frame_size.x, 1}, frame_size.x, frame_size.y);
        _frame->set_color(RGBA(0, 0, 0, 1));

        if (_shader_path != "")
        {
            _shader = new Shader();
            _shader->create_from_file(_shader_path, ShaderType::FRAGMENT);
            register_render_task(_shape, _shader);
        }
        else
            register_render_task(_shape);

        register_render_task(_frame);
    }

    // scale::value-changed
    void ColorPicker::scale_on_value_changed(GtkRange* self, gpointer user_data)
    {
        for (auto& pair : _elements)
            pair.second->set_signals_blocked(true);

        auto component = *((char*) user_data);
        auto value = gtk_range_get_value(self);

        static size_t i = 0;
        std::cout << "scale:value-changed: " << component << " " << value << " " << i++ << std::endl;

        update_color(component, value);
        update_gui();

        for (auto& pair : _elements)
            pair.second->set_signals_blocked(false);
    }

    // entry::activate
    void ColorPicker::entry_on_activate(GtkEntry* entry, gpointer user_data)
    {
        for (auto& pair : _elements)
            pair.second->set_signals_blocked(true);

        std::cout << "entry:activate" << std::endl;

        auto component = *((char*) user_data);
        auto value = std::stof(gtk_entry_get_text(entry));
        update_color(component, value);
        update_gui();

        for (auto& pair : _elements)
            pair.second->set_signals_blocked(false);
    }

    // entry::value-changed
    void ColorPicker::entry_on_value_changed(GtkSpinButton* self, gpointer user_data)
    {
        for (auto& pair : _elements)
            pair.second->set_signals_blocked(true);

        auto component = *((char*) user_data);
        auto value = gtk_spin_button_get_value(self);

        static size_t i = 0;
        std::cout << "entry:value-changed: " << component << " " << value << " " << i++ << std::endl;

        update_color(component, value);
        update_gui();

        for (auto& pair : _elements)
            pair.second->set_signals_blocked(false);
    }

    // entry::activate
    void ColorPicker::hex_entry_on_activate(GtkEntry* entry, gpointer user_data)
    {
        std::string text = gtk_entry_get_text(entry);
        current_color = html_code_to_rgba(text).operator HSVA();

        for (auto& pair : _elements)
            pair.second->set_signals_blocked(true);

        update_gui();

        for (auto& pair : _elements)
            pair.second->set_signals_blocked(false);
    }

    void ColorPicker::hex_entry_on_paste_clipboard(GtkEntry* entry, gpointer user_data)
    {
        std::string text = gtk_entry_get_text(entry);
        auto as_color = html_code_to_rgba(text);
        gtk_entry_set_text(entry, rgba_to_html_code(as_color).c_str());
    }

    ColorPicker::SliderElement::SliderElement(float width, char component, std::string string)
    {
        _component = component;
        _gradient = new Gradient(width, margin, string);

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

        connect_signals();
    }

    void ColorPicker::SliderElement::connect_signals()
    {
        _entry_signal_handlers.clear();
        _scale_signal_handlers.clear();

        _scale_signal_handlers.push_back(g_signal_connect(GTK_WIDGET(_scale), "value-changed", G_CALLBACK(scale_on_value_changed), (void *) &_component));
        _entry_signal_handlers.push_back(g_signal_connect(GTK_WIDGET(_entry), "activate", G_CALLBACK(entry_on_activate), (void *) &_component));
        _entry_signal_handlers.push_back(g_signal_connect(GTK_WIDGET(_entry), "value-changed", G_CALLBACK(entry_on_value_changed), (void *) &_component));
    }

    void ColorPicker::SliderElement::set_signals_blocked(bool b)
    {
        if (b)
        {
            for (size_t id: _entry_signal_handlers)
                g_signal_handler_block(_entry, id);

            for (size_t id: _scale_signal_handlers)
                g_signal_handler_block(_scale, id);
        }
        else
        {
            for (size_t id: _entry_signal_handlers)
                g_signal_handler_unblock(_entry, id);

            for (size_t id: _scale_signal_handlers)
                g_signal_handler_unblock(_scale, id);
        }

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

    ColorPicker::HexCodeEntry::HexCodeEntry()
    {
        _entry = GTK_ENTRY(gtk_entry_new());
        gtk_entry_set_width_chars(_entry, 4 * 2 + 1);
        gtk_widget_set_hexpand(GTK_WIDGET(_entry), TRUE);
        g_signal_connect(GTK_WIDGET(_entry), "activate", G_CALLBACK(hex_entry_on_activate), nullptr);
        g_signal_connect(GTK_WIDGET(_entry), "paste-clipboard", G_CALLBACK(hex_entry_on_paste_clipboard), nullptr);

        _warning_notification = GTK_BUTTON(gtk_button_new());
        gtk_button_set_label(_warning_notification, "T");

        _hbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, slider_to_slider_spacer));
        gtk_container_add(GTK_CONTAINER(_hbox), GTK_WIDGET(_entry));
        gtk_container_add(GTK_CONTAINER(_hbox), GTK_WIDGET(_warning_notification));
    };

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
        _html_label = new LabeledSpacer(width, "HTML");

        // sliders
        for (char c : {'A', 'S', 'V', 'R', 'G', 'B'})
            _elements[c] = new SliderElement(width, c);

        _elements['H'] = new SliderElement(width, 'H', "/home/clem/Workspace/mousetrap/resources/shaders/color_picker_hue_gradient.frag");


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

        // hex code

        _hex_code_entry = new HexCodeEntry();
        _html_region = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer));
        gtk_container_add(GTK_CONTAINER(_html_region), GTK_WIDGET(_html_label->get_native()));

        gtk_widget_set_margin_start(_hex_code_entry->get_native(), left_to_slider_left_margin);
        gtk_widget_set_valign(_hex_code_entry->get_native(), GtkAlign::GTK_ALIGN_START);
        gtk_widget_set_vexpand(_hex_code_entry->get_native(), FALSE);
        gtk_container_add(GTK_CONTAINER(_html_region), GTK_WIDGET(_hex_code_entry->get_native()));

        _all_slider_regions = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, slider_to_slider_spacer));

        gtk_widget_set_margin_top(GTK_WIDGET(_opacity_region), color_area_height - margin);
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_opacity_region));
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_hsv_region));
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_rgb_region));
        gtk_container_add(GTK_CONTAINER(_all_slider_regions), GTK_WIDGET(_html_region));

        // current color

        _current_color_element_overlay = GTK_OVERLAY(gtk_overlay_new());
        gtk_widget_set_margin_start(_current_color_area->get_native(), 0.45 * width); // unable to align without hardcoding
        gtk_widget_set_hexpand(_current_color_area->get_native(), TRUE);
        gtk_widget_set_valign(_current_color_area->get_native(), GtkAlign::GTK_ALIGN_END);
        gtk_widget_set_valign(_current_color_area->get_native(), GtkAlign::GTK_ALIGN_START);

        gtk_container_add(GTK_CONTAINER(_current_color_element_overlay), GTK_WIDGET(_all_slider_regions));
        gtk_overlay_add_overlay(_current_color_element_overlay, _current_color_area->get_native());

        gtk_widget_set_margin_top(GTK_WIDGET(_current_color_element_overlay), 0.5 * outer_margin);
        gtk_widget_set_margin_bottom(GTK_WIDGET(_current_color_element_overlay), outer_margin);
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

        g_signal_connect(GTK_WIDGET(_window), "realize", G_CALLBACK(widget_on_realize), nullptr);
    }

    bool ColorPicker::update_color(char which_component, float value)
    {
        last_color = current_color;

        switch (which_component)
        {
            case 'A':
            {
                current_color.a = value;
                break;
            }
            case 'H':
            {
                current_color.h = value;
                break;
            }
            case 'S':
            {
                current_color.s = value;
                break;
            }
            case 'V':
            {
                current_color.v = value;
                break;
            }
            case 'R':
            {
                auto rgba = current_color.operator RGBA();
                rgba.r = value;

                float hue_before = current_color.h;
                current_color = rgba.operator HSVA();

                if (current_color.s <= 0.001)
                    current_color.h = hue_before;
                break;
            }
            case 'G':
            {
                auto rgba = current_color.operator RGBA();
                rgba.g = value;

                float hue_before = current_color.h;
                current_color = rgba.operator HSVA();

                if (current_color.s <= 0.001)
                    current_color.h = hue_before;
                break;
            }
            case 'B':
            {
                auto rgba = current_color.operator RGBA();
                rgba.b = value;

                float hue_before = current_color.h;
                current_color = rgba.operator HSVA();

                if (current_color.s <= 0.001)
                    current_color.h = hue_before;
                break;
            }
        }

        return last_color.h != current_color.h or last_color.s != current_color.s or last_color.v != current_color.v or last_color.a != current_color.a;
    }
    
    void ColorPicker::update_gui()
    {
        if (currently_updating)
            return;

        currently_updating = true;

        auto update_slider_element = [&](SliderElement *slider, RGBA left, RGBA right, float value, char which) {

            auto *gradient = slider->_gradient;
            gradient->_shape->set_vertex_color(0, left);
            gradient->_shape->set_vertex_color(3, left);
            gradient->_shape->set_vertex_color(1, right);
            gradient->_shape->set_vertex_color(2, right);
            gradient->queue_render();

            gtk_spin_button_set_value(slider->_entry, value);
            gtk_range_set_value(GTK_RANGE(slider->_scale), value);
        };

        auto rgba = current_color.operator RGBA();
        auto hsva = current_color;

        auto alpha_0 = rgba;
        alpha_0.a = 0;

        auto alpha_1 = rgba;
        alpha_1.a = 1;

        update_slider_element(_elements.at('A'), alpha_0, alpha_1, rgba.a, 'A');

        rgba.a = 1;
        hsva.a = 1;

        auto h_0 = hsva;
        h_0.h = 0;
        auto h_1 = hsva;
        h_1.h = 1;

        update_slider_element(_elements.at('H'), h_0, h_1, hsva.h, 'H');

        auto s_0 = hsva;
        s_0.s = 0;
        auto s_1 = hsva;
        s_1.s = 1;

        update_slider_element(_elements.at('S'), s_0, s_1, hsva.s, 'S');

        auto v_0 = hsva;
        v_0.v = 0;
        auto v_1 = hsva;
        v_1.v = 1;

        update_slider_element(_elements.at('V'), v_0, v_1, hsva.v, 'V');

        auto r_0 = rgba;
        r_0.r = 0;
        auto r_1 = rgba;
        r_1.r = 1;

        update_slider_element(_elements.at('R'), r_0, r_1, rgba.r, 'R');

        auto g_0 = rgba;
        g_0.g = 0;
        auto g_1 = rgba;
        g_1.g = 1;

        update_slider_element(_elements.at('G'), g_0, g_1, rgba.g, 'G');

        auto b_0 = rgba;
        b_0.b = 0;
        auto b_1 = rgba;
        b_1.b = 1;

        update_slider_element(_elements.at('B'), b_0, b_1, rgba.b, 'B');

        std::string text = rgba_to_html_code(current_color.operator RGBA());
        gtk_entry_set_text(GTK_ENTRY(_hex_code_entry->_entry), text.c_str());

        _current_color_area->_last_color_shape->set_color(last_color);
        _current_color_area->_current_color_shape->set_color(current_color);
        _current_color_area->queue_render();

        currently_updating = false;
    }

    GtkWidget* ColorPicker::get_native()
    {
        return GTK_WIDGET(_window);
    }
}