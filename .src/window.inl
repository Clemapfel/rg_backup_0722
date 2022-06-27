// 
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

namespace rat
{
    void Window::create(
            std::string title,
            size_t width,
            size_t height,
            uint32_t options,
            size_t anti_aliasing_samples)
    {
        if (not _sdl_initialized)
            SDL_Init(SDL_INIT_VIDEO);

        if (width == 0 or height == 0)
        {
            width = std::max<size_t>(width, 1);
            height = std::max<size_t>(height, 1);
        }

        if (_window != nullptr)
            SDL_DestroyWindow(_window);

        uint32_t sdl_options = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

        if (options & FULLSCREEN)
        {
            sdl_options |= SDL_WINDOW_FULLSCREEN;
            _is_fullscreen = true;
        }

        if (options & BORDERLESS)
        {
            sdl_options |= SDL_WINDOW_BORDERLESS;
            _is_borderless = true;
        }

        if (options & RESIZABLE)
        {
            sdl_options |= SDL_WINDOW_RESIZABLE;
            _is_resizable = true;
        }

        if (options & ALWAYS_ON_TOP)
            sdl_options |= ALWAYS_ON_TOP;

        if (options & SKIP_TASKBAR)
            sdl_options |= SKIP_TASKBAR;

        if (options & IS_UTILITY)
            sdl_options |= SDL_WINDOW_UTILITY;

        if (options & IS_TOOLTIP)
            sdl_options |= SDL_WINDOW_TOOLTIP;

        if (options & IS_POPUP_MENU)
            sdl_options |= SDL_WINDOW_POPUP_MENU;

        if (options & GRAB_FOCUS_ON_INIT)
            sdl_options |= SDL_WINDOW_INPUT_GRABBED;

        _window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, sdl_options);
        _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |
                                                    SDL_RENDERER_TARGETTEXTURE);
        SDL_RenderSetVSync(_renderer, true);

        // setup opengl
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        _gl_context = SDL_GL_CreateContext(_window);

        glewExperimental = GL_TRUE;
        GLenum glew_error = glewInit();
        if (glew_error != GLEW_OK)
            std::cerr << "In Window::create: failed to initialize glew" << std::endl;

        SDL_GL_SetSwapInterval(1);

        glViewport(0, 0, width, height);
        SDL_GL_MakeCurrent(_window, _gl_context);
        _is_open = true;
    }

    Time Window::update()
    {
        if (InputHandler::window_closed(get_id()))
        {
            close();
            _is_open = false;
        }

        if (InputHandler::window_maximized(get_id()))
        {
            maximize();
            _is_minimized = false;
            _is_maximized = true;
        }

        if (InputHandler::window_minimized(get_id()))
        {
            minimize();
            _is_maximized = false;
            _is_minimized = true;
        }

        if (InputHandler::window_was_resized(get_id()))
        {
            _is_maximized = false;
            _is_minimized = false;
        }

        _has_focus = InputHandler::window_has_focus(get_id());
        _has_mouse_focus = InputHandler::window_has_mouse_focus(get_id());

        if (InputHandler::window_was_hidden(get_id()))
            _is_hidden = true;

        if (InputHandler::window_was_shown(get_id()))
            _is_hidden = false;

        return _clock.restart();
    }

    Window::Window()
            : _is_borderless(false),
              _is_resizable(false),
              _is_minimized(false),
              _is_maximized(false),
              _is_fullscreen(false),
              _is_hidden(false),
              _has_focus(false),
              _has_mouse_focus(false)
    {}

    Window::~Window()
    {
        close();

        if (_icon != nullptr)
            SDL_FreeSurface(_icon);
    }

    void Window::render(const Renderable *object, Transform transform)
    {
        transform *= _global_transform;
        object->render(this, transform);
    }

    SDL_Window *Window::get_native()
    {
        return _window;
    }

    SDL_Renderer *Window::get_renderer()
    {
        return _renderer;
    }

    SDL_GLContext *Window::get_context()
    {
        return &_gl_context;
    }

    void Window::close()
    {
        _is_borderless = false;
        _is_resizable = false;
        _is_minimized = false;
        _is_maximized = false;
        _is_fullscreen = false;
        _is_hidden = false;
        _has_focus = false;
        _has_mouse_focus = false;

        SDL_DestroyWindow(_window);
        SDL_GL_DeleteContext(_gl_context);
        _is_open = false;
    }

    bool Window::is_open() const
    {
        return _is_open;
    }

    Vector2ui Window::get_size() const
    {
        int width, height;
        SDL_GetRendererOutputSize(_renderer, &width, &height);
        return Vector2ui(width, height);
    }

    Vector2i Window::get_position() const
    {
        int width, height;
        SDL_GetWindowPosition(_window, &width, &height);
        return Vector2i(width, height);
    }

    void Window::set_position(int32_t x, int32_t y)
    {
        SDL_SetWindowPosition(_window, x, y);
    }

    void Window::set_hidden(bool b)
    {
        if (_is_hidden == b)
            return;

        if (b)
        {
            SDL_HideWindow(_window);
            _is_hidden = true;
        } else
        {
            SDL_ShowWindow(_window);
            _is_hidden = false;
        }
    }

    bool Window::is_hidden() const
    {
        return _is_hidden;
    }

    void Window::minimize()
    {
        SDL_MinimizeWindow(_window);
        _is_minimized = true;
    }

    bool Window::is_minimized() const
    {
        return _is_minimized;
    }

    void Window::maximize()
    {
        SDL_MaximizeWindow(_window);
        _is_maximized = true;
    }

    bool Window::is_maximized() const
    {
        return _is_maximized;
    }

    bool Window::has_focus() const
    {
        return _has_focus;
    }

    bool Window::has_mouse_focus() const
    {
        return _has_mouse_focus;
    }

    void Window::set_size(size_t width, size_t height)
    {
        SDL_SetWindowSize(_window, width, height);
    }

    WindowID Window::get_id() const
    {
        return SDL_GetWindowID(_window);
    }

    void Window::set_icon(const std::string &path)
    {
        if (_icon != nullptr)
            SDL_FreeSurface(_icon);

        _icon = IMG_Load(path.c_str());
        if (_icon == nullptr)
        {
            std::cerr << "In ts::Window::set_icon: Unable to load icon from file " << path << std::endl;
            return;
        }

        if (_icon->w != _icon->h)
            std::cerr
                    << "In ts::Window::set_icon: Icon image should be square. Visual corruption may occur because icon \""
                    << path << "\" is of size " << _icon->w << "x" << _icon->h << "." << std::endl;

        SDL_SetWindowIcon(_window, _icon);
    }

    void Window::clear()
    {
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_renderer);
    }

    void Window::flush()
    {
        SDL_RenderFlush(_renderer);
        SDL_RenderPresent(_renderer);
    }
}