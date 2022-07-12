// 
// Copyright 2022 Clemens Cords
// Created on 7/13/22 by clem (mail@clemens-cords.com)
//

#include <.src/include_gl.hpp>
#include <include/vector.hpp>

namespace rat
{
    Vector2f get_viewport_size();

    Vector2f sdl_to_gl_screen_position(Vector2f in);
    Vector2f gl_to_sdl_screen_position(Vector2f in);

    Vector3f sdl_to_gl_screen_position(Vector3f in);
    Vector3f gl_to_sdl_screen_position(Vector3f in);

    Vector2f sdl_to_gl_texture_coordinates(Vector2f in);
    Vector2f gl_to_sdl_texture_coordinates(Vector2f in);

    Vector2f sdl_to_gl_distance(Vector2f distance);
    Vector2f gl_to_sdl_distance(Vector2f distance);
}

#include <.src/opengl_common.inl>