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
        static inline auto device = std::random_device();
        static inline auto engine = std::mt19937();
    }

    void seed()
    {
        detail::engine.seed(detail::device());
    }

    float rand(float lower, float upper)
    {
        auto dist = std::uniform_real_distribution<float>(lower, upper);
        return dist(detail::engine);
    }

    float rand()
    {
        auto dist = std::uniform_real_distribution<float>();
        return dist(detail::engine);
    }

    float seed_to_rand(size_t x)
    {
        auto dist = std::uniform_real_distribution<float>();
        auto engine = std::mt19937();
        engine.seed(x);
        return dist(engine);
    }

    float seed_to_rand(size_t x, float lower, float upper)
    {
        auto dist = std::uniform_real_distribution<float>(lower, upper);
        auto engine = std::mt19937();
        engine.seed(x);
        return dist(engine);
    }
}