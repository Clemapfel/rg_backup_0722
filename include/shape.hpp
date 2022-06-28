// 
// Copyright 2022 Joshua Higginbotham
// Created on 27.05.22 by clem (mail@clemens-cords.com | https://github.com/Clemapfel)
//

#pragma once

#include <vector>
#include <array>

#include <include/renderable.hpp>
#include <include/colors.hpp>
#include <include/vector.hpp>
#include <include/geometric_shapes.hpp>
#include <include/common.hpp>
#include <include/angle.hpp>

namespace rat
{
    class Texture;

    class Shape : public Renderable
    {
        public:
            Shape();
            ~Shape();

            void as_triangle(Vector2f a, Vector2f b, Vector2f c);
            void as_rectangle(Vector2f top_left, Vector2f size);
            void as_circle(Vector2f center, float radius, size_t n_outer_vertices);
            void as_line(Vector2f a, Vector2f b);

            template<Is<Vector2f>... Vector2fs>
            void as_polygon(Vector2fs... positions);

            void render_temp();

            Rectangle get_texture_rectangle() const;
            void set_texture_rectangle(Rectangle normalized);

            Vector2f get_centroid() const;
            void set_centroid(Vector2f);

            Vector2f get_top_left() const; // of aabb
            void set_top_lef() const;

            void move(float x, float y);

            size_t get_n_vertices() const;

            RGBA get_vertex_color(size_t) const;
            void set_vertex_color(size_t, RGBA);

            void set_vertex_texture_coordinate(size_t, Vector2f);
            Vector2f get_vertex_texture_coordinate(size_t) const;

            void set_vertex_position(size_t, Vector2f);
            Vector2f get_vertex_position(size_t) const;

            void set_color(RGBA);
            RGBA get_color() const; // average of all vertices

            Texture* get_texture() const;
            void set_texture(Texture*);

            Rectangle get_bounding_box() const;

            void set_origin(Vector2f relative_to_centroid);
            Vector2f get_origin() const;

            void rotate(Angle);
            void scale(float x_factor, float y_factor);

        private:
            static Vector2f sdl_to_gl_screen_position(Vector2f);
            static Vector2f gl_to_sdl_screen_position(Vector2f);

            static Vector2f sdl_to_gl_texture_coord(Vector2f);
            static Vector2f gl_to_sdl_texture_coord(Vector2f);

            SDL_Texture* _texture;
            Rectangle _texture_rect = Rectangle{{0, 0}, {1, 1}};

            size_t _n_vertices;
            GLenum _drawing_type = GL_TRIANGLE_FAN;

            Vector2f _origin = Vector2f(0, 0);

            std::vector<float> _positions;
            std::vector<float> _colors;
            std::vector<float> _tex_coords;

            GLNativeHandle _vertex_array_id,
                    _element_buffer_id,
                    _position_buffer_id,
                    _color_buffer_id,
                    _tex_coord_buffer_id;
    };
}