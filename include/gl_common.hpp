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
    
    Vector2f to_gl_position(Vector2f);
    Vector2f from_gl_position(Vector2f);

    Vector3f to_gl_position(Vector3f);
    Vector3f from_gl_position(Vector3f);
    
    Vector2f to_gl_distance(Vector2f);
    Vector2f from_gl_distance(Vector2f);
}

// #############################################################################

namespace rat
{
    Vector2f get_viewport_size()
    {
        std::array<GLint, 4> viewport = {0, 0, 0, 0};
        glGetIntegerv(GL_VIEWPORT, viewport.data());
        size_t width = viewport.at(2);
        size_t height = viewport.at(3);
        return Vector2f(width, height);
    }

    Vector2f to_gl_position(Vector2f in)
    {
        auto size = get_viewport_size();
        auto centroid = Vector2f(size.x / 2, size.y / 2);

        auto out = centroid - in;
        out.x = 1 - out.x;
        out.x /= size.x / 2;
        out.y /= size.y / 2;
        return out;
    }

    Vector2f from_gl_position(Vector2f in)
    {
        auto size = get_viewport_size();
        auto centroid = Vector2f(size.x / 2, size.y / 2);

        auto out = in;
        out.x *= size.x / 2;
        out.y *= size.y / 2;
        out.x = 1 - out.x;
        out = centroid - out;
        return out;
    }

    Vector2f to_gl_distance(Vector2f distance)
    {
        auto size = get_viewport_size();
        return Vector2f(distance.x / (size.x * 0.5) , -1 * distance.y / (size.y * 0.5));
    }

    Vector2f from_gl_distance(Vector2f distance)
    {
        auto size = get_viewport_size();
        return Vector2f(distance.x * (size.x * 0.5), -1 * distance.y * (size.y * 0.5));
    }
}


