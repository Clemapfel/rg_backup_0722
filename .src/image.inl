// 
// Copyright 2022 Clemens Cords
// Created on 6/30/22 by clem (mail@clemens-cords.com)
//

namespace rat
{
    Image::Image()
        : _width(0), _height(0)
    {}
    
    Image::~Image()
    {
        SDL_FreeSurface(_data);
    }

    Image::operator SDL_Surface*()
    {
        return _data;
    }

    RGBA Image::bit_to_color(uint32_t in)
    {
        RGBA out;
        out.r = (r_mask & in) / 255.f;
        out.g = (g_mask & in) / 255.f;
        out.b = (b_mask & in) / 255.f;
        out.a = (a_mask & in) / 255.f;

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

    Vector2ui Image::get_size() const
    {
        return {_width, _height};
    }

    void Image::create(size_t width, size_t height, RGBA color)
    {
        _width = width;
        _height = height;
        _data = SDL_CreateRGBSurface(0, _width, _height, 32, r_mask, g_mask, b_mask, a_mask);
        
        auto value = color_to_bit(color);
        for (size_t i = 0, j = 0; i < _width, j < _height; ++i, ++j)
            ((uint32_t*) _data->pixels)[to_linear_index(i, j)] = value;
    }

    void Image::load(const std::string &path)
    {
        _data = IMG_Load(path.c_str());
    }

    template<bool is_const>
    Image::Iterator<is_const>::Iterator(Image* image, size_t x, size_t y)
        : _image(image), _x(x), _y(y)
    {}

    template<bool is_const>
    Image::Iterator<is_const> &Image::Iterator<is_const>::operator=(RGBA in) requires (not is_const)
    {
        ((uint32_t*) _image->_data->pixels)[_image->to_linear_index(_x, _y)] = color_to_bit(in);
    }

    template<bool is_const>
    Image::Iterator<is_const>::operator RGBA() const
    {
        return bit_to_color(((uint32_t*) _image->_data->pixels)[_image->to_linear_index(_x, _y)]);
    }

    template<bool is_const>
    Image::Iterator<is_const> Image::Iterator<is_const>::operator*() const
    {
        return *this;
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
        if (_x == _image->_width - 1 and _y == _image->_height - 1)
        {
            // move to past-the-end state
            _x++;
            _y++;
            return;
        }

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
        if (_y == 0 and _x == 0)
            return;

        if (_x == 0)
        {
            _x = _image->_width - 1;
            _y--;
        }
        else
            _x--;

    }

    auto Image::begin()
    {
        return Iterator<NOT_CONST>(this, 0, 0);
    }

    auto Image::begin() const
    {
        return Iterator<CONST>(const_cast<Image*>(this), 0, 0); // sic, operator= disabled by sfinae
    }

    auto Image::end()
    {
        return Iterator<NOT_CONST>(this, _width, _height);
    }

    auto Image::end() const
    {
        return Iterator<NOT_CONST>(const_cast<Image*>(this), _width, _height);
    }

    auto Image::at(size_t x, size_t y) const
    {
        if (x >= _width or y >= _height)
        {
            std::stringstream str;
            str << "In Image::at: index (" << x << ", " << y << ") out of range for an image of size " << _width << "x" << _height << std::endl;
            throw std::out_of_range(str.str());
        }

        return Iterator<CONST>(const_cast<Image*>(this), x, y);
    }

    auto Image::at(size_t x, size_t y)
    {
        if (x >= _width or y >= _height)
        {
            std::stringstream str;
            str << "In Image::at: index (" << x << ", " << y << ") out of range for an image of size " << _width << "x" << _height << std::endl;
            throw std::out_of_range(str.str());
        }

        return Image::Iterator<NOT_CONST>(this, x, y);
    }
}