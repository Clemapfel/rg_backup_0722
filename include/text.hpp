// 
// Copyright 2022 Clemens Cords
// Created on 7/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_ttf.h>

#include <map>
#include <string>
#include <memory>
#include <deque>

#include <include/colors.hpp>
#include <include/shape.hpp>
#include <include/time.hpp>

namespace rat
{
    class Text : public Renderable
    {
        public:
            // for loading fonts
            // expects:
            //  <font_id><font_regular_suffix>.ttf
            //  <font_id><font_bold_suffix>.ttf
            //  <font_id><font_italic_suffix>.ttf
            //  <font_id><font_bold_italic_suffix>.ttf
            static inline std::string font_regular_suffix = "-Regular",
                                      font_bold_suffix = "-Bold",
                                      font_italic_suffix = "-Italic",
                                      font_bold_italic_suffix = "-BoldItalic";

            /// \brief control sequence start/end
            static inline std::string tag_prefix = "<",
                                      tag_suffix = ">",
                                      tag_close_marker = "/";
                                      // example: <a> opens, </a> closes same region with tag a

            /// \brief basic format tags
            /// \note will be parsed as: \<tag_open_prefix>\<tag>\<tag_open_suffix> text \<tag_close_prefix>\<tag>\<tag_close_suffix>
            /// \example \<b>text\<\/b>
            static inline std::string bold_tag = "b",
                                      italic_tag = "i",
                                      underlined_tag = "u",
                                      strikethrough_tag = "s",
                                      shaking_tag = "fx_s",
                                      wave_tag = "fx_w",
                                      rainbow_tag = "fx_r";

            /// \brief color format tags, need to be followed by a color value
            /// \note will be parsed as: \<tag_open_prefix>\<tag=(r, g, b)> text \<tag_close_prefix>\<tag>\<tag_close_suffix><br>
            /// (spaces after comma optional. decimal pointers optional)
            /// \example \<col=(0.1, 0.8, 1)> text \</col><br>
            /// \<col=(0,1,1)> text \</col>
            static inline std::string color_foreground_tag = "col",
                                      color_background_tag = "col_bg";


            /// \brief pause during text scroll. Pause duration is equal to that of normal markers such as `.`
            static inline std::string scrolling_pause_marker = "|";

            /// \brief initialize a text by loading the font
            /// \param font_size: native size of he font, scaling the font by using a non-native size is usually discouraged, see notes
            /// \param font_id: id of the font, example: "roboto"
            /// \param font_path: absolute path to the font, example: "/home/workspace/resources/fonts" (no trailing /)
            /// \note the function will look in `font_path` for files of the following names:<br>
            ///  "\<font_path>/\<font_id>\<font_regular_suffix>.ttf"<br>
            ///  "\<font_path>/\\<font_id>\<font_bold_suffix>.ttf"<br>
            ///  "\<font_path>/\\<font_id>\<font_italic_suffix>.ttf"<br>
            ///  "\<font_path>/\\<font_id>\<font_bold_italic_suffix>.ttf"<br>
            ///  Only the regular version is required, providing additional version of the fonts may improve kerning.
            ///  <br><br>For best result, the font size should by an even integer multiple of the native glyph size, for example, if the .ttf file contains a truetype font at 23px, then the font size should be 23px, (23*2)px, (23*4)px, etc.
            Text(size_t font_size, const std::string& font_id, const std::string& font_path = "/home/clem/Workspace/mousetrap/resources/fonts/");

            /// \brief create the text, with wrapping, without scrolling
            /// \param render_target: render target in whose context the glyphs textures will be created
            /// \param formatted_text: text containing the format tags, will be parsed
            /// \param width: maximum width per line, or -1 for no wrapping
            /// \param line_spacer: vertical distance between lines, can be negative
            void create(RenderTarget&, Vector2f position, const std::string& formatted_text, size_t width_px = -1, int line_spacer = 1);

            /// \brief create the text, with wrapping, with scrolling
            /// \param render_target: render target in whose context the glyphs textures will be created
            /// \param formatted_text: text containing the format tags, will be parsed
            /// \param width: maximum width per line, or -1 for no wrapping
            /// \param line_spacer: vertical distance between lines, can be negative
            void create_as_scrolling(RenderTarget&, Vector2f position, const std::string& formatted_text, size_t width_px = -1, int line_spacer = 1);

            /// \brief align the center of the texts bounding box with point
            /// \param point
            void set_centroid(Vector2f);

            /// \brief align top left of the first glyphs bounding box with point
            /// \param point
            void set_top_left(Vector2f);

            /// \brief get centroid of text bounding box
            /// \returns centroid
            Vector2f get_centroid() const;

            /// \brief get top left of bounding box
            /// \returns position
            Vector2f get_top_left() const;

            /// \brief get bounding box
            /// \returns bounding box
            Rectangle get_bounding_box() const;

            /// \brief get dimensions of bounding box
            /// \returns bounding box
            Vector2f get_size() const;

            /// \brief get number of lines, depends on wrapping
            /// \returns size_t
            size_t get_n_lines() const;

            /// \brief text alignment type
            enum AlignmentType
            {
                /// \brief aligned with left margin
                FLUSH_LEFT,

                /// \brief aligned with right margin
                FLUSH_RIGHT,

                /// \brief aligned such that there is an even gap on both margins
                CENTERED,

                /// \brief aligned such that there is no gap on both margins
                JUSTIFIED
            };

            /// \brief set text alignment
            /// \param type
            void set_alignment(AlignmentType);

            /// \brief set line spacing
            /// \param may_be_negative: spacing, in pixels. Negative values are allowed.
            void set_line_spacing(int may_be_negative);

            /// \brief set maximum width, or -1 for inifinite width
            /// \param width: in pixels
            void set_width(size_t);

            /// \brief align the north west center of entire text with point
            /// \param point
            void align_left_with(Vector2f);

            /// \brief align the center of first line with point
            /// \param point
            void align_center_with(Vector2f);

            /// \brief align north east center of enire text with poin
            /// \param point
            void align_right_with(Vector2f);

            /// \copydoc rat::Renderable::render
            void render(const RenderTarget* target, Transform transform = Transform(), Shader* shader = nullptr) const override;

            /// \brief update the texts animations
            /// \param time: time elapsed since last frame, usually return value of `rat::Window::update`
            virtual void update(Time);

            /// \returns characters per second
            float get_scrolling_speed() const;

            /// \param value: words per second
            void set_scrolling_speed(float);

        private:
            struct Font
            {
                TTF_Font* regular;
                TTF_Font* bold;
                TTF_Font* italic;
                TTF_Font* bold_italic;
            };

            static inline std::map<std::string, Font> _fonts;
            std::string _font_id;
            size_t _n_lines;
            Vector2f _position; // top left

            struct Glyph
            {
                Glyph(RenderTarget& target)
                    : _texture(target)
                {}

                void set_top_left(Vector2f pos)
                {
                    pos.x = round(pos.x);
                    pos.y = round(pos.y);
                    _shape.set_top_left(pos);
                    _background_shape.set_top_left(pos);
                }

                bool _is_bold = false,
                     _is_italic = false,
                     _is_underlined = false,
                     _is_strikethrough = false;

                bool _is_shaking = false,
                     _is_rainbow = false,
                     _is_wave = false;

                RGBA _foreground_color = RGBA(1, 1, 1, 1),
                     _background_color = RGBA(0, 0, 0, 0);

                std::string _content;
                Texture _texture;
                Shape _shape;
                Shape _background_shape;
            };

            AlignmentType _alignment_type = FLUSH_LEFT;
            int _line_spacer = 1;
            size_t _width = -1;
            float _font_size;

            void apply_wrapping();
            std::deque<Glyph> _glyphs = {};

            std::set<size_t> _shake_indices,
                             _wave_indices,
                             _rainbow_indices;

            // fx config:

            static inline const float _shake_distance_factor = 0.05; // factor of font size
            static inline const float _shake_speed_factor = 15; // n ticks per second, upper limit is fps
            static inline float _shake_offset = 0;

            static inline const float _wave_distance_factor = 0.1;
            static inline const float _wave_speed_factor = 15;
            static inline float _wave_offset = 0;

            static inline const float _rainbow_speed_factor = 1 / 3.5; // n cycles per second
            float _rainbow_offset = 0;

            // scrolling:

            Time _scrolling_time = seconds(0);
            std::deque<size_t> _visibility_queue = {};

            std::multiset<size_t> _marker_pause_indices;

            float _scroll_letters_per_seconds = 40;
            static inline const float _scroll_pause_factor = 15; // times duration of single letter
    };
}

#include <.src/text.inl>