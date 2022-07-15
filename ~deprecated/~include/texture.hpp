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

    /// @brief abstract texture
    class Texture
    {
        public:
            virtual ~Texture();

            Texture(const Texture&) = delete;
            Texture& operator=(const Texture&) = delete;

            Texture(Texture&&);
            Texture& operator=(Texture&&);

            Vector2ui get_size() const;
            SDL_Texture* get_native();

            void set_wrap_mode(WrapMode);
            WrapMode get_wrap_mode() const;

            void set_filter_mode(FilterMode);
            FilterMode get_filter_mode() const;

            void bind();
            void unbind();
            bool valid() const;

            GLNativeHandle get_native_handle() const;

        protected:
            Texture(RenderTarget&);

            SDL_Texture* _native;
            SDL_Renderer* _renderer;
            mutable GLNativeHandle _native_handle = 0;
            bool _initialized = false;

        private:
            WrapMode _wrap_mode = WrapMode::REPEAT;
            FilterMode _filter_mode = FilterMode::NEAREST_NEIGHBOUR;
    };

    /// \brief regular texture, cannot be interacted with once it is gpu-side
    class StaticTexture : public Texture
    {
        public:
            StaticTexture(RenderTarget&);

            void create(size_t width, size_t height, RGBA color = RGBA(1, 1, 1, 1));
            void create_from(SDL_Surface*);
            void create_from(Image&);
            void load(const std::string& path);
    };

    /// \brief texture, can be modified once gpu-side
    class DynamicTexture : public Texture
    {
        public:
            DynamicTexture(RenderTarget&);

            void create(size_t width, size_t height);
            void load(const std::string& path);

            void update(Image& image, Vector2ui top_left = {0, 0});
    };

    /// \brief render texture, is a render target
    class RenderTexture : public Texture, public RenderTarget
    {
        public:
            RenderTexture(Window&);

            virtual void create(size_t width, size_t height);

            void bind_as_render_target();
            void unbind_as_render_target();

            virtual Transform get_global_transform() const;
            virtual void set_global_transform(Transform);
            virtual SDL_Renderer* get_renderer();

            void clear(RGBA = RGBA(0, 0, 0, 1));
            virtual void render(const Renderable*, Transform = Transform(), Shader* = nullptr) const;

        private:
            Window* _window;

            bool _currently_bound = false;
            Transform _global_transform = Transform();

            mutable Vector2ui _previous_viewport_size;
            mutable Transform _previous_transform;
    };
}

#include <.src/texture.inl>