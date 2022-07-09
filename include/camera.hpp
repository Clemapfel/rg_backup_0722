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
