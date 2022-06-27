//
// Created by clem on 6/26/22.
//

#include <mousetrap.hpp>
#include <thread>
#include <iostream>

#include <GLES3/gl32.h>
#include <GL/gl.h>

using namespace rat;

using GLNativeHandle = GLuint;

int main()
{
    auto window = Window();
    window.create("test", 400, 300);

    float vertices[] =
    {
        // pos, col, tex_coord
        1,  1, 0,   1, 1, 1,   1, 1, // top right
        1, -1, 0,   1, 1, 1,   1, 0, // bottom right
        -1, -1, 0,   1, 1, 1,   0, 0, // bottom left
        -1,  1, 0,   1, 1, 1,   0, 1  // top left
    };

    static unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    GLNativeHandle _vertex_array;
    GLNativeHandle _vertex_buffer;
    GLNativeHandle _element_buffer;

    glGenVertexArrays(1, &_vertex_array);
    glGenBuffers(1, &_vertex_buffer);
    glGenBuffers(1, &_element_buffer);

    glBindVertexArray(_vertex_array);

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
        auto time = window.update();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);


        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }
}