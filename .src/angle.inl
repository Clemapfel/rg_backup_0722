// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

namespace rat
{
    Angle::Angle(float radians)
            : _rads(radians)
    {}

    Angle degrees(float dg)
    {
        return Angle(dg * (180.f / M_PI));
    }

    Angle radians(float rad)
    {
        return Angle(rad);
    }

    float Angle::as_degrees() const
    {
        return _rads * (180.f / M_PI);
    }

    float Angle::as_radians() const
    {
        return _rads * (M_PI / 180.f);
    }

    Angle Angle::operator+(const Angle& other)
    {
        return Angle(this->_rads + other._rads);
    }

    Angle Angle::operator-(const Angle& other)
    {
        return Angle(this->_rads - other._rads);
    }

    Angle Angle::operator*(const Angle& other)
    {
        return Angle(this->_rads * other._rads);
    }

    Angle Angle::operator/(const Angle& other)
    {
        return Angle(this->_rads / other._rads);
    }

    Angle& Angle::operator+=(const Angle& other)
    {
        this->_rads += other._rads;
        return *this;
    }

    Angle& Angle::operator-=(const Angle& other)
    {
        this->_rads -= other._rads;
        return *this;
    }

    Angle& Angle::operator*=(const Angle& other)
    {
        this->_rads *= other._rads;
        return *this;
    }

    Angle& Angle::operator/=(const Angle& other)
    {
        this->_rads /= other._rads;
        return *this;
    }
}