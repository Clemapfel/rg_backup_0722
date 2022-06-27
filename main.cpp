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

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        auto time = window.update();

        SDL_GL_SwapWindow(window.get_native());
    }
}
