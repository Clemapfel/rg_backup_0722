// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <cmath>

namespace rat
{
    struct Angle
    {
        public:
            explicit Angle(float radians);

            float as_degrees() const;
            float as_radians() const;

            Angle operator+(const Angle&);
            Angle operator-(const Angle&);
            Angle operator*(const Angle&);
            Angle operator/(const Angle&);

            Angle& operator+=(const Angle&);
            Angle& operator-=(const Angle&);
            Angle& operator*=(const Angle&);
            Angle& operator/=(const Angle&);

        private:
            float _rads;
    };

    Angle degrees(float);
    Angle radians(float);
}

#include <.src/angle.inl>