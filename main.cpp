//
// Created by clem on 6/26/22.
//

#include <mousetrap.hpp>
#include <thread>
#include <iostream>
#include <vector>

#include <GLES3//gl32.h>
#include <GL/gl.h>

using namespace rat;

using GLNativeHandle = GLuint;

int main()
{
    auto window = Window();
    window.create("test", 400, 300);

    GLNativeHandle noop_vertex_shader,
                   noop_fragment_shader,
                   noop_shader_program;
    {
        auto print_shader_log = [](GLNativeHandle shader)
                {
            if( glIsShader( shader ) )
            {
                int infoLogLength = 0;
                int maxLength = infoLogLength;
                glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

                //Allocate string
                char* infoLog = new char[ maxLength ];

                //Get info log
                glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
                if( infoLogLength > 0 )
                {
                    //Print Log
                    printf( "%s\n", infoLog );
                }

                //Deallocate string
                delete[] infoLog;
            }
            else
            {
                printf( "Name %d is not a shader\n", shader );
            }
        };

        std::string source = R"(
            #version 330 core
            layout (location = 0) in vec3 _vertex_pos;
            layout (location = 1) in vec3 _vertex_color_rgb;
            layout (location = 2) in vec2 _vertex_tex_coord;
            out vec2 _tex_coord;
            out vec3 _vertex_color;
            // DO NOT MODIFY
            void main()
            {
                gl_Position = vec4(_vertex_pos, 1.0);
                _vertex_color = _vertex_color_rgb;
                _tex_coord = _vertex_tex_coord;
            }
        )";

        const char* source_ptr = source.c_str();
        auto id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(id, 1, &source_ptr, nullptr);
        glCompileShader(id);

        GLint vertex_compile_status = GL_FALSE;
        glGetShaderiv(noop_vertex_shader, GL_COMPILE_STATUS, &vertex_compile_status);
        if(vertex_compile_status != GL_TRUE)
        {
            printf( "Unable to compile vertex shader %d!\n", vertexShader );
            printShaderLog( vertexShader );
            success = false;
        }

        noop_vertex_shader = id;

        source = R"(
            #version 330 core
            in vec2 _tex_coord;
            out vec4 _out;
            uniform sampler2D _texture;
            // DO NOT MODIFY
            void main()
            {
                _out = texture2D(_texture, _tex_coord);
            }
        )";

        source_ptr = source.c_str();
        id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(id, 1, &source_ptr, nullptr);
        glCompileShader(id);

        noop_fragment_shader = id;

        noop_shader_program = glCreateProgram();
        glAttachShader(noop_shader_program, noop_vertex_shader);
        glAttachShader(noop_shader_program, noop_fragment_shader);
        glLinkProgram(noop_shader_program);

        int linking_success;
        char linking_log[1024] = "";
        glGetProgramiv(noop_shader_program, GL_LINK_STATUS, &linking_success);

        if (linking_success == GL_FALSE)
        {
            std::cerr << "[WARNING] Failed to link shader to program " << noop_shader_program << std::endl;
            std::cerr << linking_log << std::endl;
        }

        glUseProgram(noop_shader_program);
    }

    std::vector<float> _positions;
    std::vector<float> _colors;
    std::vector<float> _texture_coords;
    std::vector<size_t> _indices;

    static float vertices[] = {
        // pos      col           tex_coord
        +1, +1, 0,   0, 1, 1,  1, 1, // top right
        +1, -1, 0,   1, 0, 1,  1, 0, // bottom right
        -1, -1, 0,   1, 1, 0,  0, 0, // bottom left
        -1, +1, 0,   0, 0, 1,  0, 1  // top left
    };

    static unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    GLNativeHandle vertex_array,
                   vertex_buffer,
                   element_buffer;

    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &element_buffer);

    glBindVertexArray(vertex_array);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vertex_array);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        //auto time = window.update();


        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }
}