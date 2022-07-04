// 
// Copyright 2022 Clemens Cords
// Created on 7/2/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_ttf.h>

#include <map>
#include <string>
#include <memory>

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

            // control sequence start/end
            static inline std::string tag_open_prefix = "<",
                                      tag_open_suffix = ">",
                                      tag_close_prefix = "</", // has to be different from tag_open_prefix
                                      tag_close_suffix = ">";

            /// \brief basic format tags
            /// \note will be parsed as: \<tag_open_prefix>\<tag>\<tag_open_suffix> text \<tag_close_prefix>\<tag>\<tag_close_suffix>
            /// \example \<b>text\<\/b>
            static inline std::string bold_tag = "b",
                                      italic_tag = "i",
                                      underlined_tag = "u",
                                      strikethrough_tag = "s",
                                      shaking_tag = "fx_sh",
                                      wave_tag = "fx_wv",
                                      rainbow_tag = "fx_rb";

            /// \brief color format tags, need to be followed by a color value
            /// \note will be parsed as: \<tag_open_prefix>\<tag=(r, g, b)> text \<tag_close_prefix>\<tag>\<tag_close_suffix><br>
            /// (spaces after comma optional. decimal pointers optional)
            /// \example \<col=(0.1, 0.8, 1)> text \</col><br>
            /// \<col=(0,1,1)> text \</col>
            static inline std::string color_foreground_tag = "col",
                                      color_background_tag = "col_bg";

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

            /// \brief create the text, with wrapping
            /// \param render_target: render target in whose context the glyphs textures will be created
            /// \param formatted_text: text containing the format tags, will be parsed
            /// \param width: maximum width per line, or -1 for no wrapping
            void create(RenderTarget&, Vector2f position, const std::string formatted_text, size_t width_px = -1);

            /// \brief update the texts animations
            /// \param time: time elapsed since last frame, usually return value of `rat::Window::update`
            void update(Time);

            /// \brief align the west center of the first line of the text with a point
            /// \param point
            void set_position(Vector2f);

            /// \brief align centroid, uses entire bounds of text
            /// \param point
            void set_centroid(Vector2f);

            /// \copydoc rat::Renderable::render
            void render(RenderTarget& target, Transform transform = Transform(), Shader* shader = nullptr) const override;

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

            struct Glyph
            {
                Glyph(RenderTarget& target)
                    : _texture(target)
                {}

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
            };

            static void set_glyph_position(Glyph&, Vector2f); // north-west

            std::vector<Glyph> _glyphs = {};
    };
}

#include <.src/text.inl>