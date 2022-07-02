// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#include <include/render_target.hpp>
#include <include/image.hpp>

namespace rat
{
    Texture::Texture(SDL_Renderer* renderer, bool mipmap_enabled)
        : _renderer(renderer), _mipmap_enabled(mipmap_enabled)
    {}

    Texture::~Texture()
    {
        SDL_DestroyTexture(_native);
        _renderer = nullptr;
    }

    SDL_Texture* Texture::get_native()
    {
        return _native;
    }

    void Texture::create(SDL_Surface* surface)
    {
        _native = SDL_CreateTextureFromSurface(_renderer, surface);
    }

    void Texture::create(const std::string &path)
    {
        _native = IMG_LoadTexture(_renderer, path.c_str());
    }

    void Texture::create(Image& image)
    {
        auto* surface = SDL_CreateRGBSurfaceFrom(image._data.data(), image._width, image._height, 32, 4*image._width, image.r_mask, image.g_mask, image.b_mask, image.a_mask);
        _native = SDL_CreateTextureFromSurface(_renderer, surface);
        SDL_FreeSurface(surface);
    }

    void Texture::create(size_t width, size_t height, RGBA color)
    {
        auto image = Image();
        image.create(width, height, color);
        create(image);
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