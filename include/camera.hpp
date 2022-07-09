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

            // set
            void center_on(Vector2f);
            void set_zoom(float);
            void set_rotation(Angle);

            // add
            void move(float x, float y);
            void rotate(Angle);
            void zoom_in(float);
            void zoom_out(float);

            Vector2f get_center() const;
            Vector2f apply_to(Vector2f sdl_coords);

        private:
            float _zoom = 1;
            Angle _rotation = degrees(0);

            Window* _window;

            glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
            static inline const glm::vec3 _direction = glm::vec3(0.0f, 0.0f, -1.0f);
            static inline const glm::vec3 _up_normal = glm::vec3(0.0f, 1.0f,  0.0f);

            void update();

    };
}

#include <.src/camera.inl>

namespace rat
{
    Camera::Camera(Window* window)
        : _window(window)
    {}

    void Camera::update()
    {
        std::cout << _rotation.as_degrees() << std::endl;

        auto size = get_viewport_size();

        auto& transform = _window->_global_transform._transform;
        transform = glm::mat4x4(1);

        transform = glm::translate(transform, Vector3f(0.5 * size.x, 0.5 * size.y, 0));
        transform = glm::rotate(transform, _rotation.as_radians(), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, Vector3f(_zoom, _zoom, 1));
        transform = glm::translate(transform, Vector3f(-0.5 * size.x, -0.5 * size.y, 0));

        transform = glm::translate(transform, Vector3f(-_position.x, -_position.y, -_position.z));
    }

    void Camera::move(float x, float y)
    {
        _position.x -= x;
        _position.y -= y;

        update();
    }

    void Camera::center_on(Vector2f position)
    {
        auto size = get_viewport_size();
        _position.x = position.x - 0.5 * size.x;
        _position.y = position.y - 0.5 * size.y;

        update();
    }

    void Camera::set_zoom(float zoom)
    {
        _zoom = zoom;
        update();
    }

    void Camera::zoom_in(float factor)
    {
        _zoom *= factor;
        update();
    }

    void Camera::zoom_out(float factor)
    {
        _zoom /= factor;
        update();
    }

    Vector2f Camera::get_center() const
    {
        auto size = get_viewport_size();
        Vector2f center = Vector2f(size.x * 0.5, size.y * 0.5);
        center = _window->_global_transform.apply_to(center);
        return center;
    }

    void Camera::set_rotation(Angle angle)
    {
        _rotation = angle;
        update();
    }

    void Camera::rotate(Angle angle)
    {
        _rotation += angle;
        update();
    }

    Vector2f Camera::apply_to(Vector2f point)
    {
        return _window->_global_transform.apply_to(point);
    }
}