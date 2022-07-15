// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/vector.hpp>

namespace rat
{
    /// \brief geometric triangle
    struct Triangle
    {
        /// \brief points
        Vector2f a, b, c;
    };

    /// \brief geometric rectangle
    struct Rectangle
    {
        /// \brief top left corner;
        Vector2f top_left;

        /// \brief x- and y- dimension
        Vector2f size;
    };

    /// \brief geometric circle
    struct Circle
    {
        /// \brief center
        Vector2f center;

        /// \brief radius, half the diameter
        float radius;
    };
}