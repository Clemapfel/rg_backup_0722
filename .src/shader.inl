
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

namespace rat
{
    Shader::Shader()
    {
        if (_noop_program_id == 0)
        {
            _noop_fragment_shader_id = compile_shader(_noop_fragment_shader_source, ShaderType::FRAGMENT);
            _noop_vertex_shader_id = compile_shader(_noop_vertex_shader_source, ShaderType::VERTEX);
            _noop_program_id = link_program(_noop_fragment_shader_id, _noop_vertex_shader_id);
        }

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

    void Shader::create_from_string(const std::string &code, ShaderType type)
    {
        if (type == ShaderType::FRAGMENT)
            _fragment_shader_id = compile_shader(code, type);
        else
            _vertex_shader_id = compile_shader(code, type);

        _program_id = link_program(_fragment_shader_id, _vertex_shader_id);
    }

    void Shader::create_from_file(const std::string &path, ShaderType type)
    {
        auto file = std::ifstream();

        file.open(path);
        if (not file.is_open())
        {
            std::cerr << "[WARNING] In Shader::create_from_file: Unable to open file at `" << path << "`" << std::endl;
            return;
        }
        auto str = std::stringstream();
        str << file.rdbuf();

        create_from_string(str.str(), type);
        file.close();
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

    GLNativeHandle Shader::compile_shader(const std::string &source, ShaderType shader_type)
    {
        GLNativeHandle id = glCreateShader(static_cast<GLenum>(shader_type));

        const char *source_ptr = source.c_str();
        glShaderSource(id, 1, &source_ptr, nullptr);
        glCompileShader(id);

        GLint compilation_success = GL_FALSE;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compilation_success);
        if (compilation_success != GL_TRUE)
        {
            std::cerr << "In Shader::compile_shader: compilation failed:\n"
                      << source << "\n\n";

            int info_length = 0;
            int max_length = info_length;

            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &max_length);

            auto log = std::vector<char>();
            log.resize(max_length);

            glGetShaderInfoLog(id, max_length, &info_length, log.data());

            for (auto c: log)
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

    int Shader::get_vertex_position_location()
    {
        return 0;
    }

    int Shader::get_vertex_color_location()
    {
        return 1;
    }

    int Shader::get_vertex_texture_coordinate_location()
    {
        return 2;
    }

    int Shader::get_vertex_transform_location() const
    {
        return glGetUniformLocation(_program_id, "_transform");
    }

    int Shader::get_fragment_texture_location() const
    {
        return glGetUniformLocation(_program_id, "_texture");
    }

    int Shader::get_fragment_texture_set_location() const
    {
        return glGetUniformLocation(_program_id, "_texture_set");
    }
}