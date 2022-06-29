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

            Vector3f apply_to(Vector3f);
    };
}

#include <.src/transform.inl>