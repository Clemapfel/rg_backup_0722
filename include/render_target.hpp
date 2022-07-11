//
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_render.h>

#include <include/transform.hpp>
#include <include/shader.hpp>

namespace rat
{
    class Renderable;
    class RenderTarget
    {
        public:
            virtual Transform& get_global_transform() = 0;
            virtual SDL_Renderer* get_renderer() = 0;
            virtual void render(const Renderable*, Transform, Shader*) const = 0;
    };
}
