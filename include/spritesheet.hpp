// 
// Copyright 2022 Clemens Cords
// Created on 7/13/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>

#include <include/image.hpp>
#include <include/texture.hpp>
#include <include/geometric_shapes.hpp>

namespace rat
{
    class Spritesheet
    {
        public:
            Spritesheet(RenderTarget&, const std::string& path, size_t n_rows, size_t n_columns);

            Rectangle get_frame(size_t linear_index);
            Rectangle get_frame(size_t x, size_t y);

            /// \returns rectangle, position relative to frame shape
            Rectangle get_frame_bounds(size_t linear_index);
            Rectangle get_frame_bounds(size_t x, size_t y);

            size_t get_n_frames() const;

            StaticTexture* get_texture() const;

        private:
            StaticTexture _texture;
            size_t _n_rows,
                   _n_columns;

            std::vector<Rectangle> _frame_bounds;
            Vector2ui _frame_size;
    };
}

namespace rat
{
    Spritesheet::Spritesheet(RenderTarget& target, const std::string &path, size_t n_rows, size_t n_columns)
        : _texture(target), _n_rows(n_rows), _n_columns(n_columns)
    {
        auto image = Image();
        if (not image.load(path)) // already warns
            return;

        _frame_size.x = image.get_size().x / n_columns;
        _frame_size.y = image.get_size().y / n_rows;

        if (std::fmod(image.get_size().y / float(n_rows), 1) != 0)
            std::cerr << "[WARNING] In Spritesheet::Constructor: Spritesheet at `" << path << " has a y-dimension (" << image.get_size().y << ") that is not integer-divisible by the number of rows (" << n_rows << ") specified" << std::endl;

        if (std::fmod(image.get_size().x / float(n_columns), 1) != 0)
            std::cerr << "[WARNING] In Spritesheet::Constructor: Spritesheet at `" << path << " has a x-dimension (" << image.get_size().x << ") that is not integer-divisible by the number of columns (" << n_columns << ") specified" << std::endl;

        // measure true bounds (without the transparency frame)
        for (size_t y = 0; y < n_columns; ++y)
        {
            for (size_t x = 0; x < n_rows; ++x)
            {
                auto top_left = Vector2ui(_frame_size.x * x, _frame_size.y * y);
                size_t x_left = 0;
                size_t x_right = _frame_size.x;
                size_t y_top = 0;
                size_t y_bottom = _frame_size.y;

                for (size_t j = 0; j < _frame_size.y; ++j)
                {
                    for (size_t i = 0; i < _frame_size.x; ++i)
                    {
                        auto alpha = (image.at(top_left.x + i, top_left.y + j).operator RGBA()).a;
                        if (alpha != 0)
                        {
                            if (i <= 0.5 * _frame_size.x)
                                x_left = std::max(x_left, i);

                            if (i >= 0.5 * _frame_size.x)
                                x_right = std::min(x_right, i);

                            if (j <= 0.5 * _frame_size.y)
                                y_top = std::max(y_top, i);

                            if (j >= 0.5 * _frame_size.y)
                                y_bottom = std::min(y_bottom, i);
                        }
                    }
                }

                auto rectangle = Rectangle();
                rectangle.top_left = Vector2f{x_left, y_top};
                rectangle.size = Vector2f{x_right - x_left, y_bottom - y_top};
                _frame_bounds.push_back(rectangle);
            }
        }

        _texture.create_from(image);
    }

    Rectangle Spritesheet::get_frame(size_t x, size_t y)
    {
        auto out = Rectangle();
        out.top_left.x = x * _frame_size.x;
        out.top_left.y = y * _frame_size.y;
        out.size = _frame_size;

        return out;
    }

    Rectangle Spritesheet::get_frame(size_t linear_index)
    {
        return get_frame(linear_index / _n_rows)
    }

    Rectangle Spritesheet::get_frame_bounds(size_t x, size_t y)
    {
        return _frame_bounds.at(x + _n_columns * y);
    }

    Rectangle Spritesheet::get_frame_bounds(size_t linear_index)
    {
        return _frame_bounds.at(linear_index);
    }
}