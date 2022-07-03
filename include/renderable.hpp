// 
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/transform.hpp>
#include <include/shader.hpp>

namespace rat
{
    class RenderTarget;
    struct Renderable
    {
        /// \brief render
        /// \param render_target: render target
        /// \param transform: transform to be applied to position attribute of vertices
        /// \param shader: shader to be used, or nullptr to use the identity shader
        virtual void render(RenderTarget&, Transform = Transform(), Shader* = nullptr) const = 0;
    };
}