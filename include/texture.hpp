// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_render.h>

namespace rat
{
    class Texture
    {
        public:
            SDL_Texture* get_native();

        private:
            SDL_Texture* _native;
    };
}

#include <.src/texture.inl>