// 
// Copyright 2022 Clemens Cords
// Created on 7/7/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <random>

namespace rat::rng
{
    namespace detail
    {
        static inline auto device = std::random_device();
        static inline auto engine = std::mt19937();
    }

    void seed()
    {
        detail::engine.seed(detail::device());
    }

    float rand()
    {
        static auto dist = std::uniform_real_distribution<float>(0, 1);
        return dist(detail::engine);
    }
}