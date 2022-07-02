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
    // image living in RAM, 8-bit colors
    class Image
    {
        friend class Texture;
        template<bool> class Iterator;
        using NonConstIterator = Iterator<false>;
        using ConstIterator = Iterator<true>;

        public:
            Image();

            void create(size_t width, size_t height, RGBA color = RGBA(0, 0, 0, 1));

            RGBA get(size_t x, size_t y) const;
            void set(size_t x, size_t y, RGBA);

            Vector2ui get_size() const;

            NonConstIterator at(size_t, size_t);
            NonConstIterator begin();
            NonConstIterator end();

            ConstIterator at(size_t, size_t) const;
            ConstIterator begin() const;
            ConstIterator end() const;

        private:
            using Value_t = uint32_t;

            size_t _width, _height;
            std::vector<uint32_t> _data; // RGBA, 8 bytes per element

            static RGBA bit_to_color(Value_t);
            static Value_t color_to_bit(RGBA);
            size_t to_linear_index(size_t, size_t) const;

            static const Value_t r_mask = 0xFF000000;
            static const Value_t g_mask = 0x00FF0000;
            static const Value_t b_mask = 0x0000FF00;
            static const Value_t a_mask = 0x000000FF;

            template<bool is_const>
            class Iterator
            {
                public:
                    Iterator(Image*, size_t, size_t);

                    using iterator_category = std::bidirectional_iterator_tag;
                    using value_type = RGBA;//typename std::conditional<is_const, const Value_t, Value_t>::type;
                    using difference_type = int;
                    using pointer = typename std::conditional<is_const, const value_type*, value_type*>::type;
                    using reference = typename std::conditional<is_const, const value_type&, value_type&>::type;

                    Iterator<is_const>& operator=(RGBA) requires (not is_const);

                    bool operator==(Iterator<is_const>& other) const;
                    bool operator!=(Iterator<is_const>& other) const;

                    auto operator++();
                    auto operator--();

                    RGBA operator*() const;

                private:
                    mutable Image* _image;
                    size_t _x = 0,
                           _y = 0;
            };
    };
}

#include <.src/image.inl>