// 
// Copyright 2022 Joshua Higginbotham
// Created on 27.05.22 by clem (mail@clemens-cords.com | https://github.com/Clemapfel)
//

#pragma once

#include <vector>
#include <array>

#include <.src/include_gl.hpp>
#include <include/renderable.hpp>
#include <include/colors.hpp>
#include <include/vector.hpp>
#include <include/geometric_shapes.hpp>
#include <include/common.hpp>
#include <include/angle.hpp>
#include <include/shader.hpp>

namespace rat
{
    class Texture;
    struct Vertex
    {
        Vector3f position;
        Vector2f texture_coordinates;
        RGBA color;
    };

    class Shape //: public Renderable
    {
        public:
            Shape();
            virtual ~Shape();

            // primitives
            void as_triangle(Vector2f a, Vector2f b, Vector2f c);
            void as_rectangle(Vector2f top_left, Vector2f size);
            void as_circle(Vector2f center, float radius, size_t n_outer_vertices);
            void as_line(Vector2f a, Vector2f b);
            void as_line_strip(std::vector<Vector2f>);  // may overlap
            void as_polygon(std::vector<Vector2f> positions); // bounding polygon
            void as_wireframe(std::vector<Vector2f>);         // bounding polygon

            // compound shapes
            void as_frame(Vector2f top_left, Vector2f size, float width);

            void render(RenderTarget&, Transform = Transform(), Shader* = nullptr);

            Rectangle get_texture_rectangle() const;
            void set_texture_rectangle(Rectangle normalized);

            Vector2f get_centroid() const;
            void set_centroid(Vector2f);

            Vector2f get_top_left() const; // of aabb
            void set_top_left(Vector2f);

            void move(float x, float y);

            size_t get_n_vertices() const;

            RGBA get_vertex_color(size_t) const;
            void set_vertex_color(size_t, RGBA);

            void set_vertex_texture_coordinate(size_t, Vector2f);
            Vector2f get_vertex_texture_coordinate(size_t) const;

            void set_vertex_position(size_t, Vector3f);
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
            
        protected:
            std::vector<Vertex> _vertices; // in sdl coordinates
            void update_positions();
            void update_colors();
            void update_texture_coordinates();

            std::vector<uint32_t> _indices;
            void update_indices();

        private:
            void initialize();

            static Vector2f sdl_to_gl_screen_position(Vector2f);
            static Vector2f gl_to_sdl_screen_position(Vector2f);

            static Vector2f sdl_to_gl_texture_coordinates(Vector2f);
            static Vector2f gl_to_sdl_texture_coordinates(Vector2f);

            void align_texture_rectangle_with_bounding_box(); // align texture top left with aabb top left

            std::vector<Vector2f> sort_by_angle(const std::vector<Vector2f>&);

            static inline bool _noop_shader_initialized = false;
            static inline Shader* _noop_shader = nullptr;

            static inline const RGBA _default_color = RGBA(1, 1, 1, 1);

            Texture* _texture = nullptr;
            Rectangle _texture_rect = Rectangle{{0, 0}, {1, 1}};

            GLenum _render_type = GL_TRIANGLE_FAN;

            Vector2f _origin = Vector2f(0, 0);

            std::vector<float> _positions, // in gl coordinates
                               _colors,
                               _texture_coordinates;

            GLNativeHandle _vertex_array_id,
                    _element_buffer_id,
                    _position_buffer_id,
                    _color_buffer_id,
                    _texture_coordinate_buffer_id;
    };

    Shape TriangleShape(Vector2f a, Vector2f b, Vector2f c);
    Shape RectangleShape(Vector2f top_left, Vector2f size);
    Shape CircleShape(Vector2f center, float radius, size_t n_outer_vertices);
    Shape LineShape(Vector2f a, Vector2f b);
    Shape LineStripShape(std::vector<Vector2f>);
    Shape PolygonShape(std::vector<Vector2f> positions);
    Shape WireframeShape(std::vector<Vector2f>);
    Shape FrameShape(Vector2f top_left, Vector2f size, float width);

}

#include <.src/shape.inl>