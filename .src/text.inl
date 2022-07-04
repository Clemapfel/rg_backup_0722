// 
// Copyright 2022 Clemens Cords
// Created on 7/3/22 by clem (mail@clemens-cords.com)
//

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
        for (auto& glyph : _glyphs)
            glyph._shape.render(target, transform, shader);
    }

    void Text::create(RenderTarget& target, Vector2f position, const std::string formatted_text, size_t width_px)
    {
        assert(tag_open_prefix != tag_close_prefix);
        static const bool blend = true;

        bool bold_active = false,
             italic_active = false,
             underlined_active = false,
             strikethrough_active = false,
             shaking_active = false,
             rainbow_active = false,
             wave_active = false;

        RGBA foreground = RGBA(1, 1, 1, 1),
             background = RGBA(0, 0, 0, 0);

        const auto& font = _fonts.at(_font_id);

        auto push_glyph = [&](const std::string& raw, Vector2f position)
        {
            _glyphs.emplace_back(target);
            auto& glyph = _glyphs.back();
            glyph._is_bold = bold_active;
            glyph._is_italic = italic_active;
            glyph._is_underlined = underlined_active;
            glyph._is_strikethrough = strikethrough_active;
            glyph._is_shaking = shaking_active;
            glyph._is_rainbow = rainbow_active;
            glyph._is_wave = wave_active;
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

            auto ascend = TTF_FontAscent(current_font);
            auto descend = -1 * TTF_FontDescent(current_font);

            position.y -= (ascend + descend) * 0.5;

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

    void Text::update(Time time)
    {
        // todo: rainbow text is always white, change vertex color to color it
    }
}