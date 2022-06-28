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

    SDL_Texture* texture = IMG_LoadTexture(window.get_renderer(), "/home/clem/Workspace/mousetrap/mole.png");
    SDL_GL_UnbindTexture(texture);

    auto vertex_pos_location = 0;
    auto vertex_color_location = 1;
    auto vertex_tex_coord_location = 2;

    float positions[] = {
        +0.5, +0.5, 0,
        +0.5, -0.5, 0,
        -0.5, -0.5, 0,
        -0.5, +0.5, 0
    };

    float colors[] = {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };

    float tex_coords[] = {
        0, 0,
        0, 1,
        1, 1,
        1, 0
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
    glVertexAttribPointer(vertex_pos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(vertex_pos_location);

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_color_location, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(vertex_color_location);

    glGenBuffers(1, &tex_coord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_tex_coord_location, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(vertex_tex_coord_location);

    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    SDL_Color sdl_colors[] = {
        SDL_Color(), SDL_Color(), SDL_Color(), SDL_Color(),
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        //auto time = window.update();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1, 1, 1, 1);

        glUseProgram(shader.get_program_id());
        glBindVertexArray(vertex_array);

        SDL_GL_BindTexture(texture, nullptr, nullptr);
        glUniform1i(glGetUniformLocation(shader.get_program_id(), "_texture"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window.get_native());

    }
}
