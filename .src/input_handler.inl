//
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#include <iostream>
#include <SDL2/SDL.h>

namespace rat
{
    template<typename... Ts>
    void InputHandler::print_warning(Ts... in)
    {
        auto print = [](auto in){
            std::cerr << in;
        };

        (print(in), ...);
        std::cerr << std::endl;
    }

    void InputHandler::initialize()
    {
        if (_initialized)
            return;

        if (SDL_Init(SDL_INIT_EVENTS) != 0)
        {
            std::cerr << "In InputHandler::initialize: failed to initialize: " << SDL_GetError() << std::endl;
        }
    }

    void InputHandler::update()
    {
        _keyboard_state[0] = _keyboard_state[1];
        _mouse_state[0] = _mouse_state[1];
        _mouse_state[1].scroll_delta = Vector2f(0, 0);

        for (auto& pair : _controller_states)
            pair.second[0] = pair.second[1];

        for (auto& pair : _window_state)
        {
            bool has_focus = pair.second.has_focus;
            bool has_mouse_focus = pair.second.has_mouse_focus;
            pair.second = WindowState();
            pair.second.has_focus = has_focus;
            pair.second.has_mouse_focus = has_mouse_focus;
        }

        auto event = SDL_Event();
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                _keyboard_state[1].pressed.insert((KeyboardKey) event.key.keysym.sym);
            }
            else if (event.type == SDL_KEYUP)
            {
                _keyboard_state[1].pressed.erase((KeyboardKey) event.key.keysym.sym);
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                _mouse_state[1].pressed.insert((MouseButton) event.button.button);
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                _mouse_state[1].pressed.insert((MouseButton) event.button.button);
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                _mouse_state[1].position = Vector2f(event.motion.x, event.motion.x);
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                bool flipped = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED;
                _mouse_state[1].scroll_delta.x = event.wheel.x * (flipped ? -1 : 1);
                _mouse_state[1].scroll_delta.y = event.wheel.y * (flipped ? -1 : 1);
            }
            else if (event.type == SDL_CONTROLLERDEVICEADDED)
            {
                auto id = event.cdevice.which;
                std::cout << "[LOG] Controller " << id << " connected" << std::endl;

                _controller_states.insert_or_assign(id, std::array<ControllerState, 2>());
            }
            else if (event.type == SDL_CONTROLLERDEVICEREMOVED)
            {
                auto id = event.cdevice.which;
                std::cout << "[LOG] Controller " << id << " disconnected" << std::endl;

                _controller_states.erase(id);
            }
            else if (event.type == SDL_CONTROLLERBUTTONDOWN)
            {
                _controller_states[event.cbutton.which][1].pressed.insert((ControllerButton) event.cbutton.button);
            }
            else if (event.type == SDL_CONTROLLERBUTTONUP)
            {
                _controller_states[event.cbutton.which][1].pressed.erase((ControllerButton) event.cbutton.button);
            }
            else if (event.type == SDL_CONTROLLERAXISMOTION)
            {
                auto& state = _controller_states[event.caxis.which][1];

                if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX)
                    state.axis_left.x = event.caxis.value / 32767.f;

                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY)
                    state.axis_left.y = event.caxis.value / 32767.f;

                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX)
                    state.axis_right.x = event.caxis.value / 32767.f;

                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
                    state.axis_right.y = event.caxis.value / 32767.f;

                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT)
                    state.trigger_left = event.caxis.value / 32767.f;

                else if (event.caxis.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
                    state.trigger_right = event.caxis.value / 32767.f;

                else
                {
                    static bool already_printed = false;
                    if (not already_printed)
                    {
                        print_warning("In ts::InputHandler.update: controller axis ", event.caxis.axis, " unsupported");
                        already_printed = true;
                    }
                }
            }
            if (event.type == SDL_WINDOWEVENT)
            {
                WindowID id = event.window.windowID;
                if (_window_state.find(id) == _window_state.end())
                    _window_state.emplace(id, WindowState());

                if (event.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    _window_state.at(id).should_close = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_SHOWN)
                {
                    _window_state.at(id).was_shown = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_HIDDEN)
                {
                    _window_state.at(id).was_hidden = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    _window_state.at(id).was_resized = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_MOVED)
                {
                    _window_state.at(id).was_moved = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_MOVED)
                {
                    _window_state.at(id).was_restored = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                {
                    _window_state.at(id).should_minimize = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
                {
                    _window_state.at(id).should_minimize = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_ENTER)
                {
                    _window_state.at(id).has_mouse_focus = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_LEAVE)
                {
                    _window_state.at(id).has_mouse_focus = false;
                }
                else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    _window_state.at(id).has_focus = false;
                }
                else if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                    _window_state.at(id).has_focus = true;
                }
            }
            else if (event.type == SDL_QUIT)
            {
                _exit_requested = true;
            }
        }
    }

    bool InputHandler::is_down(KeyboardKey keyboard_key)
    {
        initialize();
        return _keyboard_state[1].pressed.find(keyboard_key) != _keyboard_state[1].pressed.end();
    }

    bool InputHandler::is_down(MouseButton mouse_button)
    {
        initialize();
        return _mouse_state[1].pressed.find(mouse_button) != _mouse_state[1].pressed.end();
    }

    bool InputHandler::is_down(ControllerButton controller_button, ControllerID id)
    {
        initialize();
        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::is_down: No controller with id ", id, " connected, returning false");
                already_printed.insert(id);
            }
            return false;
        }

        return it->second[1].pressed.find(controller_button) != it->second[1].pressed.end();
    }

    bool InputHandler::has_state_changed(KeyboardKey keyboard_key)
    {
        initialize();

        bool before = _keyboard_state[0].pressed.find(keyboard_key) != _keyboard_state[0].pressed.end();
        bool after = _keyboard_state[1].pressed.find(keyboard_key) != _keyboard_state[1].pressed.end();


        return before != after;
    }

    bool InputHandler::has_state_changed(MouseButton mouse_button)
    {
        initialize();
        bool before = _mouse_state[0].pressed.find(mouse_button) != _mouse_state[0].pressed.end();
        bool after = _mouse_state[1].pressed.find(mouse_button) != _mouse_state[1].pressed.end();

        return before != after;
    }

    bool InputHandler::has_state_changed(ControllerButton controller_button, ControllerID id)
    {
        initialize();
        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::has_state_changed: No controller with id ", id, " connected, returning false");
                already_printed.insert(id);
            }
            return false;
        }

        bool before = it->second[0].pressed.find(controller_button) != it->second[0].pressed.end();
        bool after = it->second[1].pressed.find(controller_button) != it->second[1].pressed.end();

        return before != after;
    }

    bool InputHandler::was_pressed(KeyboardKey keyboard_key)
    {
        initialize();
        return is_down(keyboard_key) and has_state_changed(keyboard_key);
    }

    bool InputHandler::was_pressed(MouseButton mouse_button)
    {
        initialize();
        return is_down(mouse_button) and has_state_changed(mouse_button);
    }

    bool InputHandler::was_pressed(ControllerButton controller_button, ControllerID id)
    {
        initialize();
        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::was_pressed: No controller with id ", id, " connected, returning false");
                already_printed.insert(id);
            }
            return false;
        }

        return is_down(controller_button, id) and has_state_changed(controller_button, id);
    }

    bool InputHandler::was_released(KeyboardKey keyboard_key)
    {
        initialize();
        return not was_pressed(keyboard_key);
    }

    bool InputHandler::was_released(MouseButton mouse_button)
    {
        initialize();
        return not was_pressed(mouse_button);
    }

    bool InputHandler::was_released(ControllerButton controller_button, ControllerID id)
    {
        initialize();
        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::was_released: No controller with id ", id, " connected, returning false");
                already_printed.insert(id);
            }
            return false;
        }

        return not was_pressed(controller_button, id);
    }

    Vector2f InputHandler::get_cursor_position()
    {
        initialize();
        return _mouse_state[1].position;
    }

    Vector2f InputHandler::get_scrollwheel_offset()
    {
        initialize();
        return _mouse_state[1].scroll_delta;
    }

    Vector2f InputHandler::get_controller_axis_left(ControllerID id)
    {
        initialize();

        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::get_controller_axis_left: No controller with id ", id, " connected, returning (0, 0)");
                already_printed.insert(id);
            }
            return Vector2f(0, 0);
        }
        return it->second[1].axis_left;
    }

    Vector2f InputHandler::get_controller_axis_right(ControllerID id)
    {
        initialize();

        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::get_controller_axis_right: No controller with id ", id, " connected, returning (0, 0)");
                already_printed.insert(id);
            }
            return Vector2f(0, 0);
        }
        return it->second[1].axis_right;
    }

    float InputHandler::get_controller_trigger_left(ControllerID id)
    {
        initialize();

        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::get_controller_trigger_left: No controller with id ", id, " connected, returning 0");
                already_printed.insert(id);
            }
            return 0;
        }
        return it->second[1].trigger_left;
    }

    float InputHandler::get_controller_trigger_right(ControllerID id)
    {
        initialize();

        auto it = _controller_states.find(id);
        if (it == _controller_states.end())
        {
            static auto already_printed = std::set<size_t>();
            if (already_printed.find(id) == already_printed.end())
            {
                print_warning("In ts::InputHandler::get_controller_trigger_right: No controller with id ", id, " connected, returning 0");
                already_printed.insert(id);
            }
            return 0;
        }
        return it->second[1].trigger_right;
    }

    bool InputHandler::window_closed(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.should_close;
        else
            return false;
    }

    bool InputHandler::window_minimized(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.should_minimize;
        else
            return false;
    }

    bool InputHandler::window_maximized(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.should_maximize;
        else
            return false;
    }

    bool InputHandler::window_has_mouse_focus(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.has_mouse_focus;
        else
            return false;
    }

    bool InputHandler::window_has_focus(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.has_focus;
        else
            return false;
    }

    bool InputHandler::window_was_resized(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.was_resized;
        else
            return false;
    }

    bool InputHandler::window_was_moved(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.was_moved;
        else
            return false;
    }

    bool InputHandler::window_was_restored(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.was_restored;
        else
            return false;
    }

    bool InputHandler::window_was_hidden(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.was_hidden;
        else
            return false;
    }

    bool InputHandler::window_was_shown(WindowID id)
    {
        initialize();

        auto it = _window_state.find(id);
        if (it != _window_state.end())
            return it->second.was_shown;
        else
            return false;
    }

    bool InputHandler::exit_requested()
    {
        initialize();

        return _exit_requested;
    }
}

