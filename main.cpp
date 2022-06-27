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

    std::vector<float> _positions;
    std::vector<float> _colors;
    std::vector<float> _texture_coords;
    std::vector<size_t> _indices;

    _positions = {
        0, 0, 0,
        0, 1, 0,
        1, 1, 0,
        1, 0, 0
    };

    _colors = {
        1, 1, 0, 1,
        1, 0, 1, 1,
        0, 1, 1, 1,
        1, 0, 1, 1
    };

    _texture_coords = {
        0, 0,
        0, 1,
        1, 1,
        1, 0
    };

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        auto time = window.update();
        glClearColor(0, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(window.get_native());
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }
}
