// 
// Copyright 2022 Clemens Cords
// Created on 7/15/22 by clem (mail@clemens-cords.com)
//

namespace rat
{
    void Shader::compile_shader(const std::string& source, GLuint type)
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

        const char* source_ptr = source.c_str();
        glShaderSource(id, 1, &source_ptr, nullptr);
        glCompileShader(id);

        GLint compilation_success = GL_FALSE;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_success);
        if (compilation_success != GL_TRUE)
        {
            std::cerr << "In Shader::compile_shader: compilation failed for \n"
                      << source << "\n\n";

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
    }

    void Shader::link_program()
    {
        if (not glIsShader(_vertex_shader_id) or not glIsShader(_fragment_shader_id))
        {
            std::cerr << "In Shader::link_program: trying to link a program despite one or more shaders being uninitialized." << std::endl;
            return;
        }

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
    }

    Shader::Shader()
    {
        _program_id = glCreateProgram();

        compile_shader(_noop_vertex_shader_source, GL_VERTEX_SHADER);
        compile_shader(_noop_fragment_shader_source, GL_FRAGMENT_SHADER);
        link_program();
    }

    Shader::~Shader()
    {
        glDeleteProgram(_program_id);
        glDeleteShader(_vertex_shader_id);
        glDeleteShader(_fragment_shader_id);
    }

    GLNativeHandle Shader::get_fragment_shader_id() const
    {
        return _fragment_shader_id;
    }

    GLNativeHandle Shader::get_vertex_shader_id() const
    {
        return _vertex_shader_id;
    }

    GLNativeHandle Shader::get_program_id() const
    {
        return _program_id;
    }
}