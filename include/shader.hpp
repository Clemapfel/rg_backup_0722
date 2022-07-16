// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>
#include <include/gl_common.hpp>

namespace rat
{
    struct Shader
    {
        public:
            Shader();
            ~Shader();

            GLNativeHandle get_program_id() const;
            GLNativeHandle get_fragment_shader_id() const;
            GLNativeHandle get_vertex_shader_id() const;

        private:
            GLNativeHandle compile_shader(const std::string&, GLuint shader_type);
            GLNativeHandle link_program(GLNativeHandle fragment_id, GLNativeHandle vertex_id);

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
                #version 130

                in vec3 _vertex_position_in;
                in vec4 _vertex_color_in;
                in vec2 _vertex_texture_coordinates_in;

                uniform mat4 _transform;

                out vec4 _vertex_color;
                out vec2 _texture_coordinates;

                void main()
                {
                    gl_Position = _transform * vec4(_vertex_position_in, 1.0);
                    _vertex_color = _vertex_color_in;
                }
            )";
    };
}

// #############################################################################

#include <iostream>
#include <vector>

namespace rat
{
    Shader::Shader()
    {
        if (GL_INITIALIZED and _noop_program_id == 0)
        {
            _noop_fragment_shader_id = compile_shader(_noop_fragment_shader_source, GL_FRAGMENT_SHADER);
            _noop_vertex_shader_id = compile_shader(_noop_vertex_shader_source, GL_VERTEX_SHADER);
            _noop_program_id = link_program(_noop_fragment_shader_id, _noop_vertex_shader_id);
        }
        else
            std::cerr << "[WARNING] In Shader::Shader: Trying to initialize the noop shaders, but GL_INITIALIZE is still false." << std::endl;

        _program_id = _noop_program_id;
        _fragment_shader_id = _noop_fragment_shader_id;
        _vertex_shader_id = _noop_vertex_shader_id;
    }

    Shader::~Shader()
    {
        if (_fragment_shader_id != 0 and _fragment_shader_id != _noop_fragment_shader_id)
            glDeleteShader(_fragment_shader_id);

        if (_vertex_shader_id != 0 and _vertex_shader_id != _noop_vertex_shader_id)
            glDeleteShader(_vertex_shader_id);

        if (_program_id != 0 and _program_id != _noop_program_id)
            glDeleteProgram(_program_id);
    }

    GLNativeHandle Shader::get_program_id() const
    {
        return _program_id;
    }

    GLNativeHandle Shader::get_vertex_shader_id() const
    {
        return _vertex_shader_id;
    }

    GLNativeHandle Shader::get_fragment_shader_id() const
    {
        return _fragment_shader_id;
    }

    GLNativeHandle Shader::compile_shader(const std::string& source, GLuint shader_type)
    {
        if (not (shader_type == GL_FRAGMENT_SHADER or shader_type == GL_VERTEX_SHADER))
            std::cerr << "[ERROR] In Shader::compiler_shader: Shader type is neither GL_FRAGMENT_SHADER nor GL_VERTEX_SHADER" << std::endl;

        GLNativeHandle id = glCreateShader(shader_type);

        const char* source_ptr = source.c_str();
        glShaderSource(id, 1, &source_ptr, nullptr);
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

            glDeleteShader(id);
            id = 0;
        }

        return id;
    }

    GLNativeHandle Shader::link_program(GLNativeHandle fragment_id, GLNativeHandle vertex_id)
    {
        GLNativeHandle id = glCreateProgram();
        glAttachShader(id, fragment_id);
        glAttachShader(id, vertex_id);
        glLinkProgram(id);

        GLint link_success = GL_FALSE;
        glGetProgramiv(id, GL_LINK_STATUS, &link_success);
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

            glDeleteProgram(id);
            id = 0;
        }

        return id;
    }
}