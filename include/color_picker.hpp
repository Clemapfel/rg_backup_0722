// 
// Copyright 2022 Clemens Cords
// Created on 7/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>
#include <include/gl_canvas.hpp>

#include <map>

namespace rat
{
    static inline RGBA current_color = RGBA(1, 1, 1, 1);
        // currently selected color at pallete index 1

    static inline RGBA last_color = RGBA(1, 1, 1, 1);
        // last selected color, only used as UI element in color picker for comparison
    
    // verbose color picker, sliders for every component
    union ColorPicker
    {
        public:
            /// \brief construct but don't allocate
            ColorPicker(float width, float height);

            /// \brief allocate
            static void initialize();

            /// \brief update current_color and all ui elements
            static void update_color(char which_component, float value);

            /// \brief expose
            static GtkWidget* get_native();
            
        private:
            // top left area that display current color and compares it with last one
            struct CurrentColorArea : public GLCanvas
            {
                void update();

                void on_realize(GtkGLArea*) override;
                gboolean on_render(GtkGLArea*, GdkGLContext*) override;
                void on_shutdown(GtkGLArea*) override;
                void on_resize(GtkGLArea* area, gint width, gint height) override;

                Shape _last_color_shape;
                Shape _current_color_shape;
            };

            static inline CurrentColorArea* _current_color_area;
            
            // gradient shape, used as backdrop for scales
            struct Gradient : public GLCanvas
            {
                void update();

                void on_realize(GtkGLArea*) override;
                gboolean on_render(GtkGLArea*, GdkGLContext*) override;
                void on_shutdown(GtkGLArea*) override;
                void on_resize(GtkGLArea* area, gint width, gint height) override;

                Shape _shape;
            };
            
            // scale + gradient + entry
            struct SliderElement
            {
                Gradient  _gradient;
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

            // global container
            static inline GtkWindow* _window;
    };
}