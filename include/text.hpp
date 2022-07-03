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
            /// \param font_id: id of the font, example: "roboto"
            /// \param font_path: absolute path to the font, example: "/home/workspace/resources/fonts" (no trailing /)
            /// \note the function will look in `font_path` for files of the following names:<br>
            ///  \<font_id><font_regular_suffix>.ttf<br>
            ///  \<font_id><font_bold_suffix>.ttf<br>
            ///  \<font_id><font_italic_suffix>.ttf<br>
            ///  \<font_id><font_bold_italic_suffix>.ttf<br>
            Text(size_t font_size, const std::string& font_id, const std::string& font_path = "/home/clem/Workspace/mousetrap/resources/fonts/");

            //
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

        //private:
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

                RGBA _foreground_color = RGBA(1, 1, 1, 1),
                     _background_color = RGBA(0, 0, 0, 0);

                std::string _content;
                Texture _texture;
                Shape _shape;
            };

            static void set_glyph_position(Glyph&, Vector2f); // north-west

            std::vector<Glyph> glyphs;
    };
}

#include <.src/text.inl>

namespace rat
{
    Text::Text(size_t font_size, const std::string &font_id, const std::string &font_path)
        : _font_id(font_id)
    {
        if (not TTF_WasInit())
            TTF_Init();

        if (_fonts.find(font_id) == _fonts.end())
        {
            _fonts.emplace(font_id, Font{nullptr, nullptr, nullptr, nullptr});
            auto& element = _fonts.at(font_id);

            static auto load = [&](const std::string suffix) -> TTF_Font*
            {
                auto path = font_path + font_id + suffix + ".ttf";
                auto* out = TTF_OpenFont(path.c_str(), font_size);

                if (out == nullptr)
                    std::cerr << "[WARNING] Unable to load font at " << path << std::endl;

                return out;
            };

            element.regular = load(font_regular_suffix);
            element.bold = load(font_bold_suffix);
            element.italic = load(font_italic_suffix);
            element.bold_italic = load(font_bold_italic_suffix);
        }
    }

    void Text::render(RenderTarget& target, Transform transform, Shader* shader) const
    {
        for (auto& glyph : glyphs)
            glyph._shape.render(target, transform, shader);
    }

    void Text::create(RenderTarget& target, Vector2f position, const std::string formatted_text, size_t width_px)
    {
        static const bool blend = true;

        bool bold_active = false,
             italic_active = false,
             underlined_active = false,
             strikethrough_active = false;

        RGBA foreground = RGBA(1, 1, 1, 1),
             background = RGBA(0, 0, 0, 0);

        const auto& font = _fonts.at(_font_id);

        auto push_glyph = [&](const std::string& raw, Vector2f position)
        {
            glyphs.emplace_back(target);
            auto& glyph = glyphs.back();
            glyph._is_bold = bold_active;
            glyph._is_italic = italic_active;
            glyph._is_underlined = underlined_active;
            glyph._is_strikethrough = strikethrough_active;
            glyph._foreground_color = foreground;
            glyph._background_color = background;
            glyph._content = raw;

            TTF_Font* current_font = nullptr;
            uint32_t style = TTF_STYLE_NORMAL;

            // use specialized font unless unavailable
            if (bold_active and italic_active)
            {
                if (font.bold_italic == nullptr)
                {
                    current_font = font.regular;
                    style |= TTF_STYLE_BOLD;
                    style |= TTF_STYLE_ITALIC;
                }
                else
                    current_font = font.bold_italic;
            }
            else if (bold_active)
            {
                if (font.bold == nullptr)
                {
                    current_font = font.regular;
                    style |= TTF_STYLE_BOLD;
                }
                else
                    current_font = font.bold;
            }
            else if (italic_active)
            {
                if (font.italic == nullptr)
                {
                    current_font = font.regular;
                    style |= TTF_STYLE_ITALIC;
                }
                else
                    current_font = font.italic;
            }
            else
                current_font = font.regular;

            if (underlined_active)
                style |= TTF_STYLE_ITALIC;

            if (strikethrough_active)
                style |= TTF_STYLE_STRIKETHROUGH;

            TTF_SetFontStyle(current_font, style);
            SDL_Surface* surface = nullptr;

            if (background.a == 0)
            {
                if (blend)
                    surface = TTF_RenderText_Blended(current_font, raw.c_str(), as_sdl_color(foreground));
                else
                    surface = TTF_RenderText_Solid(current_font, raw.c_str(), as_sdl_color(foreground));
            }
            else
                surface = TTF_RenderText_Shaded(current_font, raw.c_str(), as_sdl_color(foreground), as_sdl_color(background));

            glyph._texture.create_from(surface);
            glyph._shape = Shape();
            glyph._shape = RectangleShape(position, Vector2f(surface->w, surface->h));
            glyph._shape.set_texture(&glyph._texture);

            // reset
            SDL_FreeSurface(surface);
            for (auto* f : {font.regular, font.bold, font.italic, font.bold_italic})
            {
                if (f != nullptr)
                    TTF_SetFontStyle(f, TTF_STYLE_NORMAL);
            }
        };

        push_glyph(formatted_text, position);
    }
}