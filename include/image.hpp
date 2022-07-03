// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <vector>

#include <include/colors.hpp>
#include <include/common.hpp>

namespace rat
{
    // image living in RAM
    class Image
    {
        friend class Texture;
        template<bool> class Iterator;

        public:
            Image();
            ~Image();

            void create(size_t width, size_t height, RGBA color = RGBA(0, 0, 0, 1));
            void load(const std::string& path);

            Vector2ui get_size() const;

            auto at(size_t, size_t);
            auto begin();
            auto end();

            auto at(size_t, size_t) const;
            auto begin() const;
            auto end() const;

            operator SDL_Surface*();

        private:
            using Value_t = uint32_t;

            size_t _width, _height;
            SDL_Surface* _data; // RGBA, 8 bytes per element stored in 32-bit int

            static RGBA bit_to_color(Value_t);
            static Value_t color_to_bit(RGBA);
            size_t to_linear_index(size_t, size_t) const;

            static const Value_t r_mask = 0xFF000000;
            static const Value_t g_mask = 0x00FF0000;
            static const Value_t b_mask = 0x0000FF00;
            static const Value_t a_mask = 0x000000FF;

            static inline const bool NOT_CONST = false;
            static inline const bool CONST = true;

            template<bool is_const>
            class Iterator
            {
                public:
                    Iterator(Image*, size_t, size_t);

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = Iterator<is_const>; // sic
                    using difference_type = int;
                    using pointer = typename std::conditional<is_const, const value_type*, value_type*>::type;
                    using reference = typename std::conditional<is_const, const value_type&, value_type&>::type;

                    Iterator<is_const>& operator=(RGBA) requires (not is_const);

                    bool operator==(Iterator<is_const>& other) const;
                    bool operator!=(Iterator<is_const>& other) const;

                    auto operator++();
                    auto operator--();

                    operator RGBA() const;
                    Iterator<is_const> operator*() const;

                private:
                    mutable Image* _image;
                    size_t _x = 0,
                           _y = 0;
            };
    };
}

#include <.src/image.inl>