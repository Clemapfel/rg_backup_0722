// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <glm/glm.hpp>

namespace rat
{
    /// \brief colors in RGBA representation, values in [0, 1]
    using RGBA = glm::vec<4, float>;

    /// \brief color in HSVA representation, values in [0, 1]
    struct HSVA
    {
        /// \brief hue component;
        float h = 0;

        /// \brief saturation component;
        float s = 0;

        /// \brief value component;
        float v = 0;

        /// \brief alpha component;
        float a = 1;

        /// \brief default ctor, initializes to HSVA(0, 0, 0, 1)
        HSVA() = default;

        /// \brief construct
        /// \param h: hue
        /// \param s: saturation
        /// \param v: value
        /// \param a: transparency
        HSVA(float h, float s, float v, float a);

        /// \brief convert RGBA to HSVA
        /// \param HSVA
        HSVA(RGBA);

        /// \brief convert HSVA to RGBA
        /// \returns RGBA
        operator RGBA();
    };
}

// ##############################################################################################

#include <include/vector.hpp>

namespace rat
{
    HSVA::HSVA(float h_in, float s_in, float v_in, float a_in)
            : h(h_in), s(s_in), v(v_in), a(a_in)
    {}

    HSVA::HSVA(RGBA in)
    {
        auto rgb_to_hsv = [alpha = this->a](auto r, auto g, auto b) -> glm::vec<4, float>
        {
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

            return HSVA(h / 360.f, s, v, alpha);
        };

        auto out = rgb_to_hsv(in.r, in.g, in.b);
        h = out.x;
        s = out.y;
        v = out.z;
        a = out.a;
    }

    HSVA::operator RGBA()
    {
        auto h = this->h;

        h *= 360;
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

        return RGBA(rgb.r, rgb.g, rgb.b, a);
    }
}