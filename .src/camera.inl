// 
// Copyright 2022 Clemens Cords
// Created on 7/8/22 by clem (mail@clemens-cords.com)
//

namespace rat
{
    Camera::Camera(RenderTarget *target)
            : _target(target)
    {}

    void Camera::update()
    {
        auto size = get_viewport_size();

        auto to_set = Transform();
        auto& transform = to_set.transform;

        transform = glm::translate(transform, Vector3f(0.5 * size.x, 0.5 * size.y, 0));
        transform = glm::translate(transform, Vector3f(-_position.x, -_position.y, -_position.z));
        transform = glm::rotate(transform, _rotation.as_radians(), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, Vector3f(_zoom, _zoom, 1));
        transform = glm::translate(transform, Vector3f(-0.5 * size.x, -0.5 * size.y, 0));

        _target->set_global_transform(to_set);
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
        center = _target->get_global_transform().apply_to(center);
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
        return _target->get_global_transform().apply_to(point);
    }
}