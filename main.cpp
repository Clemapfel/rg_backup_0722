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

    SDL_Texture* texture = IMG_LoadTexture(window.get_renderer(), "/home/clem/Workspace/mousetrap/mole.png");
    SDL_GL_UnbindTexture(texture);


    size_t frame = 50;
    size_t width = window.get_size().x;
    size_t height = window.get_size().y;

    auto rect = Shape();
    rect.as_rectangle(Vector2f(0 + frame, 0 + frame), Vector2f(width - 2*frame, height - 2*frame));

    auto tri = Shape();
    tri.as_triangle(Vector2f(0.5 * width, frame), Vector2f(frame, height - frame), Vector2f(width - frame, height - frame));

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        auto time = window.update();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1, 1, 1, 1);

        SDL_GL_BindTexture(texture, nullptr, nullptr);
        //rect.render(window);
        tri.render(window);

        SDL_GL_SwapWindow(window.get_native());
    }

    /*
    auto gProgramID = shader.get_program_id();
    auto vertex_pos_location = 0;
    auto vertex_color_location = 1;
    auto vertex_tex_coord_location = 2;

    glViewport(0, 0, 400, 300);

    std::array<GLint, 4> viewport;
    glGetIntegerv(GL_VIEWPORT, viewport.data());

    float frame = 50;
    size_t width = viewport.at(2);
    size_t height = viewport.at(3);
    Vector2ui top_left  = Vector2ui(viewport.at(0), viewport.at(1) - height);

    std::vector<float> positions = {
        0 + frame, 0 + frame, 0,
        width - frame, 0 + frame, 0,
        width - frame, height - frame, 0,
        0 + frame, height - frame, 0
    };

    auto project_position = [&](Vector2f local_in) -> Vector2f {

        Vector2f centroid = Vector2f(width / 2, height / 2);
        local_in -= centroid;
        local_in.x /= width / 2;
        local_in.y /= height / 2;
        return local_in;
    };

    auto project_position_reverse = [&](Vector2f in) -> Vector2f {

        Vector2f centroid = Vector2f(width / 2, height / 2);
        in.x *= (width / 2);
        in.y *= (height / 2);
        in += centroid;
        return in;
    };

    // translate to pixel coordiantes
    for (size_t i = 0; i < positions.size(); i += 3)
    {
        auto in = Vector2f(positions.at(i), positions.at(i+1));
        auto out = project_position(in);
        positions.at(i) = out.x;
        positions.at(i+1) = out.y;
    }

    for (size_t i = 0; i < positions.size(); i += 3)
        std::cout << positions.at(i) << " " << positions.at(i+1) << " " << positions.at(i+2) << std::endl;

    std::vector<float> colors = {
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1
    };

    std::vector<float> tex_coords = {
        0, 0,
        0, 1,
        1, 1,
        1, 0
    };

    // translate to sdl coordinates axis orientation
    auto project_tex_coord = [](Vector2f in) -> Vector2f {

        auto out = Vector2f(in.y, in.x);
        out.x = 1 - out.x;
        out.y = 1 - out.y;
        return out;
    };

    auto project_tex_coord_reverse = [](Vector2f in) -> Vector2f {

        auto out = in;
        out.x = 1 - out.x;
        out.y = 1 - out.y;
        return Vector2f(out.y, out.x);
    };

    for (size_t i = 0; i < tex_coords.size(); i += 2)
    {
        auto in = Vector2f(tex_coords.at(i), tex_coords.at(i+1));
        auto out = project_tex_coord(in);
        tex_coords.at(i) = out.x;
        tex_coords.at(i+1) = out.y;
    }

    for (size_t i = 0; i < tex_coords.size(); i += 2)
        std::cout << tex_coords.at(i) << " " << tex_coords.at(i+1) << std::endl;

    std::vector<uint32_t> indices = {
        0, 1, 2, 1, 2, 3
    };

    GLNativeHandle vertex_array, element_buffer;
    GLNativeHandle position_buffer, color_buffer, tex_coord_buffer;

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_pos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(vertex_pos_location);

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_color_location, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(vertex_color_location);

    glGenBuffers(1, &tex_coord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, tex_coords.size() * sizeof(float), tex_coords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_tex_coord_location, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0);
    glEnableVertexAttribArray(vertex_tex_coord_location);

    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    SDL_Color sdl_colors[] = {
        SDL_Color(), SDL_Color(), SDL_Color(), SDL_Color(),
    };

    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        //auto time = window.update();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1, 1, 1, 1);

        glUseProgram(shader.get_program_id());
        glBindVertexArray(vertex_array);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

        SDL_GL_BindTexture(texture, nullptr, nullptr);
        glUniform1i(glGetUniformLocation(shader.get_program_id(), "_texture"), 0);
        glDrawElements(GL_TRIANGLE_FAN, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window.get_native());

    }
     */
}
