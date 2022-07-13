// 
// Copyright 2022 Clemens Cords
// Created on 7/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <map>

#include <include/shape.hpp>

namespace rat
{
    struct Sprite
    {
        public:
            Sprite(const std::string& spritesheet_path);

        private:
            static std::map<std::string, StaticTexture> _texture_index;
            Shape _shape;
    };
}