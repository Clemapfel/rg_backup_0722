// 
// Copyright 2022 Clemens Cords
// Created on 6/30/22 by clem (mail@clemens-cords.com)
//

namespace rat
{
    Image::Image()
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
        uint8_t r, g, b, a;

        SDL_GetRGBA(in, _data->format, &r, &g, &b, &a);

        out.r = r / 255.f;
        out.g = g / 255.f;
        out.b = b / 255.f;
        out.a = a / 255.f;

        return out;
    }

    uint32_t Image::color_to_bit(RGBA in)
    {
        return SDL_MapRGBA(_data->format, in.r * 255, in.g * 255, in.b * 255, in.a * 255);
    }

    size_t Image::to_linear_index(size_t x, size_t y) const
    {
        return y * get_size().x + x;
    }

    Vector2ui Image::get_size() const
    {
        return {_data->w, _data->h};
    }

    void Image::create(size_t width, size_t height, RGBA color)
    {
        _data = SDL_CreateRGBSurface(0, width, height, 32, r_mask, g_mask, b_mask, a_mask);
        
        auto value = color_to_bit(color);
        for (size_t i = 0; i < get_size().x; ++i)
            for (size_t j = 0; j < get_size().y; ++j)
                ((uint32_t*) _data->pixels)[to_linear_index(i, j)] = value;
    }

    void Image::load(const std::string &path)
    {
        _data = IMG_Load(path.c_str());

        if (_data->format->format != SDL_PIXELFORMAT_RGBA32)
        {
            auto* _temp = _data;
            auto format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
            _data = SDL_ConvertSurfaceFormat(_temp, format->format, 0);
        }
    }

    template<bool is_const>
    Image::Iterator<is_const>::Iterator(Image* image, size_t x, size_t y)
        : _image(image), _x(x), _y(y)
    {}

    template<bool is_const>
    uint32_t Image::Iterator<is_const>::raw()
    {
        return ((uint32_t*) _image->_data->pixels)[_image->to_linear_index(_x, _y)];
    }

    template<bool is_const>
    Image::Iterator<is_const> &Image::Iterator<is_const>::operator=(RGBA in) requires (not is_const)
    {
        ((uint32_t*) _image->_data->pixels)[_image->to_linear_index(_x, _y)] = _image->color_to_bit(in);
    }

    template<bool is_const>
    Image::Iterator<is_const>::operator RGBA() const
    {
        return _image->bit_to_color(((uint32_t*) _image->_data->pixels)[_image->to_linear_index(_x, _y)]);
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
        if (_x == _image->get_size().x - 1 and _y == _image->get_size().y - 1)
        {
            // move to past-the-end state
            _x++;
            _y++;
            return;
        }

        if (_x == _image->get_size().x - 1)
        {
            _x = 0;
            if (_y != _image->get_size().y - 1)
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
            _x = _image->get_size().x - 1;
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
        return Iterator<NOT_CONST>(this, get_size().x, get_size().y);
    }

    auto Image::end() const
    {
        return Iterator<CONST>(const_cast<Image*>(this), get_size().x, get_size().y);
    }

    auto Image::at(size_t x, size_t y) const
    {
        if (x >= get_size().x or y >= get_size().y)
        {
            std::stringstream str;
            str << "In Image::at: index (" << x << ", " << y << ") out of range for an image of size " << get_size().x << "x" << get_size().y << std::endl;
            throw std::out_of_range(str.str());
        }

        return Iterator<CONST>(const_cast<Image*>(this), x, y);
    }

    auto Image::at(size_t x, size_t y)
    {
        if (x >= get_size().x or y >= get_size().y)
        {
            std::stringstream str;
            str << "In Image::at: index (" << x << ", " << y << ") out of range for an image of size " << get_size().x << "x" << get_size().y << std::endl;
            throw std::out_of_range(str.str());
        }

        return Image::Iterator<NOT_CONST>(this, x, y);
    }

    uint32_t* Image::data()
    {
        return (uint32_t*) _data->pixels;
    }
}