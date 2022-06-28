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

    static float vertices[] = {
        // pos,          col,       tex_coord
        +0.5, +0.5, 0,   1, 0, 0,   1, 1, // top right
        +0.5, -0.5, 0,   0, 1, 0,   1, 0, // bottom right
        -0.5, -0.5, 0,   0, 1, 1,   0, 0, // bottom left
        -0.5, +0.5, 0,   1, 0, 1,   0, 1  // top left
    };

    static unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    GLNativeHandle vertex_array, vertex_buffer, element_buffer;

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

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        //auto time = window.update();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vertex_array);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow( gWindow );
    }
}
