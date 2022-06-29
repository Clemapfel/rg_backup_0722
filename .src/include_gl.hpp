// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

using GLNativeHandle = GLuint;

namespace rat::detail
{
    Vector2ui get_viewport_size()
    {
        std::array<GLint, 4> viewport;
        glGetIntegerv(GL_VIEWPORT, viewport.data());
        size_t width = viewport.at(2);
        size_t height = viewport.at(3);
        return Vector2ui(width, height);
    }
}