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
        if (_initialized && _native != nullptr)
            SDL_DestroyTexture(_native);

        _renderer = nullptr;
    }

    Texture::Texture(Texture&& other)
    {
        _native = other._native;
        _initialized = other._initialized;
        _renderer = other._renderer;
        _mipmap_enabled = other._mipmap_enabled;
        _wrap_mode = other._wrap_mode;
        _filter_mode = other._filter_mode;

        other._initialized = false;
        other._native = nullptr;
        other._renderer = nullptr;
    }

    SDL_Texture* Texture::get_native()
    {
        return _native;
    }

    WrapMode Texture::get_wrap_mode() const
    {
        return _wrap_mode;
    }

    void Texture::set_wrap_mode(WrapMode mode)
    {
        _wrap_mode = mode;
    }

    void Texture::set_filter_mode(FilterMode mode)
    {
        _filter_mode = mode;
    }

    FilterMode Texture::get_filter_mode() const
    {
        return _filter_mode;
    }

    void Texture::create_from(SDL_Surface* surface)
    {
        _native = SDL_CreateTextureFromSurface(_renderer, surface);
        _initialized = true;
    }

    void Texture::create_from(Image& image)
    {
        auto surface = image.operator SDL_Surface*();
        _native = SDL_CreateTextureFromSurface(_renderer, surface);
        _initialized = true;
        SDL_FreeSurface(surface);
    }

    void Texture::load(const std::string &path)
    {
        _native = IMG_LoadTexture(_renderer, path.c_str());
        if (_native == nullptr)
            std::cerr << "[WARNING] In Texture::load: Unable to load image at " << path << std::endl;

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
        if (not valid())
            return;

        glBindTexture(GL_TEXTURE_2D, get_native_handle());
        //SDL_GL_BindTexture(_native, nullptr, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, get_wrap_mode());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  get_wrap_mode());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, get_filter_mode());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, get_filter_mode());
    }

    void Texture::unbind()
    {
        if (not valid())
            return;

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    bool Texture::valid() const
    {
        return _initialized and _renderer != nullptr and _native != nullptr;
    }

    Vector2ui Texture::get_size() const
    {
        int width, height;
        SDL_QueryTexture(_native, nullptr, nullptr, &width, &height);
        return Vector2f(width, height);
    }

    GLNativeHandle Texture::get_native_handle() const
    {
        if (_native_handle != 0)
            return _native_handle;

        GLint id;
        SDL_GL_BindTexture(_native, nullptr, nullptr);

        glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);

        _native_handle = id;

        SDL_GL_UnbindTexture(_native);
        glBindTexture(GL_TEXTURE_2D, 0);

        return id;
    }
}