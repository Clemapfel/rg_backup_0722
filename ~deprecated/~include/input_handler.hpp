// 
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_events.h>
#include <glm/geometric.hpp>

#include <map>
#include <array>
#include <set>

#include <include/keycodes.hpp>
#include <include/vector.hpp>

namespace rat
{
    using ControllerID = size_t;
    using WindowID = int32_t;

    union  InputHandler
    {
        public:
            static void update();

            static bool is_down(KeyboardKey);
            static bool is_down(MouseButton);
            static bool is_down(ControllerButton, ControllerID);

            static bool has_state_changed(KeyboardKey);
            static bool has_state_changed(MouseButton);
            static bool has_state_changed(ControllerButton, ControllerID);

            static bool was_pressed(KeyboardKey);
            static bool was_pressed(MouseButton);
            static bool was_pressed(ControllerButton, ControllerID);

            static bool was_released(KeyboardKey);
            static bool was_released(MouseButton);
            static bool was_released(ControllerButton, ControllerID);

            static Vector2f get_cursor_position();
            static Vector2f get_scrollwheel_offset();

            static Vector2f get_controller_axis_left(ControllerID = 0);
            static Vector2f get_controller_axis_right(ControllerID = 0);

            static float get_controller_trigger_left(ControllerID = 0);
            static float get_controller_trigger_right(ControllerID = 0);

            static bool window_closed(WindowID);
            static bool window_minimized(WindowID);
            static bool window_maximized(WindowID);
            static bool window_has_mouse_focus(WindowID);
            static bool window_has_focus(WindowID);
            static bool window_was_resized(WindowID);
            static bool window_was_moved(WindowID);
            static bool window_was_restored(WindowID);
            static bool window_was_hidden(WindowID);
            static bool window_was_shown(WindowID);

            static bool exit_requested();

        private:
            static inline bool _initialized = false;
            static void initialize();

            struct KeyboardState
            {
                std::set<KeyboardKey> pressed;
            };

            struct MouseState
            {
                std::set<MouseButton> pressed;

                Vector2f position;
                Vector2f scroll_delta;
            };

            struct ControllerState
            {
                std::set<ControllerButton> pressed;

                Vector2f axis_left;
                Vector2f axis_right;

                float trigger_left;
                float trigger_right;
            };

            struct WindowState
            {
                bool should_close = false;
                bool should_minimize = false;
                bool should_maximize = false;
                bool has_mouse_focus = false;
                bool has_focus = false;
                bool was_shown = false;
                bool was_hidden = false;
                bool was_resized = false;
                bool was_moved = false;
                bool was_restored = false;
            };

            static inline bool _exit_requested = false;
            static inline std::array<KeyboardState, 2> _keyboard_state;
            static inline std::array<MouseState, 2> _mouse_state;
            static inline std::map<ControllerID, std::array<ControllerState, 2>> _controller_states;
            static inline std::map<WindowID, WindowState> _window_state;
            template<typename... Ts>
            static void print_warning(Ts...);
    };
}

#include <.src/input_handler.inl>