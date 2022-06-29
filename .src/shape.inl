// 
// Copyright 2022 Joshua Higginbotham
// Created on 27.05.22 by clem (mail@clemens-cords.com | https://github.com/Clemapfel)
//

#include <stdexcept>

#include <glm/glm.hpp>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>

#include <.src/include_gl.hpp>
#include <include/render_target.hpp>
#include <include/shape.hpp>
#include <include/texture.hpp>

namespace rat
{
    Vector2f Shape::sdl_to_gl_screen_position(Vector2f in)
    {
        auto size = detail::get_viewport_size();
        Vector2f centroid = Vector2f(size.x / 2, size.y / 2);

        auto out = in;
        out = centroid - out;
        out.x /= size.x / 2;
        out.y /= size.y / 2;
        return out;
    }

    Vector2f Shape::gl_to_sdl_screen_position(Vector2f in)
    {
        auto size = detail::get_viewport_size();
        Vector2f centroid = Vector2f(size.x / 2, size.y / 2);

        auto out = in;
        out.x *= size.x / 2;
        out.y *= size.y / 2;
        out = centroid + out;
        return out;
    }

    Vector2f Shape::sdl_to_gl_texture_coordinates(Vector2f in)
    {
        in.x = 1 - in.x;
        return in;
    }

    Vector2f Shape::gl_to_sdl_texture_coordinates(Vector2f in)
    {
        in.x = 1 - in.x;
        return in;
    }

    Shape::Shape()
    {
        if (not _noop_shader_initialized)
        {
            _noop_shader = new Shader();
            _noop_shader_initialized = true;
        }

        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_position_buffer_id);
        glGenBuffers(1, &_color_buffer_id);
        glGenBuffers(1, &_texture_coordinate_buffer_id);
        glGenBuffers(1, &_element_buffer_id);
    }

    Shape::~Shape()
    {
        glDeleteVertexArrays(1, &_vertex_array_id);
        glDeleteBuffers(1, &_position_buffer_id);
        glDeleteBuffers(1, &_color_buffer_id);
        glDeleteBuffers(1, &_texture_coordinate_buffer_id);
        glDeleteBuffers(1, &_element_buffer_id);
    }

    void Shape::render(RenderTarget& target, Transform transform, Shader* shader)
    {
        GLNativeHandle program_id;

        if (shader == nullptr)
            program_id = _noop_shader->get_program_id();
        else
            program_id = shader->get_program_id();

        glUseProgram(program_id);
        glBindVertexArray(_vertex_array_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer_id);

        if (_texture != nullptr)
            SDL_GL_BindTexture(_texture->get_native(), nullptr, nullptr);

        glUniform1i(glGetUniformLocation(program_id, "_texture"), 0);
        glUniform1i(glGetUniformLocation(program_id, "_texture_set"), _texture != nullptr);

        glDrawElements(_render_type, _indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        if (_texture != nullptr)
            SDL_GL_UnbindTexture(_texture->get_native());
    }

    void Shape::update_positions()
    {
        _positions.clear();
        _positions.reserve(_vertices.size() * 3);

        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            const auto& v = _vertices.at(i);
            auto position = sdl_to_gl_screen_position(v.position);

            _positions.push_back(position.x);
            _positions.push_back(position.y);
            _positions.push_back(v.position.z);
        }

        // vertex position: vec3 at layout = 0
        glBindVertexArray(_vertex_array_id);
        glBindBuffer(GL_ARRAY_BUFFER, _position_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _positions.size() * sizeof(float), _positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Shape::update_colors()
    {
        _colors.clear();
        _positions.reserve(_vertices.size() * 4);

        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            const auto& col = _vertices.at(i).color;

            _colors.push_back(col.r);
            _colors.push_back(col.g);
            _colors.push_back(col.b);
            _colors.push_back(col.a);
        }

        // color: rgba at layout = 1
        glBindBuffer(GL_ARRAY_BUFFER, _color_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _colors.size() * sizeof(float), _colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Shape::update_texture_coordinates()
    {
        _texture_coordinates.clear();
        _positions.reserve(_vertices.size() * 2);

        for (size_t i = 0; i < _vertices.size(); ++i)
        {
            const auto& pos = sdl_to_gl_texture_coordinates(_vertices.at(i).texture_coordinates);
            _texture_coordinates.push_back(pos.x);
            _texture_coordinates.push_back(pos.y);
        }

        // tex coord: vec2 at layout = 2
        glBindBuffer(GL_ARRAY_BUFFER, _texture_coordinate_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _texture_coordinates.size() * sizeof(float), _texture_coordinates.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Shape::update_indices()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(float), _indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    Vector2f Shape::get_centroid() const
    {
        Vector3f sum;
        for (auto& v : _vertices)
            sum += v.position;

        return sum / Vector3f(_vertices.size());
    }

    void Shape::set_centroid(Vector2f position)
    {
        auto delta = position - get_centroid();
        for (auto& v : _vertices)
        {
            v.position.x += delta.x;
            v.position.y += delta.y;
        }

        update_positions();
    }

    Rectangle Shape::get_bounding_box() const
    {
        float min_x = infinity<float>;
        float min_y = infinity<float>;
        float min_z = infinity<float>;

        float max_x = negative_infinity<float>;
        float max_y = negative_infinity<float>;
        float max_z = negative_infinity<float>;

        for (auto& v : _vertices)
        {
            min_x = std::min(min_x, v.position.x);
            min_y = std::min(min_y, v.position.x);
            min_z = std::min(min_z, v.position.x);

            max_x = std::max(max_x, v.position.x);
            max_y = std::max(max_y, v.position.x);
            max_z = std::max(max_z, v.position.x);
        }

        return Rectangle{
            {min_x, min_y},
            {max_x - min_x, max_y - min_y}
        };
    }

    void Shape::align_texture_rectangle_with_bounding_box()
    {
        auto aabb = get_bounding_box();
        for (auto& v : _vertices)
        {
            v.texture_coordinates.x = (v.position.x - aabb.top_left.x) / aabb.size.x;
            v.texture_coordinates.y = (v.position.y - aabb.top_left.y) / aabb.size.y;
        }
        update_texture_coordinates();
    }

    Vector2f Shape::get_top_left() const
    {
        return get_bounding_box().top_left;
    }

    void Shape::set_top_left(Vector2f position)
    {
        auto delta = position - get_bounding_box().top_left;
        for (auto& v : _vertices)
        {
            v.position.x += delta.x;
            v.position.y += delta.y;
        }

        update_positions();
    }

    void Shape::move(float x, float y)
    {
        for (auto& v : _vertices)
        {
            v.position.x += x;
            v.position.y += y;
        }

        update_positions();
    }

    size_t Shape::get_n_vertices() const
    {
        return _vertices.size();
    }

    void Shape::set_vertex_color(size_t i, RGBA color)
    {
        _vertices.at(i).color = color;
        update_colors();
    }

    RGBA Shape::get_vertex_color(size_t index) const
    {
        return RGBA(_vertices.at(index).color);
    }

    void Shape::set_vertex_position(size_t i, Vector3f position)
    {
        _vertices.at(i).position = position;
        update_positions();
    }

    Vector2f Shape::get_vertex_position(size_t i) const
    {
        return _vertices.at(i).position;
    }

    void Shape::set_vertex_texture_coordinate(size_t i, Vector2f coordinates)
    {
        _vertices.at(i).texture_coordinates = coordinates;
        update_texture_coordinates();
    }

    Vector2f Shape::get_vertex_texture_coordinate(size_t i) const
    {
        return _vertices.at(i).texture_coordinates;
    }

    void Shape::set_color(RGBA color)
    {
        for (auto& v : _vertices)
            v.color = color;

        update_colors();
    }

    RGBA Shape::get_color() const
    {
        RGBA sum;
        for (auto& v : _vertices)
            sum += v.color;

        float n = _vertices.size();
        return sum / RGBA(n, n, n, n);
    }

    Texture* Shape::get_texture() const
    {
        return _texture;
    }

    void Shape::set_texture(Texture* texture)
    {
        _texture = texture;
    }

    void Shape::set_origin(Vector2f relative_to_centroid)
    {
        _origin = relative_to_centroid;
    }

    Vector2f Shape::get_origin() const
    {
        return _origin;
    }

    void Shape::scale(float x_factor, float y_factor)
    {
        auto center = get_centroid() + _origin;

        for (auto& v : _vertices)
        {
            auto point = Vector2f(v.position.x, v.position.y) - center;
            auto distance = glm::distance(Vector2f(0, 0), point);
            auto angle_rad = std::atan2(point.x, point.y);

            v.position.x = center.x + cos(angle_rad) * distance * x_factor;
            v.position.y = center.y + sin(angle_rad) * distance * y_factor;
        }
        update_positions();
    }

    void Shape::as_rectangle(Vector2f top_left, Vector2f size)
    {
        _vertices =
        {
            Vertex{{top_left.x, top_left.y, 0}, {0, 0}, _default_color},
            Vertex{{top_left.x + size.x, top_left.y, 0}, {1, 0}, _default_color},
            Vertex{{top_left.x + size.x, top_left.y + size.y, 0}, {1, 1}, _default_color},
            Vertex{{top_left.x, top_left.y + size.y, 0}, {0, 1}, _default_color}
        };
        _indices = {0, 1, 3, 1, 2, 3};
        _render_type = GL_TRIANGLE_FAN;

        update_positions();
        update_colors();
        update_texture_coordinates();
        update_indices();
    }

    void Shape::as_triangle(Vector2f a, Vector2f b, Vector2f c)
    {
        _vertices =
        {
            Vertex{{a.x, a.y, 0}, {0, 0}, _default_color},
            Vertex{{b.x, b.y, 0}, {0, 0}, _default_color},
            Vertex{{c.x, c.y, 0}, {0, 0}, _default_color},
        };
        _indices = {0, 1, 2};
        _render_type = GL_TRIANGLES;

        align_texture_rectangle_with_bounding_box();

        update_positions();
        update_colors();
        update_texture_coordinates();
        update_indices();
    }
}

