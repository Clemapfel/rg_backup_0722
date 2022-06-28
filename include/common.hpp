// 
// Copyright 2022 Clemens Cords
// Created on 6/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <type_traits>

namespace rat
{
    template<typename T, typename U>
    concept Is = std::is_same_v<T, U>;
}