//
// Created by clem on 6/26/22.
//

#include <mousetrap.hpp>
#include <iostream>

int main()
{
    rat::InputHandler::initialize();

    while (not rat::InputHandler::exit_requested())
    {
        if (rat::InputHandler::was_pressed(rat::KeyboardKey::SPACE))
            std::cout << "space" << std::endl;
    }
}