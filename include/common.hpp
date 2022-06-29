// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <type_traits>
#include <limits>

namespace rat
{
    template<typename T, typename U>
    concept Is = std::is_same_v<T, U>;

    template<typename T>
    const T infinity = std::numeric_limits<T>::max();

    template<typename T>
    const T negative_infinity = std::numeric_limits<T>::min();
}