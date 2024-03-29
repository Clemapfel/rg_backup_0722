// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>

namespace rat
{
    enum class ShaderType
    {
        FRAGMENT = GL_FRAGMENT_SHADER,
        VERTEX = GL_VERTEX_SHADER
    };

    struct Shader
    {
        public:
            Shader();
            ~Shader();

            GLNativeHandle get_program_id() const;
            GLNativeHandle get_fragment_shader_id() const;
            GLNativeHandle get_vertex_shader_id() const;

            // attribute locations all shaders have:
            static int get_vertex_position_location();
            static int get_vertex_color_location();
            static int get_vertex_texture_coordinate_location();

            // uniform locations all shaders have:
            int get_vertex_transform_location() const;
            int get_fragment_texture_location() const;
            int get_fragment_texture_set_location() const;

            //
            void create_from_string(const std::string& code, ShaderType);
            void create_from_file(const std::string& path, ShaderType);

        private:
            [[nodiscard]] GLNativeHandle compile_shader(const std::string&, ShaderType shader_type);
            [[nodiscard]] GLNativeHandle link_program(GLNativeHandle fragment_id, GLNativeHandle vertex_id);

            // local
            GLNativeHandle _program_id,
                    _fragment_shader_id,
                    _vertex_shader_id;

            // default noop
            static inline size_t _noop_program_id,
                    _noop_fragment_shader_id,
                    _noop_vertex_shader_id;

            static inline const std::string _noop_fragment_shader_source = R"(
                #version 130

                in vec4 _vertex_color;
                in vec2 _texture_coordinates;
                in vec3 _vertex_position;

                out vec4 _fragment_color;

                uniform int _texture_set;
                uniform sampler2D _texture;

                void main()
                {
                    if (_texture_set != 1)
                        _fragment_color = _vertex_color;
                    else
                        _fragment_color = texture2D(_texture, _texture_coordinates) * _vertex_color;
                }
            )";

            static inline const std::string _noop_vertex_shader_source = R"(
                #version 330

                layout (location = 0) in vec3 _vertex_position_in;
                layout (location = 1) in vec4 _vertex_color_in;
                layout (location = 2) in vec2 _vertex_texture_coordinates_in;

                uniform mat4 _transform;

                out vec4 _vertex_color;
                out vec2 _texture_coordinates;
                out vec3 _vertex_position;

                void main()
                {
                    gl_Position = /* _transform * */ vec4(_vertex_position_in, 1.0);
                    _vertex_color = _vertex_color_in;
                    _vertex_position = _vertex_position_in;
                    _texture_coordinates = _vertex_texture_coordinates_in;
                }
            )";
    };

    static inline Shader* noop_shader = nullptr;
}

#include <.src/shader.inl>