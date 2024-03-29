// 
// Copyright 2022 Clemens Cords
// Created on 6/26/22 by clem (mail@clemens-cords.com)
//

namespace rat
{
    Time::Time(int64_t n_nanoseconds)
            : _ns(std::chrono::nanoseconds(n_nanoseconds))
    {}

    Time minutes(double n)
    {
        return Time(int64_t(ceil(n * 1e+9) * 60));
    }

    Time seconds(double n)
    {
        return Time(int64_t(ceil(n * 1e+9)));
    }

    Time milliseconds(double n)
    {
        return Time(int64_t(ceil(n * 1e+6)));
    }

    Time microseconds(double n)
    {
        return Time(int64_t(ceil(n * 1e+3)));
    }

    Time nanoseconds(size_t n)
    {
        return Time(n);
    }

    double Time::as_minutes() const
    {
        return as_seconds() / 60;
    }

    double Time::as_seconds() const
    {
        return double(_ns.count()) / 1e+9;
    }

    double Time::as_milliseconds() const
    {
        return double(_ns.count()) / 1e+6;
    }

    double Time::as_microseconds() const
    {
        return double(_ns.count()) / 1e+3;
    }

    size_t Time::as_nanoseconds() const
    {
        return _ns.count();
    }

    Time Time::operator+(const Time& other)
    {
        auto out = *this;
        out._ns += other._ns;
        return out;
    }

    Time& Time::operator+=(const Time& other)
    {
        _ns += other._ns;
        return *this;
    }

    Time Time::operator-(const Time& other)
    {
        auto out = *this;
        out._ns -= other._ns;
        return out;
    }

    Time& Time::operator-=(const Time& other)
    {
        _ns -= other._ns;
        return *this;
    }

    bool Time::operator==(const Time& other)
    {
        return _ns == other._ns;
    }

    bool Time::operator!=(const Time& other)
    {
        return _ns != other._ns;
    }

    bool Time::operator<(const Time& other)
    {
        return _ns < other._ns;
    }

    bool Time::operator<=(const Time& other)
    {
        return _ns <= other._ns;
    }

    bool Time::operator>(const Time& other)
    {
        return _ns > other._ns;
    }

    bool Time::operator>=(const Time& other)
    {
        return _ns >= other._ns;
    }

    Clock::Clock()
            : _start(std::chrono::steady_clock::now())
    {}

    Time Clock::elapsed()
    {
        auto now = std::chrono::steady_clock::now();

        std::chrono::duration<int64_t, std::nano> elapsed = now - _start;
        return Time(elapsed.count());
    }

    Time Clock::restart()
    {
        Time since = elapsed();
        _start = std::chrono::steady_clock::now();
        return since;
    }
}