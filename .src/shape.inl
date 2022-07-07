// 
// Copyright 2022 Joshua Higginbotham
// Created on 27.05.22 by clem (mail@clemens-cords.com | https://github.com/Clemapfel)
//

#include <stdexcept>
#include <algorithm>

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

        auto out = centroid - in;
        out.x = 1 - out.x;
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
        out.x = 1 - out.x;
        out = centroid - out;
        return out;
    }

    Vector2f Shape::sdl_to_gl_texture_coordinates(Vector2f in)
    {
        in.x = 1 - in.x; // sdl texture are x-flipped
        return in;
    }

    Vector2f Shape::gl_to_sdl_texture_coordinates(Vector2f in)
    {
        in.x = 1 - in.x;
        return in;
    }

    Shape::Shape(const Shape& other)
    {
        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_position_buffer_id);
        glGenBuffers(1, &_color_buffer_id);
        glGenBuffers(1, &_texture_coordinate_buffer_id);
        glGenBuffers(1, &_element_buffer_id);

        _vertices = other._vertices;
        _texture = other._texture;
        _texture_rect = other._texture_rect;
        _render_type = other._render_type;
        _origin = other._origin;
        _indices = other._indices;

        initialize();
    }

    Shape &Shape::operator=(const Shape& other)
    {
        if (&other == this)
            return *this;

        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_position_buffer_id);
        glGenBuffers(1, &_color_buffer_id);
        glGenBuffers(1, &_texture_coordinate_buffer_id);
        glGenBuffers(1, &_element_buffer_id);

        _vertices = other._vertices;
        _texture = other._texture;
        _texture_rect = other._texture_rect;
        _render_type = other._render_type;
        _origin = other._origin;
        _indices = other._indices;

        initialize();
    }

    Shape::Shape(Shape&& other)
    {
        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_position_buffer_id);
        glGenBuffers(1, &_color_buffer_id);
        glGenBuffers(1, &_texture_coordinate_buffer_id);
        glGenBuffers(1, &_element_buffer_id);

        _vertices = std::move(other._vertices);
        _texture = std::move(other._texture);
        _texture_rect = std::move(other._texture_rect);
        _render_type = std::move(other._render_type);
        _origin = std::move(other._origin);
        _indices = std::move(other._indices);

        initialize();
    }

    Shape &Shape::operator=(Shape&& other)
    {
        if (&other == this)
            return *this;

        glGenVertexArrays(1, &_vertex_array_id);
        glGenBuffers(1, &_position_buffer_id);
        glGenBuffers(1, &_color_buffer_id);
        glGenBuffers(1, &_texture_coordinate_buffer_id);
        glGenBuffers(1, &_element_buffer_id);

        _vertices = std::move(other._vertices);
        _texture = std::move(other._texture);
        _texture_rect = std::move(other._texture_rect);
        _render_type = std::move(other._render_type);
        _origin = std::move(other._origin);
        _indices = std::move(other._indices);

        initialize();
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

    void Shape::render(RenderTarget& target, Transform transform, Shader* shader) const
    {
        GLNativeHandle program_id;

        if (shader == nullptr)
            program_id = _noop_shader->get_program_id();
        else
            program_id = shader->get_program_id();

        glUseProgram(program_id);
        glBindVertexArray(_vertex_array_id);

        glBindBuffer(GL_ARRAY_BUFFER, _position_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _positions.size() * sizeof(float), _positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, _color_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _colors.size() * sizeof(float), _colors.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, _texture_coordinate_buffer_id);
        glBufferData(GL_ARRAY_BUFFER, _texture_coordinates.size() * sizeof(float), _texture_coordinates.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer_id);

        if (_texture != nullptr)
            _texture->bind();

        glUniform1i(glGetUniformLocation(program_id, "_texture"), 0);
        glUniform1i(glGetUniformLocation(program_id, "_texture_set"), _texture != nullptr);

        glDrawElements(_render_type, _indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        if (_texture != nullptr)
            _texture->unbind();
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
        glBindVertexArray(0);
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
        glBindVertexArray(0);
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
        glBindVertexArray(0);
    }

    void Shape::update_indices()
    {
        glBindVertexArray(_vertex_array_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(float), _indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
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
            min_y = std::min(min_y, v.position.y);
            min_z = std::min(min_z, v.position.z);

            max_x = std::max(max_x, v.position.x);
            max_y = std::max(max_y, v.position.y);
            max_z = std::max(max_z, v.position.z);
        }

        return Rectangle{
            {min_x, min_y},
            {max_x - min_x, max_y - min_y}
        };
    }

    void Shape::align_texture_rectangle_with_bounding_box()
    {
        set_texture_rectangle(Rectangle{{0, 0}, {1, 1}});
    }

    void Shape::set_texture_rectangle(Rectangle normalized)
    {
        auto aabb = get_bounding_box();
        for (auto& v : _vertices)
        {
            // scale into [0, 1]
            v.texture_coordinates.x = 1 - (v.position.x - aabb.top_left.x) / aabb.size.x;
            v.texture_coordinates.y = (v.position.y - aabb.top_left.y) / aabb.size.y;

            // scale into correct size
            v.texture_coordinates.x *= (normalized.size.x - normalized.top_left.x);
            v.texture_coordinates.y *= (normalized.size.y - normalized.top_left.y);

            // anchor at correct top left
            v.texture_coordinates.x += normalized.top_left.x;
            v.texture_coordinates.y += normalized.top_left.y;

        }
        update_texture_coordinates();
    }

    Vector2f Shape::get_top_left() const
    {
        return get_bounding_box().top_left;
    }

    Vector2f Shape::get_size() const
    {
        return get_bounding_box().size;
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

    Vector3f Shape::get_vertex_position(size_t i) const
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

    void Shape::initialize()
    {
        // order matters:
        update_positions();
        update_colors();
        align_texture_rectangle_with_bounding_box();
        update_indices();
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
        initialize();
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
        initialize();
    }

    void Shape::as_line(Vector2f a, Vector2f b)
    {
        _vertices =
        {
            Vertex{{a.x, a.y, 0}, {0, 0}, _default_color},
            Vertex{{b.x, b.y, 0}, {0, 0}, _default_color}
        };
        _indices = {0, 1};
        _render_type = GL_LINES;
        initialize();
    }

    void Shape::as_circle(Vector2f center, float radius, size_t n_outer_vertices)
    {
        const float step = 360.f / n_outer_vertices;

        _vertices.clear();
        _vertices.push_back(Vertex{{center.x, center.y, 0}, {0, 0}, _default_color});

        for (float angle = 0; angle < 360; angle += step)
        {
            auto as_radians = angle * M_PI / 180.f;
            _vertices.push_back(Vertex{
                {
                    center.x + cos(as_radians) * radius,
                    center.y + sin(as_radians) * radius,
                    0
                },
                {0, 0},
                _default_color}
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
            _vertices.push_back(Vertex{{position.x, position.y, 0}, {0, 0}, _default_color});
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
            _vertices.push_back(Vertex{{position.x, position.y, 0}, {0, 0}, _default_color});
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
            _vertices.push_back(Vertex{{position.x, position.y, 0}, {0, 0}, _default_color});
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
            _vertices.push_back(Vertex{{x, y, 0}, {0, 0}, _default_color});
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

    Shape TriangleShape(Vector2f a, Vector2f b, Vector2f c)
    {
        auto out = Shape();
        out.as_triangle(a, b, c);
        return out;
    }

    Shape RectangleShape(Vector2f top_left, Vector2f size)
    {
        auto out = Shape();
        out.as_rectangle(top_left, size);
        return out;
    }

    Shape CircleShape(Vector2f center, float radius, size_t n_outer_vertices)
    {
        auto out = Shape();
        out.as_circle(center, radius, n_outer_vertices);
        return out;
    }

    Shape LineShape(Vector2f a, Vector2f b)
    {
        auto out = Shape();
        out.as_line(a, b);
        return out;
    }

    Shape LineStripShape(std::vector<Vector2f> vertices)
    {
        auto out = Shape();
        out.as_line_strip(vertices);
        return out;
    }

    Shape PolygonShape(std::vector<Vector2f> positions)
    {
        auto out = Shape();
        out.as_polygon(positions);
        return out;
    }

    Shape WireframeShape(std::vector<Vector2f> positions)
    {
        auto out = Shape();
        out.as_wireframe(positions);
        return out;
    }

    Shape FrameShape(Vector2f top_left, Vector2f size, float width)
    {
        auto out = Shape();
        out.as_frame(top_left, size, width);
        return out;
    }


}

