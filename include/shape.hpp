// 
// Copyright 2022 Clemens Cords
// Created on 7/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <vector>
#include <string>

#include <include/gl_common.hpp>
#include <include/colors.hpp>

namespace rat
{
    class Shape
    {
        public:
            Shape();
            ~Shape();

            void as_triangle(Vector2f a, Vector2f b, Vector2f c);
            void as_rectangle(Vector2f top_left, Vector2f size);
            void as_circle(Vector2f center, float radius, size_t n_outer_vertices);
            void as_line(Vector2f a, Vector2f b);
            void as_line_strip(std::vector<Vector2f>);
            void as_polygon(std::vector<Vector2f> positions); // bounding polygon
            void as_wireframe(std::vector<Vector2f>);         // bounding polygon
            void as_frame(Vector2f top_left, Vector2f size, float width);

            void render(Shader& shader, Transform transform);
            
        protected:
            struct Vertex
            {
                Vertex(float x, float y)
                    : position(x, y, 0), color(1, 1, 1, 1), texture_coordinates(0, 0)
                {}

                Vector3f position;
                RGBA color;
                Vector2f texture_coordinates;
            };
            
            std::vector<Vertex> _vertices;
            std::vector<int> _indices;
            GLenum _render_type = GL_TRIANGLE_STRIP;

            void update_position();
            void update_color();
            void update_texture_coordinate();
            void initialize();

            std::vector<Vector2f> sort_by_angle(const std::vector<Vector2f>&);
            
        private:
            struct VertexInfo
            {
                float _position[3];
                float _color[4];
                float _texture_coordinates[2];
            };

            void update_data(
                bool update_position = true,
                bool update_color = true,
                bool update_tex_coords = true
            );

            std::vector<VertexInfo> _vertex_data;

            GLNativeHandle _vertex_array_id = 0,
                           _vertex_buffer_id = 0;
            
    };
}

// ##############################################################################################

#include <include/shader.hpp>

namespace rat
{
    Shape::Shape()
    {
        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_vertex_buffer_id);
    }

    Shape::~Shape()
    {
        glDeleteVertexArrays(1, &_vertex_array_id);
        glDeleteBuffers(1, &_vertex_buffer_id);
    }

    void Shape::initialize()
    {
        _vertex_data.clear();
        _vertex_data.reserve(_vertices.size());

        for (auto &v: _vertices)
        {
            _vertex_data.emplace_back();
            auto &data = _vertex_data.back();

            data._position[0] = v.position[0];
            data._position[1] = v.position[1];
            data._position[2] = v.position[2];

            data._color[0] = v.color.r;
            data._color[1] = v.color.g;
            data._color[2] = v.color.b;
            data._color[3] = v.color.a;

            data._texture_coordinates[0] = v.texture_coordinates[0];
            data._texture_coordinates[1] = v.texture_coordinates[1];
        }

        update_data(true, true, true);
    }

    void Shape::update_data(bool update_position, bool update_color, bool update_tex_coords)
    {
        glBindVertexArray(_vertex_array_id);
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _vertex_data.size() * sizeof(VertexInfo), _vertex_data.data(), GL_STATIC_DRAW);

        if (update_position)
        {
            auto position_location = Shader::get_vertex_position_location();
            glEnableVertexAttribArray(position_location);
            glVertexAttribPointer(position_location,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(struct VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct VertexInfo, _position))
            );
        }

        if (update_color)
        {
            auto color_location = Shader::get_vertex_color_location();
            glEnableVertexAttribArray(color_location);
            glVertexAttribPointer(color_location,
                                  4,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(struct VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct VertexInfo, _color))
            );
        }

        if (update_tex_coords)
        {
            auto texture_coordinate_location = Shader::get_vertex_texture_coordinate_location();
            glEnableVertexAttribArray(texture_coordinate_location);
            glVertexAttribPointer(texture_coordinate_location,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(struct VertexInfo),
                                  (GLvoid *) (G_STRUCT_OFFSET(struct VertexInfo, _texture_coordinates))
            );
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray (0);
    }

    void Shape::update_position()
    {
        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            auto& v = _vertices.at(i);
            auto& data = _vertex_data.at(i);

            data._position[0] = v.position[0];
            data._position[1] = v.position[1];
            data._position[2] = v.position[2];
        }

        update_data(true, false, false);
    }

    void Shape::update_color()
    {
        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            auto& v = _vertices.at(i);
            auto& data = _vertex_data.at(i);

            data._color[0] = v.color.r;
            data._color[1] = v.color.g;
            data._color[2] = v.color.b;
            data._color[3] = v.color.a;
        }

        update_data(false, true, false);
    }

    void Shape::update_texture_coordinate()
    {
        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            auto& v = _vertices.at(i);
            auto& data = _vertex_data.at(i);

            data._texture_coordinates[0] = v.texture_coordinates[0];
            data._texture_coordinates[1] = v.texture_coordinates[1];
        }

        update_data(false, false, true);
    }

    void Shape::render(Shader& shader, Transform transform)
    {
        glUseProgram(shader.get_program_id());
        glUniformMatrix4fv(shader.get_vertex_transform_location(), 1, GL_FALSE, &(transform.transform[0][0]));

        // bind texture TODO
        glUniform1i(shader.get_fragment_texture_set_location(), GL_FALSE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(_vertex_array_id);
        glDrawElements(_render_type, _indices.size(), GL_UNSIGNED_INT, _indices.data());

        glBindVertexArray(0);
        glUseProgram(0);
    }

    std::vector<Vector2f> Shape::sort_by_angle(const std::vector<Vector2f>& in)
    {
        auto center = Vector2f(0, 0);
        for (const auto& pos : in)
            center += pos;

        size_t n = in.size();
        center /= Vector2f(n, n);

        std::vector<std::pair<Vector2f, Angle>> by_angle;
        for (const auto& pos : in)
            by_angle.emplace_back(pos, radians(std::atan2(pos.x - center.x, pos.y - center.y)));

        std::sort(by_angle.begin(), by_angle.end(), [](const std::pair<Vector2f, Angle>& a, const std::pair<Vector2f, Angle>& b)
        {
            return a.second.as_degrees() < b.second.as_degrees();
        });

        auto out = std::vector<Vector2f>();
        out.reserve(in.size());

        for (auto& pair : by_angle)
            out.push_back(pair.first);

        return out;
    }

    void Shape::as_triangle(Vector2f a, Vector2f b, Vector2f c)
    {
        _vertices =
        {
            Vertex(a.x, b.y),
            Vertex(b.x, b.y),
            Vertex(c.x, c.y)
        };

        _indices = {0, 1, 2};
        _render_type = GL_TRIANGLES;
        initialize();
    }

    void Shape::as_line(Vector2f a, Vector2f b)
    {
        _vertices =
        {
            Vertex(a.x, a.y),
            Vertex(b.x, b.y)
        };

        _indices = {0, 1};
        _render_type = GL_LINES;
        initialize();
    }

    void Shape::as_circle(Vector2f center, float radius, size_t n_outer_vertices)
    {
        const float step = 360.f / n_outer_vertices;

        _vertices.clear();
        _vertices.push_back(Vertex(center.x, center.y));

        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            _vertices.emplace_back(
                center.x + cos(as_radians) * radius,
                center.y + sin(as_radians) * radius
            );
        }

        _indices.clear();
        for (size_t i = 0; i < _vertices.size(); ++i)
            _indices.push_back(i);

        _indices.push_back(1);

        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::as_line_strip(std::vector<Vector2f> positions)
    {
        _vertices.clear();
        _indices.clear();

        size_t i = 0;
        for (auto& position : positions)
        {
            _vertices.emplace_back(position.x, position.y);
            _indices.push_back(i++);
        }

        _render_type = GL_LINE_STRIP;
        initialize();
    }

    void Shape::as_wireframe(std::vector<Vector2f> positions)
    {
        _vertices.clear();
        _indices.clear();

        positions = sort_by_angle(positions);

        size_t i = 0;
        for (auto& position : positions)
        {
            _vertices.emplace_back(position.x, position.y);
            _indices.push_back(i++);
        }

        _render_type = GL_LINE_LOOP;
        initialize();
    }

    void Shape::as_polygon(std::vector<Vector2f> positions)
    {
        _vertices.clear();
        _indices.clear();

        positions = sort_by_angle(positions);

        size_t i = 0;
        for (auto& position : positions)
        {
            _vertices.emplace_back(position.x, position.y);
            _indices.push_back(i++);
        }

        _render_type = GL_TRIANGLE_FAN;
        initialize();
    }

    void Shape::as_frame(Vector2f top_left, Vector2f size, float width)
    {
        _vertices.clear();
        _indices.clear();

        // hard-coded minimum vertex decomposition

        auto push_vertex = [&](float x, float y) {
            _vertices.emplace_back(x, y);
        };

        float x = top_left.x;
        float y = top_left.y;
        float w = size.x;
        float h = size.y;
        float l = width;

        // in order: left to right, top to bottom

        push_vertex(x, y);         // 0
        push_vertex(x+w-l, y);     // 1
        push_vertex(x+w, y);       // 2

        push_vertex(x, y+l);       // 3
        push_vertex(x+l, y+l);     // 4
        push_vertex(x+w-l, y+l);   // 5

        push_vertex(x+l, y+h-l);   // 6
        push_vertex(x+w-l, y+h-l); // 7
        push_vertex(x+w, y+h-l);   // 8

        push_vertex(x, y+h);       // 9
        push_vertex(x+l, y+h);     // 10
        push_vertex(x+w, y+h);     // 11

        _indices = {
            0, 1, 5, 0, 5, 3,
            1, 2, 7, 2, 7, 8,
            6, 11, 10, 6, 8, 11,
            3, 9, 10, 3, 4, 10
        };

        _render_type = GL_TRIANGLES;
        initialize();
    }
}