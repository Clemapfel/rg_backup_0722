// 
// Copyright 2022 Clemens Cords
// Created on 7/8/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/window.hpp>

namespace rat
{
    class Camera
    {
        public:
            Camera(Window*);

            void move(float x, float y);
            void center_on(Vector2f);
            void set_zoom(float);
            void rotate(Angle);

            Vector2f get_center() const;
            Vector2f apply_to(Vector2f sdl_coords);

        private:
            float _zoom = 1;
            Angle _angle = degrees(0);

            Window* _window;
    };
}

#include <.src/camera.inl>

namespace rat
{
    Camera::Camera(Window* window)
        : _window(window)
    {}

    void Camera::move(float x, float y)
    {
        _window->_global_transform.rotate(degrees(-1 * _angle.as_degrees()), sdl_to_gl_screen_position(get_center()));
        _window->_global_transform.translate(sdl_to_gl_distance(Vector2f(x, y)));
        _window->_global_transform.rotate(degrees(+1 * _angle.as_degrees()), sdl_to_gl_screen_position(get_center()));
    }

    void Camera::center_on(Vector2f position)
    {
        auto size = get_viewport_size();
        _window->_global_transform.translate(sdl_to_gl_distance(Vector2f(-1 * position.x + 0.5 * size.x, -1 * position.y + 0.5 * size.y)));
    }

    void Camera::set_zoom(float zoom)
    {
        _window->_global_transform.scale(1 / _zoom, 1 / _zoom);
        _window->_global_transform.scale(zoom, zoom);
        _zoom = zoom;
    }

    Vector2f Camera::get_center() const
    {
        auto size = get_viewport_size();
        Vector2f center = Vector2f(size.x * 0.5, size.y * 0.5);
        auto center_gl = sdl_to_gl_screen_position(center);
        center_gl = _window->_global_transform.apply_to(center_gl);
        return gl_to_sdl_screen_position(center_gl);
    }

    void Camera::rotate(Angle angle)
    {
        _window->_global_transform.rotate(angle, sdl_to_gl_screen_position(get_center()));
        _angle += angle;
    }

    Vector2f Camera::apply_to(Vector2f sdl_point)
    {
        auto gl_point = sdl_to_gl_screen_position(sdl_point);
        gl_point = _window->_global_transform.apply_to(gl_point);
        return gl_to_sdl_screen_position(gl_point);
    }

}