// 
// Copyright 2022 Clemens Cords
// Created on 7/24/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <deque>

#include <include/vector.hpp>
#include <include/geometric_shapes.hpp>

namespace rat
{
    enum Alignment
    {
        TOP_LEFT,
        TOP,
        TOP_RIGHT,
        RIGHT,
        BOTTOM_RIGHT,
        BOTTOM,
        BOTTOM_LEFT,
        LEFT,
        CENTER
    };

    struct Movable
    {
        virtual void set_centroid() = 0;
        virtual Rectangle get_bounding_box() const = 0;

        Vector2f get_centroid() const;
        Vector2f get_top_left() const;
        Vector2f get_size() const;

        Vector2f get_alignment_point(Alignment, float margin) const;
    };

    struct Resizable
    {
        virtual void set_size(Vector2f) = 0;
    };

    namespace UI
    {
        struct Element : public Movable, public Resizable
        {
        };

        struct Container : public Element
        {
            void push_front(Element *);
            void push_back(Element *);

            protected:
                std::deque<UI::Element> _elements;
                virtual void reorder() = 0;
        }
    }
}

// #####################

namespace rat
{
    Vector2f Movable::get_centroid() const
    {
        return get_alignment_point(CENTER, 0);
    }

    Vector2f Movable::get_top_left() const
    {
        return get_alignment_point(TOP_LEFT, 0);
    }

    Vector2f Movable::get_size() const
    {
        return get_bounding_box().size;
    }

    Vector2f Movable::get_alignment_point(Alignment alignment, float margin) const
    {
        auto aabb = get_bounding_box();
        float x = aabb.top_left.x;
        float y = aabb.top_left.y;
        float m = margin;

        switch (alignment)
        {
            case TOP_LEFT:
                return aabb.top_left + Vector2f(0 + m, 0 + m);
            case TOP:
                return aabb.top_left + Vector2f(x * 0.5, 0 + m);
            case TOP_RIGHT:
                return aabb.top_left + Vector2f(aabb.size.x - m, 0 - m);
            case RIGHT:
                return aabb.top_left + Vector2f(aabb.size.x - m, aabb.size.y * 0.5);
            case BOTTOM_RIGHT:
                return aabb.top_left + Vector2f(aabb.size.x - m, aabb.size.y - m);
            case BOTTOM:
                return aabb.top_left + Vector2f(aabb.size.x * 0.5, aabb.size.y - m);
            case BOTTOM_LEFT:
                return aabb.top_left + Vector2f(0 + m, aabb.size.y - m);
            case LEFT:
                return aabb.top_left + Vector2f(0 + m, aabb.size.y * 0.5);
            case CENTER:
                return aabb.top_left + Vector2f(aabb.size.x * 0.5, aabb.size.y * 0.5);
        }
    }
}