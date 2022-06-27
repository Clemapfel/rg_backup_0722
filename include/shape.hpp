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

namespace rat
{
    class Texture;

    class Shape : public Renderable
    {
        public:
            virtual ~Shape() = default;

            void render(RenderTarget*, Transform) const final override;

            void set_centroid(Vector2f position);
            Vector2f get_centroid() const;

            virtual void move(float x_offset, float y_offset);

            void set_color(RGBA);
            RGBA get_color(size_t vertex_index) const;

            size_t get_n_vertices() const;
            Texture* get_texture() const;

            void set_texture(Texture*);

            void set_texture_rectangle(Rectangle rect);
            Rectangle get_texture_rectangle();

            Rectangle get_bounding_box() const;

            void set_vertex_position(size_t index, Vector2f position);
            void set_vertex_color(size_t index, RGBA);
            void set_vertex_texture_coordinates(size_t index, Vector2f relative);

            Vector2f get_vertex_position(size_t index) const;
            RGBA get_vertex_color(size_t index) const;
            Vector2f get_vertex_texture_coordinates(size_t index);

            void set_origin(Vector2f relative_to_centroid);
            Vector2f get_origin() const;

            void rotate();
            void scale(float);

        protected:
            Shape() = default;

            std::vector<float> _positions;
            std::vector<float> _colors;
            std::vector<float> _texture_coords;
            std::vector<size_t> _indices;

        private:
            Vector2f _origin = {0, 0};

            Texture* _texture = nullptr;
            Rectangle _texture_rect = Rectangle{{0, 0}, {1, 1}};
            void apply_texture_rectangle();

            Vector2f compute_centroid() const;
    };
}