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

    // no docs
    SDL_Color as_sdl_color(RGBA);
}

#include <.src/colors.inl>