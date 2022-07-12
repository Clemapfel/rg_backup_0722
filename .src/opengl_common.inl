// 
// Copyright 2022 Clemens Cords
// Created on 7/13/22 by clem (mail@clemens-cords.com)
//

#include <array>

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

    Vector2f sdl_to_gl_screen_position(Vector2f in)
    {
        auto size = get_viewport_size();
        auto centroid = Vector2f(size.x / 2, size.y / 2);

        auto out = centroid - in;
        out.x = 1 - out.x;
        out.x /= size.x / 2;
        out.y /= size.y / 2;
        return out;
    }

    Vector2f gl_to_sdl_screen_position(Vector2f in)
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

    Vector3f sdl_to_gl_screen_position(Vector3f in)
    {
        auto size = get_viewport_size();
        auto centroid = Vector3f(size.x / 2, size.y / 2, 0);

        auto out = centroid - in;
        out.x = 1 - out.x;
        out.x /= size.x / 2;
        out.y /= size.y / 2;
        return out;
    }

    Vector3f gl_to_sdl_screen_position(Vector3f in)
    {
        auto size = get_viewport_size();
        auto centroid = Vector3f(size.x / 2, size.y / 2, 0);

        auto out = in;
        out.x *= size.x / 2;
        out.y *= size.y / 2;
        out.x = 1 - out.x;
        out = centroid - out;
        return out;
    }

    Vector2f sdl_to_gl_texture_coordinates(Vector2f in)
    {
        in.x = 1 - in.x; // sdl texture are x-flipped
        return in;
    }

    Vector2f gl_to_sdl_texture_coordinates(Vector2f in)
    {
        in.x = 1 - in.x;
        return in;
    }

    Vector2f sdl_to_gl_distance(Vector2f distance)
    {
        auto size = get_viewport_size();
        return Vector2f(distance.x / (size.x * 0.5) , -1 * distance.y / (size.y * 0.5));
    }

    Vector2f gl_to_sdl_distance(Vector2f distance)
    {
        auto size = get_viewport_size();
        return Vector2f(distance.x * (size.x * 0.5), -1 * distance.y * (size.y * 0.5));
    }
}