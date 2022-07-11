// 
// Copyright 2022 Clemens Cords
// Created on 7/11/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/texture.hpp>
#include <include/render_target.hpp>

namespace rat
{
    class RenderTexture : public Texture, public RenderTarget
    {
        public:
            RenderTexture(RenderTarget& window_context);
            ~RenderTexture();

            virtual void create(size_t width, size_t height, RGBA color = RGBA(1, 1, 1, 1));

            void render(const Renderable*, Transform = rat::Transform(), Shader* = nullptr) const override;
            SDL_Renderer* get_renderer() override;
            Transform get_global_transform() const override;
            void set_global_transform(Transform) override;

            void clear(RGBA);

        private:
            GLNativeHandle _framebuffer_id;
            Transform _global_transform = Transform();
    };
}

#include <.src/render_texture.inl>

#include <include/renderable.hpp>

namespace rat
{
    RenderTexture::RenderTexture(RenderTarget &window_context)
            : Texture(window_context, false)
    {
        glGenFramebuffers(1, &_framebuffer_id);
    }

    RenderTexture::~RenderTexture()
    {
        glDeleteFramebuffers(1, &_framebuffer_id);
    }

    void RenderTexture::clear(RGBA color)
    {
        if (not Texture::valid())
            return;

        SDL_UnlockTexture(_native);

        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_id);
        glBindTexture(GL_TEXTURE_2D, get_native_handle());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, get_native_handle(), 0);

        GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, draw_buffers);

        static bool warn_once = true;
        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            if (warn_once)
            {
                std::cerr << "[WARNING] In RenderTexture::clear: framebuffer status incomplete. No rendering will take place." << std::endl;
                warn_once = false;
            }

            return;
        }

        glClearColor(color.r, color.g, color.b, color.a);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RenderTexture::render(const Renderable *renderable, Transform transform, Shader *shader) const
    {
        if (not Texture::valid())
            return;

        SDL_UnlockTexture(_native);

        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer_id);
        glBindTexture(GL_TEXTURE_2D, get_native_handle());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, get_native_handle(), 0);

        GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, draw_buffers);

        static bool warn_once = true;
        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            if (warn_once)
            {
                std::cerr << "[WARNING] In RenderTexture::clear: framebuffer status incomplete. No rendering will take place." << std::endl;
                warn_once = false;
            }

            return;
        }

        renderable->render(this, transform, shader);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    SDL_Renderer *RenderTexture::get_renderer()
    {
        return Texture::_renderer;
    }

    Transform RenderTexture::get_global_transform() const
    {
        return _global_transform;
    }

    void RenderTexture::set_global_transform(Transform transform)
    {
        _global_transform = transform;
    }

    void RenderTexture::create(size_t width, size_t height, RGBA color)
    {
        _native = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, width, height);
    }
}

