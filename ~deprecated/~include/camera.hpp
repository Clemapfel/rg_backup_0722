// 
// Copyright 2022 Clemens Cords
// Created on 7/8/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/window.hpp>

namespace rat
{
    /// \brief camera controlling a rat::RenderTarget
    class Camera
    {
        public:
            /// \brief create a camera controlling a render target
            /// \param render_target: target
            Camera(RenderTarget*);

            /// \brief get the render target the camera is currently controlling
            /// \returns pointer to render target
            RenderTarget* get_render_target() const;

            /// \brief set the current render target
            /// \param target: new target to be controlled, or nullptr
            void set_render_target(RenderTarget*);

            /// \brief set the camera position such that it is centered on the given point
            /// \param point: point to center on
            void center_on(Vector2f);

            /// \brief set the camera zoom
            /// \param value: zoom where 1 is no zoom, 0.5 is 50% zoom out, 1.5 is 50% zoom in
            void set_zoom(float);

            /// \brief set the camera rotation
            /// \param value: angle, where 0° or 360° is no rotation, clockwise
            void set_rotation(Angle);

            /// \brief move the camera by a given offset
            /// \param x: x-offset, may be negative
            /// \param y: y-offset, may be negative
            void move(float x, float y);

            /// \brief rotate the camera by a given offset
            /// \param angle: offset, will be added to the current rotation
            void rotate(Angle);

            /// \brief zoom the camera in, this makes elements on screen appear larger
            /// \param value: where 0 is no additional magnification, 1 is 100% increase in magnification
            void zoom_in(float);

            /// \brief zoom the camera out, this makes elements on screen appear smaller
            /// \param value: where 0 is no additional magnification, 1 is 100% decrease in magnification
            void zoom_out(float);

            /// \brief get the world coordinates of the point the camera is currently focused on
            /// \returns point, world-coordinates
            Vector2f get_center() const;

            /// \brief apply the cameras transform to a point
            /// \param coords: absolute world coordinates
            Vector2f apply_to(Vector2f coords);

        private:
            float _zoom = 1;
            Angle _rotation = degrees(0);

            RenderTarget* _target;

            glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f);
            static inline const glm::vec3 _direction = glm::vec3(0.0f, 0.0f, -1.0f);
            static inline const glm::vec3 _up_normal = glm::vec3(0.0f, 1.0f,  0.0f);

            void update();
    };
}

#include <.src/camera.inl>
