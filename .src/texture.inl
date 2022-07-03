// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#include <include/render_target.hpp>
#include <include/image.hpp>

namespace rat
{
    Texture::Texture(RenderTarget& renderer, bool mipmap_enabled)
        : _renderer(renderer.get_renderer()), _mipmap_enabled(mipmap_enabled)
    {}

    Texture::~Texture()
    {
        if (_initialized)
            SDL_DestroyTexture(_native);

        _renderer = nullptr;
    }

    SDL_Texture* Texture::get_native()
    {
        return _native;
    }

    void Texture::create_from(SDL_Surface* surface)
    {
        _native = SDL_CreateTextureFromSurface(_renderer, surface);
        _initialized = true;
    }

    void Texture::load(const std::string &path)
    {
        _native = IMG_LoadTexture(_renderer, path.c_str());
        _initialized = true;
    }

    void Texture::create(size_t width, size_t height, RGBA color)
    {
        auto image = Image();
        image.create(width, height, color);
        create_from(image);
        _initialized = true;
    }

    void Texture::bind()
    {
        SDL_GL_BindTexture(_native, nullptr, nullptr);
    }

    void Texture::unbind()
    {
        SDL_GL_UnbindTexture(_native);
    }

    Vector2ui Texture::get_size() const
    {
        int width, height;
        SDL_QueryTexture(_native, nullptr, nullptr, &width, &height);
        return Vector2f(width, height);
    }
}