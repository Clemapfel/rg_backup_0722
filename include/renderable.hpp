// 
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/transform.hpp>

namespace rat
{
    class RenderTarget;
    struct Renderable
    {
        virtual void render(RenderTarget* target, Transform transform) const = 0;
    };
}