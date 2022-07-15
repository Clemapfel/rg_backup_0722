// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <include/vector.hpp>

static inline bool GL_INITIALIZED = false;
using GLNativeHandle = GLuint;

namespace rat
{
    Vector2f get_viewport_size();
}

