// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <glm/glm.hpp>

namespace rat
{
    struct RGBA;
    struct HSVA;
    struct CYMK;

    struct RGBA
    {
        float r = 0;
        float g = 0;
        float b = 0;
        float a = 0;

        RGBA() = default;
        RGBA(float r, float g, float b, float a);

        explicit RGBA(glm::vec4);
        explicit operator glm::vec4() const;

        // rgb <-> rgb
        explicit RGBA(HSVA);
        explicit operator HSVA() const;

        // rgb <-> cym
        explicit RGBA(CYMK);
        explicit operator CYMK() const;

        float to_grayscale() const;
    };

    struct HSVA
    {
        float h = 0;
        float s = 0;
        float v = 0;
        float a = 1;

        HSVA() = default;
        HSVA(float h, float s, float v, float a);

        explicit HSVA(glm::vec4);
        explicit operator glm::vec4() const;

        // hsv <-> rgb
        explicit HSVA(RGBA);
        operator RGBA() const;

        // hsv <-> cym
        explicit HSVA(CYMK);
        explicit operator CYMK() const;

        float to_grayscale() const;
    };

    struct CYMK
    {
        float c = 0;
        float y = 0;
        float m = 0;
        float k = 0;

        CYMK() = default;
        CYMK(float c, float y, float m, float k);

        explicit CYMK(glm::vec4);
        explicit operator glm::vec4() const;

        // cymk <-> rgb
        explicit CYMK(RGBA);
        operator RGBA() const;

        // cym <-> hsv
        explicit CYMK(HSVA);
        explicit operator HSVA() const;

        float to_grayscale() const;
    };

    glm::vec4 rgba_to_hsva(glm::vec4);
    glm::vec4 hsva_to_rgba(glm::vec4);

    glm::vec4 cymk_to_rgba(glm::vec4);
    glm::vec4 rgba_to_cymk(glm::vec4);

    glm::vec4 cymk_to_hsva(glm::vec4);
    glm::vec4 hsva_to_cymk(glm::vec4);

    RGBA html_code_to_rgba(const std::string& code);

}

// ##############################################################################################

#include <include/vector.hpp>
#include <sstream>
#include <cctype>

namespace rat
{
    RGBA::RGBA(float r, float g, float b, float a)
        : r(r), g(g), b(b), a(a)
    {}

    RGBA::RGBA(glm::vec4 vec)
        : r(vec[0]), g(vec[1]), b(vec[2]), a(vec[3])
    {}

    float RGBA::to_grayscale() const
    {
        return (r + g + b) / 3;
    }

    RGBA::operator glm::vec4() const
    {
        return glm::vec4(r, g, b, a);
    }

    RGBA::RGBA(CYMK cymk)
    {
        auto out = cymk_to_rgba(cymk.operator glm::vec4());
        r = out[0];
        g = out[1];
        b = out[2];
        a = out[3];
    }

    RGBA::operator CYMK() const
    {
        return CYMK(rgba_to_cymk(this->operator glm::vec4()));
    }

    RGBA::RGBA(HSVA hsva)
    {
        auto out = hsva_to_rgba(hsva.operator glm::vec4());
        r = out[0];
        g = out[1];
        b = out[2];
        a = out[4];
    }

    RGBA::operator HSVA() const
    {
        return HSVA(rgba_to_hsva(this->operator glm::vec4()));
    }

    HSVA::HSVA(float r, float g, float b, float a)
            : h(r), s(g), v(b), a(a)
    {}

    HSVA::HSVA(glm::vec4 vec)
            : h(vec[0]), s(vec[1]), v(vec[2]), a(vec[3])
    {}

    float HSVA::to_grayscale() const
    {
        return s;
    }

    HSVA::operator glm::vec4() const
    {
        return glm::vec4(h, s, v, a);
    }

    HSVA::HSVA(RGBA rgba)
    {
        auto out = rgba_to_hsva(rgba.operator glm::vec4());
        h = out[0];
        s = out[1];
        v = out[2];
        a = out[3];
    }

    HSVA::operator RGBA() const
    {
        return RGBA(hsva_to_rgba(this->operator glm::vec4()));
    }

    HSVA::HSVA(CYMK cymk)
    {
        auto out = cymk_to_rgba(cymk.operator glm::vec4());
        h = out[0];
        s = out[1];
        v = out[2];
        a = out[3];
    }

    HSVA::operator CYMK() const
    {
        return CYMK(rgba_to_hsva(this->operator glm::vec4()));
    }

    CYMK::CYMK(float c, float y, float m, float k)
        : c(c), y(y), m(m), k(k)
    {}

    CYMK::CYMK(glm::vec4 vec)
        : c(vec[0]), y(vec[1]), m(vec[2]), k(vec[3])
    {}

    float CYMK::to_grayscale() const
    {
        return operator RGBA().to_grayscale();
    }

    CYMK::operator glm::vec4() const
    {
        return glm::vec4(c, y, m, k);
    }

    CYMK::CYMK(RGBA rgba)
    {
        auto out = rgba_to_cymk(rgba.operator glm::vec4());
        c = out[0];
        y = out[1];
        m = out[2];
        k = out[3];
    }

    CYMK::operator RGBA() const
    {
        return RGBA(cymk_to_rgba(this->operator glm::vec4()));
    }

    CYMK::CYMK(HSVA hsva)
    {
        auto out = hsva_to_cymk(hsva.operator glm::vec4());
        c = out[0];
        y = out[1];
        m = out[2];
        k = out[3];
    }

    CYMK::operator HSVA() const
    {
        return HSVA(cymk_to_hsva(this->operator glm::vec4()));
    }

    glm::vec4 rgba_to_hsva(glm::vec4 in)
    {
        const float r = in.r;
        const float g = in.g;
        const float b = in.b;
        const float a = in.a;

        float h, s, v;

        auto min = r < g ? r : g;
        min = min  < b ? min  : b;

        auto max = r > g ? r : g;
        max = max  > b ? max  : b;

        auto delta = max - min;

        if (delta > 0)
        {
            if (max == r)
                h = 60 * (fmod(((g - b) / delta), 6));

            else if (max == g)
                h = 60 * (((b - r) / delta) + 2);

            else if (max == b)
                h = 60 * (((r - g) / delta) + 4);

            if (max > 0)
                s = delta / max;
            else
                s = 0;

            v = max;
        }
        else
        {
            h = 0;
            s = 0;
            v = max;
        }

        if (h < 0)
            h += 360;

        return glm::vec4(h / 360.f, s, v, a);
    }

    glm::vec4 hsva_to_rgba(glm::vec4 in)
    {
        const float h = in[0] * 360;
        const float s = in[1];
        const float v = in[2];
        const float a = in[3];

        float c = v * s;
        float h_2 = h / 60;
        float x = c * (1 - std::fabs(std::fmod(h_2, 2) - 1));

        Vector3f rgb;

        if (0 <= h_2 and h_2 < 1)
        {
            rgb = Vector3f(c, x, 0);
        }
        else if (1 <= h_2 and h_2 < 2)
        {
            rgb = Vector3f(x, c, 0);
        }
        else if (2 <= h_2 and h_2 < 3)
        {
            rgb = Vector3f(0, c, x);
        }
        else if (3 <= h_2 and h_2 < 4)
        {
            rgb = Vector3f(0, x, c);
        }
        else if (4 <= h_2 and h_2 < 5)
        {
            rgb = Vector3f(x, 0, c);
        }
        else if (5 <= h_2 and h_2 <= 6)
        {
            rgb = Vector3f(c, 0, x);
        }

        auto m = v - c;

        rgb.r += m;
        rgb.g += m;
        rgb.b += m;

        return glm::vec4(rgb.r, rgb.g, rgb.b, a);
    }

    glm::vec4 rgba_to_cymk(glm::vec4 in)
    {
        float r = in[0];
        float g = in[1];
        float b = in[2];

        float k = 1 - std::max(std::max(r, g), b);
        float c = (1 - r - k) / (1 - k);
        float m = (1 - g - k) / (1 - k);
        float y = (1 - b - k) / (1 - k);

        return glm::vec4(c, m, y, k);
    }

    glm::vec4 cymk_to_rgba(glm::vec4 in)
    {
        const float c = in[0];
        const float y = in[1];
        const float m = in[2];
        const float k = in[3];

        float r = (1 - c) * (1 - k);
        float g = (1 - m) * (1 - k);
        float b = (1 - y) * (1 - k);
        return glm::vec4(r, g, b, 1);
    }

    glm::vec4 hsva_to_cymk(glm::vec4 in)
    {
        return rgba_to_cymk(hsva_to_rgba(in));
    }

    glm::vec4 cymk_to_hsva(glm::vec4 in)
    {
        return rgba_to_hsva(cymk_to_rgba(in));
    }

    RGBA html_code_to_rgba(const std::string& code)
    {
        static auto hex_char_to_int = [](char c) -> uint8_t
        {
            if (c == '0')
                return 0;

            if (c == '1')
                return 1;

            if (c == '2')
                return 2;

            if (c == '3')
                return 3;

            if (c == '4')
                return 4;

            if (c == '5')
                return 5;

            if (c == '6')
                return 6;

            if (c == '7')
                return 7;

            if (c == '8')
                return 8;

            if (c == '9')
                return 9;

            if (c == 'A' or c == 'a')
                return 10;

            if (c == 'B' or c == 'b')
                return 11;

            if (c == 'C' or c == 'c')
                return 12;

            if (c == 'D' or c == 'd')
                return 13;

            if (c == 'E' or c == 'e')
                return 14;

            if (c == 'F' or c == 'f')
                return 15;

            std::stringstream str;
            str << "In html_code_to_rgba: Unrecognized hex character: " << c << std::endl;
            throw std::invalid_argument(str.str());
        };

        static auto hex_component_to_int = [](char left, char right) -> uint8_t
        {
           return hex_char_to_int(left) * 16 + hex_char_to_int(right);
        };

        RGBA out;
        size_t offset = code.front() == '#' ? 1 : 0;

        out.r = hex_component_to_int(code.at(offset + 0), code.at(offset + 1)) / 255.f;
        out.g = hex_component_to_int(code.at(offset + 2), code.at(offset + 3)) / 255.f;
        out.b = hex_component_to_int(code.at(offset + 4), code.at(offset + 5)) / 255.f;

        if (code.size() > offset + 6)
            out.a = hex_component_to_int(code.at(offset + 6), code.at(offset + 7)) / 255.f;
        else
            out.a = 1;

        return out;
    }

    std::string rgba_to_html_code(RGBA in, bool show_alpha = true)
    {
        in.r = glm::clamp<float>(in.r, 0.f, 1.f);
        in.g = glm::clamp<float>(in.g, 0.f, 1.f);
        in.b = glm::clamp<float>(in.b, 0.f, 1.f);
        in.a = glm::clamp<float>(in.a, 0.f, 1.f);

        std::stringstream str;
        str << "#";
        str << std::hex << int(std::round(in.r * 255))
            << std::hex << int(std::round(in.g * 255))
            << std::hex << int(std::round(in.b * 255));

        if (show_alpha)
            str << std::hex << int(std::round(in.a * 255));

        return str.str();
    }
}