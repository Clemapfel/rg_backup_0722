// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <type_traits>
#include <limits>

namespace rat
{
    /// \concept: wrapper for std::is_same_v
    template<typename T, typename U>
    concept Is = std::is_same_v<T, U>;

    /// \concept: enable if
    template<bool Condition>
    concept EnableIf = requires {std::enable_if<Condition, bool>::type;};

    /// \brief positive infinity for given numeric type
    template<typename T>
    const T infinity = std::numeric_limits<T>::max();

    /// \brief negative infinity for given numeric type
    template<typename T>
    const T negative_infinity = std::numeric_limits<T>::min();
}