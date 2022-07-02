// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_render.h>
#include <.src/include_gl.hpp>
#include <include/image.hpp>

namespace rat
{

    enum WrapMode
    {
        ZERO = 0, // GL_CLAMP_TO_BORDER with 0 padding
        ONE = 1,  // GL_CLAMP_TO_BORDER with 1 padding
        REPEAT = GL_REPEAT,
        MIRROR = GL_MIRRORED_REPEAT,
        STRETCH = GL_CLAMP_TO_EDGE
    };

    enum FilterMode
    {
        NEAREST_NEIGHBOUR, // GL_NEAREST or GL_MIPMAP
        LINEAR,            // GL_LINEAR or GL_LINEAR_MIPMAP_LINEAR
        CUBIC              // GL_CUBIC_IMG or GL_CUBIC_MIPMAP_LINEAR_IMG
    };

    class Texture
    {
        public:
            Texture(SDL_Renderer*, bool enable_mipmap = false);
            ~Texture();

            void create(size_t width, size_t height, RGBA color = RGBA(1, 1, 1, 1));
            void create(const std::string& path);
            void create(Image&);
            void create(SDL_Surface*);

            Vector2ui get_size() const;
            SDL_Texture* get_native();

            void set_wrap_mode(WrapMode);
            WrapMode get_wrap_mode() const;

            void set_filer_mode(FilterMode);
            FilterMode get_filter_mode() const;

            void bind();
            void unbind();

        private:
            SDL_Renderer* _renderer;
            SDL_Texture* _native;

            bool _mipmap_enabled;
            WrapMode _wrap_mode = WrapMode::REPEAT;
            FilterMode _filter_mode = FilterMode::NEAREST_NEIGHBOUR;
    };
}

#include <.src/texture.inl>