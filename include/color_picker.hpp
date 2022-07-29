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
                gboolean on_render(GtkGLArea*, GdkGLContext*) override;
                void on_shutdown(GtkGLArea*) override;
                void on_resize(GtkGLArea* area, gint width, gint height) override;

                void render();

                Shader _shader;
                Shape _last_color_shape;
                Shape _current_color_shape;
            };

            static inline CurrentColorArea* _current_color_area;
            
            // scale + gradient + entry
            struct SliderElement
            {
                ShaderArea  _gradient;
                GtkOverlay* _overlay;
                GtkScale* _scale;
                GtkEntry* _entry;
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
                GtkHBox* _hbox;
                GtkLabel* _label;
                GtkSeparatorMenuItem _hline;
            };
            
            static inline LabeledSpacer* _opacity_label;
            static inline LabeledSpacer* _hsv_label;
            static inline LabeledSpacer* _rgb_label;
            static inline LabeledSpacer* _cmyk_label;
            
            // close / dock dialogue
            struct CloseDialogue
            {
                GtkHBox* _hbox;
                GtkButton* _close_button;
                GtkButton* _dock_button;    // pin to main window
                GtkButton* _free_button;    // move to new window
            };

            static inline CloseDialogue* _close_dialogue;
            
            // global config
            static inline const float margin = 20;

            static inline const float left_to_slider_left_margin = 2 * margin;
                // left of window to left of slider

            static inline const float left_to_label_left_margin = 1 * margin;
                // left of window to left of label

            static inline const float bottom_margin = 0.5 * margin;
                // bottom of window to bottom of K slider

            static inline const float right_margin = 1 * margin;
                // right of window to right of entries

            static inline const float top_margin = 0.5 * margin;
                // top of window to top of current_color_area

            static inline const float scale_to_entry_spacer = 1 * margin;
                // right of scale to left of entry for each slider

            static inline const float scale_to_entry_x_ratio = 0.8;
                // scale.width = factor * (box.width - scale_to_entry_spacer)
                // entry.width = (1 - factor) * (box.width - scale_to_entry_spacer)

            static inline const float slider_to_slider_spacer = 1 * margin;
                // bottom of slider to top of next slider within same region (HSV, RGBA, CYMK)
                // also space from bottom of last slider in region to top of label of next region
                // also space from bottom of label to top of first slider in that region

            static inline const float _last_color_to_current_color_x_ratio = 1 / 12.f;
                // width of "last color" = factor * width of current_color_area

            static inline const float _current_color_y_height = 10 * margin;
                // height of last color

            // global containers

            static inline GtkHBox* _window;
                // main window, return by get_native

            static inline GtkHBox* _opacity_region;
            static inline GtkHBox* _hsv_region;
            static inline GtkHBox* _rgb_region;
            static inline GtkHBox* _cmyk_region;

            static inline GtkHBox* _all_slider_regions;
                // spacer + opacity + hsv + rgb + cymk region

            static inline GtkOverlay* _current_color_element_overlay;
                // all_slider_regions under, current_color_element in top right

            static inline GtkOverlay* _close_dialogue_overlay;
                // window underneath, close dialogue top left
    };
}

// ##############################################################################################

#include <include/shader.hpp>

namespace rat
{
    ColorPicker::CurrentColorArea::CurrentColorArea(float width, float height)
        : GLCanvas({width, height}), _shader(),  _last_color_shape(), _current_color_shape()
    {}

    void ColorPicker::CurrentColorArea::render()
    {
        _last_color_shape.set_centroid({0, 0});

        glUseProgram(_shader.get_program_id());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        static auto identity_transform = Transform();
        _current_color_shape.render(_shader, identity_transform);
        _last_color_shape.render(_shader, identity_transform);

        glFlush();
    }

    void ColorPicker::CurrentColorArea::on_realize(GtkGLArea* area)
    {
        gtk_gl_area_make_current(area);

        auto last_width = _last_color_to_current_color_x_ratio * 1;

        _last_color_shape.as_rectangle({-10, 10}, {20, 20});
        _last_color_shape.set_color(RGBA(1, 0, 1, 1));

        _current_color_shape.as_rectangle({1 + last_width, 0}, {1 - last_width, 1});
        _last_color_shape.set_color(RGBA(0, 1, 1, 1));
    }

    gboolean ColorPicker::CurrentColorArea::on_render(GtkGLArea* area, GdkGLContext*)
    {
        gtk_gl_area_make_current(area);
        render();
        return FALSE;
    }

    void ColorPicker::CurrentColorArea::on_shutdown(GtkGLArea*)
    {}

    void ColorPicker::CurrentColorArea::on_resize(GtkGLArea *area, gint width, gint height)
    {
        gtk_gl_area_make_current(area);
        render();
    }

    void ColorPicker::initialize(float width)
    {
        _window = (GtkHBox*) gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

        // current color
        _current_color_area = new CurrentColorArea(width, 2 * margin);
        gtk_container_add(GTK_CONTAINER(_window), _current_color_area->get_native());
    }

    void ColorPicker::update_color(char which_component, float value)
    {
        auto set_gradient_color = [](ShaderArea& gradient, RGBA left, RGBA right) {
            
            auto* shape = gradient.get_shape();
            shape->set_vertex_color(0, left);
            shape->set_vertex_color(3, left);
            shape->set_vertex_color(1, right);
            shape->set_vertex_color(2, right);
            gradient.queue_render();
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

        set_gradient_color(_elements.at('A')->_gradient, alpha_0, alpha_1);

        // skip H

        auto s_0 = hsva;
        s_0.s = 0;
        auto s_1 = hsva;
        s_0.s = 1;

        set_gradient_color(_elements.at('S')->_gradient, s_0, s_1);

        auto v_0 = hsva;
        v_0.v = 0;
        auto v_1 = hsva;
        v_1.v = 0;

        set_gradient_color(_elements.at('V')->_gradient, v_0, v_1);

        auto r_0 = rgba;
        r_0.r = 0;
        auto r_1 = rgba;
        r_1.r = 1;

        set_gradient_color(_elements.at('R')->_gradient, r_0, r_1);

        auto g_0 = rgba;
        g_0.g = 0;
        auto g_1 = rgba;
        g_1.g = 1;

        set_gradient_color(_elements.at('G')->_gradient, g_0, g_1);

        auto b_0 = rgba;
        b_0.b = 0;
        auto b_1 = rgba;
        b_1.b = 1;

        set_gradient_color(_elements.at('B')->_gradient, b_0, b_1);

        auto c_0 = cmyk;
        c_0.c = 0;
        auto c_1 = cmyk;
        c_1.c = 1;

        set_gradient_color(_elements.at('C')->_gradient, c_0, c_1);

        auto m_0 = cmyk;
        m_0.m = 0;
        auto m_1 = cmyk;
        m_1.m = 1;

        set_gradient_color(_elements.at('M')->_gradient, m_0, m_1);

        auto y_0 = cmyk;
        y_0.y = 0;
        auto y_1 = cmyk;
        y_1.y = 1;

        set_gradient_color(_elements.at('Y')->_gradient, y_0, y_1);

        auto k_0 = cmyk;
        k_0.k = 0;
        auto k_1 = cmyk;
        k_1.k = 1;

        set_gradient_color(_elements.at('K')->_gradient, k_0, k_1);

        // update color field

        _current_color_area->_last_color_shape.set_color(last_color);
        _current_color_area->_current_color_shape.set_color(current_color);
        _current_color_area->queue_render();
    }

    GtkWidget* ColorPicker::get_native()
    {
        return GTK_WIDGET(_window);
    }
}