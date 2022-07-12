// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#include <include/render_target.hpp>
#include <include/image.hpp>

namespace rat
{
    Texture::Texture(RenderTarget& target)
        : _renderer(target.get_renderer())
    {}

    Texture::~Texture()
    {
        if (_initialized && _native != nullptr)
            SDL_DestroyTexture(_native);
    }

    Texture::Texture(Texture&& other)
    {
        _native = other._native;
        _initialized = other._initialized;
        _wrap_mode = other._wrap_mode;
        _filter_mode = other._filter_mode;

        other._initialized = false;
        other._native = nullptr;
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
        return _initialized and _native != nullptr;
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

    // STATIC TEXTURE

    StaticTexture::StaticTexture(RenderTarget& target)
        : Texture(target)
    {}

    void StaticTexture::create_from(SDL_Surface* surface)
    {
        _native = SDL_CreateTextureFromSurface(_renderer, surface);
        _initialized = true;
    }

    void StaticTexture::create_from(Image& image)
    {
        auto surface = image.operator SDL_Surface*();
        _native = SDL_CreateTextureFromSurface(_renderer, surface);
        _initialized = true;
        SDL_FreeSurface(surface);
    }

    void StaticTexture::load(const std::string &path)
    {
        _native = IMG_LoadTexture(_renderer, path.c_str());
        if (_native == nullptr)
            std::cerr << "[WARNING] In Texture::load: Unable to load image at " << path << std::endl;

        _initialized = true;
    }

    void StaticTexture::create(size_t width, size_t height, RGBA color)
    {
        auto image = Image();
        image.create(width, height, color);
        create_from(image);
        _initialized = true;
    }

    // DYNAMIC TEXTURE

    DynamicTexture::DynamicTexture(RenderTarget& target)
        : Texture(target)
    {}

    void DynamicTexture::create(size_t width, size_t height)
    {
        _native = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (_native != nullptr)
            _initialized = true;
    }

    // RENDER TEXTURE

    RenderTexture::RenderTexture(Window& target)
        : Texture(target), _window(&target)
    {}

    void RenderTexture::create(size_t width, size_t height)
    {
        _native = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
        if (_native != nullptr)
            _initialized = true;
    }

    SDL_Renderer* RenderTexture::get_renderer()
    {
        return _window->get_renderer();
    }

    Transform RenderTexture::get_global_transform() const
    {
        return _global_transform;
    }

    void RenderTexture::set_global_transform(Transform transform)
    {
        _global_transform = transform;
    }

    void RenderTexture::bind_as_render_target()
    {
        if (not _initialized)
            std::cerr << "[WARNING] In RenderTexture::bind_as_render_target: Trying to bind a texture that has not been created yet." << std::endl;

        _previous_viewport_size = get_viewport_size();
        _previous_transform = _window->get_global_transform();

        auto size = get_size();
        glViewport(0, 0, size.x, size.y);
        _window->set_global_transform(_global_transform);

        SDL_SetRenderTarget(_window->get_renderer(), get_native());
        _currently_bound = true;
    }

    void RenderTexture::render(const Renderable* renderable, Transform transform, Shader* shader) const
    {
        if (not _currently_bound)
            std::cerr << "[WARNING] In RenderTexture::render: Trying to render to a texture even though it is not currently bounds as a render target. Use `bind_as_render_target` before calling `render`." << std::endl;

        transform = transform.combine_with(_global_transform);
        renderable->render(_window, transform, shader);
    }

    void RenderTexture::clear(RGBA color)
    {
        if (not _currently_bound)
            std::cerr << "[WARNING] In RenderTexture::clear: Trying to clear a texture even though it is not currently bounds as a render target. Use `bind_as_render_target` before calling `render`." << std::endl;

        SDL_SetRenderDrawColor(_window->get_renderer(), color.r * 255, color.g * 255, color.b * 255, color.a * 255);
        SDL_RenderClear(_window->get_renderer());
    }

    void RenderTexture::unbind_as_render_target()
    {
        //SDL_GL_SwapWindow(_window->get_native());
        //SDL_RenderFlush(_window->get_renderer());
        //SDL_RenderPresent(_window->get_renderer());

        glViewport(0, 0, _previous_viewport_size.x, _previous_viewport_size.y);

        _window->set_global_transform(_previous_transform);
        SDL_SetRenderTarget(_window->get_renderer(), nullptr);
        _currently_bound = false;
    }
}