// 
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <glm/gtx/transform.hpp>

namespace rat
{
    class Transform : public glm::tmat4x4<float>
    {
        public:
            Transform();

            Vector2f apply_to(Vector2f);
            Vector3f apply_to(Vector3f);

            void rotate();
    };
}

#include <.src/transform.inl>