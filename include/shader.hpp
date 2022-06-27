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

        private:
            void compile_shader(const std::string&, GLuint shader_type);

            GLNativeID _vertex_shader_id,
                       _fragment_shader_id,
                       _program_id;

            static inline const GLchar* _noop_fragment_source[] = {
                "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
            };

            static inline const GLchar* _noop_vertex_shader_source[] = {
                "#version 140\nin vec2 LVertexPos2D; void main() { gl_Position = vec4( LVertexPos2D.x, LVertexPos2D.y, 0, 1 ); }"
            };
    };
}

namespace rat
{
    Shader::Shader()
    {
        _program_id = glCreateProgram();

        auto compile_shader = [&](const GLchar* source[], GLuint type)
        {
            GLuint id;
            if (type == GL_VERTEX_SHADER)
            {
                _vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
                id = _vertex_shader_id;
            }
            else if (type == GL_FRAGMENT_SHADER)
            {
                _fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
                id = _fragment_shader_id;
            }

            glShaderSource(id, 1, source, nullptr);
            glCompileShader(id);

            GLint compilation_success = GL_FALSE;
            glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_success);
            if (compilation_success != GL_TRUE)
            {
                std::cerr << "In Shader::compile_shader: compilation failed: ";

                int info_length = 0;
                int max_length = info_length;

                glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

                auto log = std::vector<char>();
                log.resize(max_length);

                glGetShaderInfoLog(id, max_length, &info_length, log.data());

                for (auto c : log)
                    std::cerr << c;

                std::cerr << std::endl;
            }

            glAttachShader(_program_id, id);
        };

        compile_shader(_noop_vertex_shader_source, GL_VERTEX_SHADER);
        compile_shader(_noop_fragment_source, GL_FRAGMENT_SHADER);

        auto link_program = [&]()
        {
            glAttachShader(_program_id, _vertex_shader_id);
            glAttachShader(_program_id, _fragment_shader_id);
            glLinkProgram(_program_id);

            GLint link_success = GL_FALSE;
            glGetProgramiv(_program_id, GL_LINK_STATUS, &link_success);
            if (link_success != GL_TRUE)
            {
                std::cerr << "In Shader::link_program: linking failed:" << std::endl;

                int info_length = 0;
                int max_length = info_length;

                glGetProgramiv(_program_id, GL_INFO_LOG_LENGTH, &max_length);

                auto log = std::vector<char>();
                log.resize(max_length);

                glGetProgramInfoLog(_program_id, max_length, &info_length, log.data());

                for (auto c: log)
                    std::cerr << c;

                std::cerr << std::endl;
            }
        };

        link_program();
    }
}