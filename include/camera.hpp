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
            Vector2f _translation_offset = Vector2f(0, 0);

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
        static glm::vec3 position = glm::vec3(0.0f, 0.0f,  1.0f);
        static glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
        static glm::vec3 up_normal = glm::vec3(0.0f, 1.0f,  0.0f);

        //_window->_global_transform.translate(Vector2f(x, y));
        //_translation_offset += Vector2f(x, y);
    }

    void Camera::center_on(Vector2f position)
    {
        auto size = get_viewport_size();
        auto screen_pos = apply_to(position);
        auto offset = Vector2f(-1 * screen_pos.x + 0.5 * size.x, -1 * screen_pos.y + 0.5 * size.y);
        _window->_global_transform.translate(offset);
        _translation_offset += offset;
    }

    void Camera::set_zoom(float zoom)
    {
        auto size = get_viewport_size();
        _window->_global_transform._transform = glm::lookAt(
                glm::vec3(0.5 * size.x, 0.5 * size.y, 0),
                glm::vec3(0.5 * size.x, 0.5 * size.y, 0),
                glm::vec3(0, 0, 1));
        //auto size = get_viewport_size();

        //auto& transform = _window->_global_transform._transform;

        // reset to origin
        //_window->_global_transform.translate(Vector2f(-1, -1) * _translation_offset);
        //_window->_global_transform.translate(Vector2f(-0.5, -0.5) * size);

        //_window->_global_transform.scale(1 / _zoom, 1 / _zoom);
        //_window->_global_transform.scale(zoom, zoom);

        //_window->_global_transform.translate(Vector2f(+0.5, +0.5) * size);
        //_window->_global_transform.translate(_translation_offset);

        _zoom = zoom;
    }

    Vector2f Camera::get_center() const
    {
        auto size = get_viewport_size();
        Vector2f center = Vector2f(size.x * 0.5, size.y * 0.5);
        center = _window->_global_transform.apply_to(center);
        return center;
    }

    void Camera::rotate(Angle angle)
    {
        _window->_global_transform.rotate(angle, get_center());
        _angle += angle;
    }

    Vector2f Camera::apply_to(Vector2f point)
    {
        return _window->_global_transform.apply_to(point);
    }
}