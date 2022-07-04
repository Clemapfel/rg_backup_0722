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
        {
            if (glyph._background_color.a != 0)
                glyph._background_shape.render(target, transform, shader);

            glyph._shape.render(target, transform, shader);
        }
    }

    void Text::create(RenderTarget& target, Vector2f position, const std::string formatted_text, size_t width_px)
    {
        static const bool blend = true;

        bool bold_active = false,
             italic_active = false,
             underlined_active = false,
             strikethrough_active = false,
             shaking_active = false,
             rainbow_active = false,
             wave_active = false,
             color_foreground_active = false,
             color_background_active = false;

        RGBA foreground = RGBA(1, 1, 1, 1),
             background = RGBA(0, 0, 0, 1);

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

            if (blend)
                surface = TTF_RenderText_Blended(current_font, raw.c_str(), as_sdl_color(RGBA(foreground.r, foreground.g, foreground.b, 1)));
            else
                surface = TTF_RenderText_Solid(current_font, raw.c_str(), as_sdl_color(RGBA(foreground.r, foreground.g, foreground.b, 1)));

            glyph._texture.create_from(surface);

            auto ascend = TTF_FontAscent(current_font);
            auto descend = -1 * TTF_FontDescent(current_font);

            position.y -= (ascend + descend) * 0.5;

            glyph._shape = RectangleShape(position, Vector2f(surface->w, surface->h));
            glyph._shape.set_color(RGBA(1, 1, 1, 1));

            if (color_background_active)
            {
                glyph._background_shape = RectangleShape(position, Vector2f(surface->w, surface->h));
                glyph._background_shape.set_color(glyph._background_color);
            }

            // reset
            SDL_FreeSurface(surface);
            for (auto* f : {font.regular, font.bold, font.italic, font.bold_italic})
            {
                if (f != nullptr)
                    TTF_SetFontStyle(f, TTF_STYLE_NORMAL);
            }
        };

        auto parse_color = [](const std::string& str) -> RGBA
        {
            // assumes string of the type (r,g,b) or (r, g, b)
            if (str.at(0) != '(')
            {
                std::string msg = "Error parsing color: got: `(`, got: `";
                msg.push_back(str.at(0));
                msg.push_back('`');
                throw std::invalid_argument(msg);
            }


            std::stringstream red, green, blue;

            size_t comma_count = 0;
            for (size_t i = 1; i < str.size(); ++i)
            {
                if (str.at(i) == ',')
                    comma_count++;
                else if (str.at(i) == ' ')
                    continue;
                else
                {
                    if (comma_count == 0)
                        red << str.at(i);
                    else if (comma_count == 1)
                        green << str.at(i);
                    else if (comma_count == 2)
                        blue << str.at(i);
                    else if (comma_count == 3)
                    {
                        static bool once = true;
                        if (once)
                        {
                            std::cerr << "[WARNING] In Text::create::parse_color: Any color component other than r, g and b will be ignored." << std::endl;
                            once = false;
                        }
                    }
                    else
                        throw std::invalid_argument("In Text::create::parse_color: Unable to parse color: too many color components");
                }
            }

            auto r = std::stof(red.str());
            auto g = std::stof(green.str());
            auto b = std::stof(blue.str());

            if (r < 0 or g < 0 or b < 0 or r > 1 or g > 1 or b > 1)
                std::cerr << "[WARNING] In Text::create::parse_color: Color components should be 32-bit floats in [0, 1], got: " << str << ")" << std::endl;

            return RGBA(r, g, b, 1);
        };

        auto substr_is = [&](size_t i, const std::string& str) -> bool
        {
            return formatted_text.substr(i, str.size()) == str;
        };

        auto assert_not_open = [](bool b) -> void
        {
            if (b)
                throw std::invalid_argument("trying to open a region that is already open");
        };

        auto assert_open = [](bool b) -> void
        {
            if (not b)
                throw std::invalid_argument("trying to close a region that is already closed");
        };

        size_t i = 0;
        try
        {
            while (i < formatted_text.size())
            {
                if (substr_is(i, "\\"))
                {
                    i += 1;
                    goto skip_control_tags;
                }

                // start open
                if (substr_is(i, tag_prefix) and not substr_is(i+1, tag_close_marker))
                {
                    i += tag_prefix.size();
                    if (substr_is(i, bold_tag) and substr_is(i + bold_tag.size(), tag_suffix))
                    {
                        assert_not_open(bold_active);

                        bold_active = true;
                        i += bold_tag.size();
                    }
                    else if (substr_is(i, italic_tag) and substr_is(i + italic_tag.size(), tag_suffix))
                    {
                        assert_not_open(italic_active);

                        italic_active = true;
                        i += italic_tag.size();
                    }
                    else if (substr_is(i, underlined_tag) and substr_is(i + underlined_tag.size(), tag_suffix))
                    {
                        assert_not_open(underlined_active);

                        underlined_active = true;
                        i += underlined_tag.size();
                    }
                    else if (substr_is(i, shaking_tag) and substr_is(i + shaking_tag.size(), tag_suffix))
                    {
                        assert_not_open(shaking_active);

                        shaking_active = true;
                        i += shaking_tag.size();
                    }
                    else if (substr_is(i, wave_tag) and substr_is(i + wave_tag.size(), tag_suffix))
                    {
                        assert_not_open(wave_active);

                        wave_active = true;
                        i += wave_tag.size();
                    }
                    else if (substr_is(i, rainbow_tag) and substr_is(i + rainbow_tag.size(), tag_suffix))
                    {
                        assert_not_open(rainbow_active);

                        rainbow_active = true;
                        i += rainbow_tag.size();
                    }
                    else if (substr_is(i, color_foreground_tag) and substr_is(i + color_foreground_tag.size(), "="))
                    {
                        assert_not_open(color_foreground_active);
                        color_foreground_active = true;

                        i += color_foreground_tag.size();
                        if (not substr_is(i, "="))
                        {
                            std::stringstream str;
                            str << "Expected: `=` after `" << color_foreground_tag << "`. Got: " << formatted_text.at(i);
                            throw std::invalid_argument(str.str());
                        }

                        i += 1;

                        std::stringstream str;
                        while (not substr_is(i, ")"))
                        {
                            str << formatted_text.at(i);
                            i += 1;

                            if (i >= formatted_text.size() - 1)
                                throw std::invalid_argument("Unable to parse color tag value");
                        }
                        foreground = parse_color(str.str());
                        i += 1;
                    }
                    else if (substr_is(i, color_background_tag) and substr_is(i + color_background_tag.size(), "="))
                    {
                        assert_not_open(color_background_active);
                        color_background_active = true;

                        i += color_background_tag.size();
                        if (not substr_is(i, "="))
                        {
                            std::stringstream str;
                            str << "Expected: `=` after `" << color_background_tag << "`. Got: " << formatted_text.at(i);
                            throw std::invalid_argument(str.str());
                        }
                        i += 1;

                        std::stringstream str;
                        while (not substr_is(i, ")"))
                        {
                            str << formatted_text.at(i);
                            i += 1;

                            if (i >= formatted_text.size() - 1)
                                throw std::invalid_argument("Unable to parse color tag value");
                        }
                        background = parse_color(str.str());
                        i += 1;
                    }
                    else
                        throw std::invalid_argument("unrecognized control tag");

                    if (not substr_is(i, tag_suffix))
                    {
                        std::stringstream str;
                        str << "Expected: " << tag_suffix << ". Got: " << formatted_text.at(i);
                        throw std::invalid_argument(str.str());
                    }

                    i += tag_suffix.size();
                    continue;
                }

                // start close
                else if (substr_is(i, tag_prefix) and substr_is(i+1, tag_close_marker))
                {
                    i += tag_prefix.size();
                    i += tag_close_marker.size();

                    if (substr_is(i, bold_tag) and substr_is(i + bold_tag.size(), tag_suffix))
                    {
                        assert_open(bold_active);

                        bold_active = false;
                        i += bold_tag.size();
                    }
                    else if (substr_is(i, italic_tag) and substr_is(i + italic_tag.size(), tag_suffix))
                    {
                        assert_open(italic_active);

                        italic_active = false;
                        i += italic_tag.size();
                    }
                    else if (substr_is(i, underlined_tag) and substr_is(i + underlined_tag.size(), tag_suffix))
                    {
                        assert_open(underlined_active);

                        underlined_active = false;
                        i += underlined_tag.size();
                    }
                    else if (substr_is(i, shaking_tag) and substr_is(i + shaking_tag.size(), tag_suffix))
                    {
                        assert_open(shaking_active);

                        shaking_active = false;
                        i += shaking_tag.size();
                    }
                    else if (substr_is(i, wave_tag) and substr_is(i + wave_tag.size(), tag_suffix))
                    {
                        assert_open(wave_active);

                        wave_active = false;
                        i += wave_tag.size();
                    }
                    else if (substr_is(i, rainbow_tag) and substr_is(i + rainbow_tag.size(), tag_suffix))
                    {
                        assert_open(rainbow_active);

                        rainbow_active = false;
                        i += rainbow_tag.size();
                    }
                    else if (substr_is(i, color_foreground_tag) and substr_is(i + color_foreground_tag.size(), tag_suffix))
                    {
                        assert_open(color_foreground_active);

                        color_foreground_active = false;
                        foreground = RGBA(1, 1, 1, 1);

                        i += color_foreground_tag.size();
                    }
                    else if (substr_is(i, color_background_tag) and substr_is(i + color_background_tag.size(), tag_suffix))
                    {
                        assert_open(color_background_active);

                        color_background_active = false;
                        background = RGBA(0, 0, 0, 0);

                        i += color_background_tag.size();
                    }
                    else
                        throw std::invalid_argument("unrecognized control tag");

                    if (not substr_is(i, tag_suffix))
                    {
                        std::stringstream str;
                        str << "Expected: `" << tag_suffix << "`, Got: `" << formatted_text.at(i) << "`";
                        throw std::invalid_argument(str.str());
                    }

                    i += tag_suffix.size();
                    continue;
                }

                skip_control_tags:

                if (i >= formatted_text.size() - 1)
                    break;

                std::string to_push;
                to_push.push_back(formatted_text.at(i));
                push_glyph(to_push, position);

                i += 1;
                position.x += _glyphs.back()._shape.get_size().x;
            }
        }
        catch (std::invalid_argument& exc)
        {
            // pretty printing parsing error

            std::cerr << "[ERROR] In Text::create: Error parsing text at position " << i << std::endl;

            size_t count = 0;
            const size_t left_offset = 20;
            const size_t right_offset = 10;

            std::cerr << "\t";
            for (size_t j = std::max<int>(int(i) - left_offset, 0); j <= i + right_offset && j < formatted_text.size(); ++j, count++)
                std::cerr << formatted_text.at(j);

            std::cerr << std::endl;

            std::cerr << "\t";
            for (size_t j = 0; j <= count - right_offset; ++j)
                std::cerr << "~";

            std::cerr << "^" << std::endl;
            std::cerr << " what(): " << exc.what() << std::endl;
        }

        auto warn_never_closed = [](const std::string& which, const std::string& tag) {
            std::cerr << "[WARNING] In Text::create: Reached end of text, but "
                      << which << " region was never closed with "
                      << tag_prefix << tag_close_marker << tag << tag_suffix
                      << std::endl;
        };

        if (bold_active)
            warn_never_closed("bold", bold_tag);

        if (italic_active)
            warn_never_closed("italic", italic_tag);

        if (strikethrough_active)
            warn_never_closed("strikethrough", strikethrough_tag);

        if (underlined_active)
            warn_never_closed("underlined", underlined_tag);

        if (shaking_active)
            warn_never_closed("shaking fx", shaking_tag);

        if (wave_active)
            warn_never_closed("wave fx", wave_tag);

        if (rainbow_active)
            warn_never_closed("rainbow fx", rainbow_tag);

        if (color_foreground_active)
            warn_never_closed("color", color_foreground_tag);

        if (color_background_active)
            warn_never_closed("background color", color_background_tag);

        for (auto& g : _glyphs)
            g._shape.set_texture(&g._texture);
    }

    void Text::update(Time time)
    {
        // todo: rainbow text is always white, change vertex color to color it
    }
}