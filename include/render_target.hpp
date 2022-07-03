//
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_render.h>

namespace rat
{
    class RenderTarget
    {
        public:
            virtual SDL_Renderer* get_renderer() = 0;
    };
}
