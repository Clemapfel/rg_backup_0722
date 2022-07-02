// 
// Copyright 2022 Clemens Cords
// Created on 6/30/22 by clem (mail@clemens-cords.com)
//


#include "include/image.hpp"

namespace rat
{
    Image::Image()
        : _width(0), _height(0)
    {}

    RGBA Image::bit_to_color(uint32_t in)
    {
        RGBA out;
        out.r = (r_mask & in) / 255.f;
        out.g = (g_mask & in) / 255.f;
        out.b = (b_mask & in) / 255.f;
        out.a = (a_mask & in) / 255.f;

        std::cout << out.r << " " << out.g << " " << out.b << " " << out.a << std::endl;

        return out;
    }

    uint32_t Image::color_to_bit(RGBA in)
    {
        static const uint32_t r_shift = 3 * 8;
        static const uint32_t g_shift = 2 * 8;
        static const uint32_t b_shift = 1 * 8;
        static const uint32_t a_shift = 0 * 8;

        auto r = static_cast<uint32_t>(round(in.r * 255));
        auto g = static_cast<uint32_t>(round(in.g * 255));
        auto b = static_cast<uint32_t>(round(in.b * 255));
        auto a = static_cast<uint32_t>(round(in.a * 255));

        auto out = uint32_t(0);
        out |= (r << r_shift);
        out |= (g << g_shift);
        out |= (b << b_shift);
        out |= (a << a_shift);

        return out;
    }

    size_t Image::to_linear_index(size_t x, size_t y) const
    {
        return y * _width + x;
    }

    RGBA Image::get(size_t x, size_t y) const
    {
         return bit_to_color(_data.at(to_linear_index(x, y)));
    }

    void Image::set(size_t x, size_t y, RGBA color)
    {
        _data.at(to_linear_index(x, y)) = color_to_bit(color);
    }

    Vector2ui Image::get_size() const
    {
        return {_width, _height};
    }

    void Image::create(size_t width, size_t height, RGBA color)
    {
        _width = width;
        _height = height;
        _data.resize(width * height, color_to_bit(color));
    }

    template<bool is_const>
    Image::Iterator<is_const>::Iterator(Image* image, size_t x, size_t y)
        : _image(image), _x(x), _y(y)
    {}

    template<bool is_const>
    Image::Iterator<is_const> &Image::Iterator<is_const>::operator=(RGBA in) requires (not is_const)
    {
        _image->_data.at(_image->to_linear_index(_x, _y)) = color_to_bit(in);
        std::cout << _x << " " << _y << " " << color_to_bit(in) << " " << std::endl;
    }

    template<bool is_const>
    RGBA Image::Iterator<is_const>::operator*() const
    {
        return bit_to_color(_image->_data.at(_image->to_linear_index(_x, _y)));
    }

    template<bool is_const>
    bool Image::Iterator<is_const>::operator==(Iterator<is_const> & other) const
    {
        return this->_x == other._x and this->_y == other._y;
    }

    template<bool is_const>
    bool Image::Iterator<is_const>::operator!=(Iterator<is_const> & other) const
    {
        return not ((*this) == other);
    }

    template<bool is_const>
    auto Image::Iterator<is_const>::operator++()
    {
        if (_x == _image->_width - 1)
        {
            _x = 0;
            if (_y != _image->_height - 1)
                _y++;
        }
        else
            _x++;
    }

    template<bool is_const>
    auto Image::Iterator<is_const>::operator--()
    {
        if (_x == 0)
        {
            _x = _image->_width - 1;
            if (_y != 0)
                _y--;
        }
        else
            _x--;
    }

    typename Image::NonConstIterator Image::begin()
    {
        return NonConstIterator(this, 0, 0);
    }

    typename Image::ConstIterator Image::begin() const
    {
        return ConstIterator(const_cast<Image*>(this), 0, 0); // sic, operator= disabled by sfinae
    }

    typename Image::NonConstIterator Image::end()
    {
        return NonConstIterator(this, _width - 1, _height - 1);
    }

    typename Image::ConstIterator Image::end() const
    {
        return ConstIterator(const_cast<Image*>(this), _width - 1, _height - 1);
    }

    typename Image::ConstIterator Image::at(size_t x, size_t y) const
    {
        return ConstIterator(const_cast<Image*>(this), x, y);
    }

    typename Image::NonConstIterator Image::at(size_t x, size_t y)
    {
        return Image::NonConstIterator(this, x, y);
    }
}