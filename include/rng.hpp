// 
// Copyright 2022 Clemens Cords
// Created on 7/7/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <random>

namespace rat
{
    namespace detail
    {
        static inline auto engine = std::mt19937();
    }

    float rand()
    {
        static auto dist = std::uniform_real_distribution<float>(0, 1);
        return dist(detail::engine);
    }
}