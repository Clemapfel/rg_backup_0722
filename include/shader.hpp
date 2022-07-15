// 
// Copyright 2022 Clemens Cords
// Created on 6/27/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <.src/include_gl.hpp>

namespace rat
{
    class Shader
    {
        public:
            Shader();
            ~Shader();

            GLNativeHandle get_program_id() const;
            GLNativeHandle get_fragment_shader_id() const;
            GLNativeHandle get_vertex_shader_id() const;

        private:
            void compile_shader(const std::string&, GLuint shader_type);
            void link_program();

            GLNativeHandle _vertex_shader_id,
                           _fragment_shader_id,
                           _program_id;

            static inline std::string _noop_vertex_shader_source = R"(
                #version 420

                layout (location = 0) in vec3 vertex_position;
                layout (location = 1) in vec4 vertex_color;
                layout (location = 2) in vec2 vertex_texture_coordinate;

                out vec3 _vertex_position;
                out vec4 _vertex_color;
                out vec2 _vertex_texture_coordinate;

                void main()
                {
                    gl_Position = vec4(vertex_position.xyz, 1);

                    _vertex_position = vertex_position;
                    _vertex_color = vertex_color;
                    _vertex_texture_coordinate = vertex_texture_coordinate;
                }
            )";

            static inline std::string _noop_fragment_shader_source = R"(
                #version 420

                in vec3 _vertex_position;
                in vec4 _vertex_color;
                in vec2 _vertex_texture_coordinate;

                out vec4 _fragment_color;

                uniform int _texture_set;
                uniform sampler2D _texture;

                void main()
                {
                    if (_texture_set == 0)
                        _fragment_color = _vertex_color;
                    else
                        _fragment_color = texture2D(_texture, _vertex_texture_coordinate) * _vertex_color;
                }
            )";
    };
}

#include <.src/shader.inl>