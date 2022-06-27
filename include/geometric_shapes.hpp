// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/vector.hpp>

namespace rat
{
    struct Triangle
    {
        Vector2f a, b, c;
    };

    struct Rectangle
    {
        Vector2f top_left, size;
    };

    struct Circle
    {
        Vector2f center;
        float radius;
    };
}