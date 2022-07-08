// 
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <.src/include_gl.hpp>

#include <string>
#include <include/input_handler.hpp>
#include <include/render_target.hpp>
#include <include/renderable.hpp>
#include <include/vector.hpp>
#include <include/time.hpp>

namespace rat
{
    enum WindowOptions : uint32_t
    {
        DEFAULT = 0,
        FULLSCREEN = 1 << 1,
        BORDERLESS = 1 << 2,
        RESIZABLE  = 1 << 3,
        ALWAYS_ON_TOP = 1 << 4,
        SKIP_TASKBAR = 1 << 5,
        IS_UTILITY = 1 << 6,
        IS_TOOLTIP = 1 << 7,
        IS_POPUP_MENU = 1 << 8,
        GRAB_FOCUS_ON_INIT = 1 << 9
    };

    using WindowID = int32_t;
    class Window : public RenderTarget
    {
        friend class Camera;

        public:
            Window();
            ~Window();

            Time update();

            void render(const Renderable*, Transform = rat::Transform());
            void clear();
            void flush();

            void create(std::string title, size_t width, size_t height, uint32_t options = DEFAULT, size_t anti_aliasing_samples = 8);
            void set_icon(const std::string& path);

            void close();
            bool is_open() const;

            Vector2ui get_size() const;
            void set_size(size_t width, size_t height);

            void set_position(int32_t x, int32_t y);
            Vector2i get_position() const;

            void set_hidden(bool);
            bool is_hidden() const;

            void minimize();
            bool is_minimized() const;

            void maximize();
            bool is_maximized() const;

            bool has_focus() const;
            bool has_mouse_focus() const;

            WindowID get_id() const;
            SDL_Window* get_native();
            SDL_Renderer* get_renderer() override;
            SDL_GLContext* get_context();

            Transform get_global_transform() const override;

        //private:
            static inline bool _sdl_initialized = false;

            SDL_Window* _window = nullptr;
            SDL_Renderer* _renderer;
            SDL_GLContext _gl_context;

            SDL_Surface* _icon = nullptr;

            Transform _global_transform; // camera state

            Clock _clock;

            bool _is_open = false;
            bool _is_borderless;
            bool _is_resizable;
            bool _is_minimized;
            bool _is_maximized;
            bool _is_fullscreen;
            bool _is_hidden;
            bool _has_focus;
            bool _has_mouse_focus;
    };
}

#include <.src/window.inl>