// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//


namespace rat
{
    Transform::Transform()
        : glm::tmat4x4<float>(0.f)
    {
        for (size_t i = 0; i < 4; ++i)
            (*this)[i][i] = 1.f;
    }
}