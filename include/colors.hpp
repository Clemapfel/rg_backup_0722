// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <glm/glm.hpp>

namespace rat
{
    using RGBA = glm::vec<4, float>;

    struct HSVA
    {
        float h = 0,
              s = 0,
              v = 0,
              a = 1;

        HSVA() = default;
        HSVA(float, float, float, float);

        // rgba -> hsva
        HSVA(RGBA);

        // hsva -> rgba
        operator RGBA();
    };

    SDL_Color as_sdl_color(RGBA);
}

#include <.src/colors.inl>