//
// Created by clem on 6/26/22.
//

#include <mousetrap.hpp>
#include <thread>
#include <iostream>
#include <vector>
#include <array>

using namespace rat;

using GLNativeHandle = GLuint;

int main()
{
    auto window = Window();
    window.create("test", 400, 300);

    auto shader = Shader();

    SDL_Window* gWindow = window.get_native();
    SDL_GLContext gContext = window.get_context();
    auto gProgramID = shader.get_program_id();

    auto vertex_pos_location = 0;
    auto vertex_tex_coord_location = 1;
    auto vertex_color_location = 2;

    float positions[] = {
        +0.5, +0.5, 0,
        +0.5, -0.5, 0,
        -0.5, -0.5, 0,
        -0.5, +0.5, 0
    };

    float colors[] = {
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1,
        1, 0, 0, 1
    };

    float tex_coords[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1
    };

    static unsigned int indices[] = {
        0, 1, 3, 1, 2, 3
    };

    GLNativeHandle vertex_array, element_buffer;
    GLNativeHandle position_buffer, color_buffer, tex_coord_buffer;

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(2, &tex_coord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        //auto time = window.update();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader.get_program_id());

        glBindVertexArray(vertex_array);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow( gWindow );
    }
}
