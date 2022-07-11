//
// Created by clem on 6/26/22.
//

#include <mousetrap.hpp>
#include <thread>
#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <chrono>

#include <SDL2/SDL_ttf.h>

using namespace rat;

using GLNativeHandle = GLuint;

float rng()
{
    auto dist = std::uniform_real_distribution<float>(0, 1);
    auto device = std::random_device();
    auto engine = std::mt19937();
    engine.seed(device());
    return dist(engine);
}

int main()
{
    auto window = Window();
    window.create("test", 800, 600);

    auto camera = Camera(&window);
    std::vector<Shape> shapes;

    shapes.push_back(CircleShape({80, 80}, 5, 4));
    shapes.back().set_color(RGBA(1, 0, 0, 1));

    shapes.push_back(RectangleShape({100, 100}, {400, 300})); //, 50));
    shapes.back().set_centroid(Vector2f(window.get_size().x * 0.5, window.get_size().y * 0.5));

    auto content = "In animal crossing, scrolling <b>pauses</b> on punctuation. This makes the scroll dialogue feel as if it was being read out loud. Other than this, I stole the format tags from <b>Paper Mario: TTYD</b>, which uses|| <fx_r><fx_w>RAINBOW TEXT</fx_w></fx_r> like dis.|||||||||||\n\n<col=(0.25, 0.25, 0.25)>(also Undertale but <b><fx_s>fuck</fx_s></b> that game)</col>";
    auto position = {50, 50};
    auto width = window.get_size().x - 2 * 50;
    auto text = Text(48, "Roboto");
    text.create(window, {50, 50}, content, width);

    // TODO
    auto texture = RenderTexture(window);
    texture.load("/home/clem/Workspace/mousetrap/mole.png");

    auto render_texture = RenderTexture(window);
    render_texture.load("/home/clem/Workspace/mousetrap/mole.png");

    size_t _width = render_texture.get_size().x;
    size_t _height = render_texture.get_size().y;

    /*
    std::vector<float> data;
    data.reserve(_width * _height * 4);
    for (size_t i = 0; i < _width * _height; ++i)
    {
        data.push_back(1);
        data.push_back(0);
        data.push_back(0);
        data.push_back(1);
    }

    glBindTexture(GL_TEXTURE_2D, render_texture.get_native_handle());
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA32F,
                 _width,
                 _height,
                 0,
                 GL_RGBA,
                 GL_FLOAT,
                 data.data());
                 */

    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    glBindTexture(GL_TEXTURE_2D, render_texture.get_native_handle());

    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, render_texture.get_native_handle(), 0);
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "framebuffer incomplete" << std::endl;

    /*
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 1, 1, 1);
    text.render(&window);
     */

    SDL_GL_MakeCurrent(window.get_native(), window.get_context());
    SDL_RenderClear(window.get_renderer());

    auto rect = SDL_Rect();
    rect.x = 0;
    rect.y = 0;
    rect.w = 10000;
    rect.h = 10000;

    SDL_SetRenderDrawColor(window.get_renderer(), 0, 255, 255, 255);
    SDL_RenderDrawRect(window.get_renderer(), &rect);
    SDL_RenderFlush(window.get_renderer());
    SDL_RenderPresent(window.get_renderer());

    text.render(&window);
    SDL_GL_SwapWindow(window.get_native());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_RenderClear(window.get_renderer());

    for (auto& shape : shapes)
      shape.set_texture(&render_texture);

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        auto time = window.update();

        if (InputHandler::was_pressed(ESCAPE))
            break;

        window.clear();

        if (InputHandler::is_down(KeyboardKey::UP))
        {
            camera.move(0, -10);
        }

        if (InputHandler::is_down(KeyboardKey::DOWN))
        {
            camera.move(0, +10);
        }

        if (InputHandler::is_down(KeyboardKey::LEFT))
        {
            camera.move(-10, 0);
        }

        if (InputHandler::is_down(KeyboardKey::RIGHT))
        {
            camera.move(+10, 0);
        }

        static auto angle = degrees(0);

        if (InputHandler::is_down(KeyboardKey::X))
        {
            angle += degrees(1);
            camera.set_rotation(angle);
        }

        if (InputHandler::is_down(KeyboardKey::Y))
        {
            angle -= degrees(1);
            camera.set_rotation(angle);
        }

        if (InputHandler::was_pressed(KeyboardKey::PLUS))
        {
            camera.zoom_in(1.3);
        }

        if (InputHandler::was_pressed(KeyboardKey::MINUS))
        {
            camera.zoom_out(1.3);
        }

        if (InputHandler::was_pressed(KeyboardKey::SPACE))
        {
            camera.center_on(shapes.front().get_centroid());
        }

        auto clock = Clock();
        text.update(time);
        text.render(&window);

        const float offset = 10;
        for (auto& shape : shapes)
            shape.render(&window);

        window.flush();
        window.display();
    }

    return 0;

    /*

    auto px = 48;
    auto text = Text(px, "Roboto");
    auto str = "Lorem ipsum dolor sit amet, <col=(1, 0, 1)>consectetur</col> adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur...";

    text.create(window, {50, 25}, str, window.get_size().x - 2 * 50);

    Vector2f align_point = {200, 150};
    auto align_dot = CircleShape(align_point + Vector2f(0.5, 0.5), 3, 4);//RectangleShape(aabb.top_left, aabb.size);
    align_dot.set_color(RGBA(1, 0, 0, 1));

    auto aabb = text.get_bounding_box();
    auto rect = RectangleShape(aabb.top_left, aabb.size);
    rect.set_color(RGBA(0.1, 0.1, 0.1, 1));

    text.set_alignment(Text::JUSTIFIED);

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        auto time = window.update();

        if (InputHandler::was_pressed(ESCAPE))
            break;

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0, 0, 0, 1);

        rect.render(window);
        text.render(window);
        align_dot.render(window);

        if (InputHandler::was_pressed(UP))
            text.set_alignment(Text::CENTERED);
        if (InputHandler::was_pressed(LEFT))
            text.set_alignment(Text::FLUSH_LEFT);
        if (InputHandler::was_pressed(RIGHT))
            text.set_alignment(Text::FLUSH_RIGHT);
        if (InputHandler::was_pressed(DOWN))
            text.set_alignment(Text::JUSTIFIED);

        SDL_GL_SwapWindow(window.get_native());
        SDL_Delay(1000 / 15);
    }

     /*
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

    auto line = Shape();
    line.as_line(Vector2f(0, 0), Vector2f(width / 2, height / 2));

    auto circle = Shape();
    circle.as_circle(Vector2f(width / 2, height / 2), frame * 2, 6);

    std::vector<Vector2f> vertices;
    for (size_t i = 1; i < circle.get_n_vertices(); ++i)
        vertices.push_back(Vector2f(rng() * width * 0.5, rng() * height * 0.5));

    auto line_strip = Shape();
    line_strip.as_line_strip(vertices);

    auto line_loop = Shape();
    line_loop.as_wireframe(vertices);

    auto polygon = Shape();
    polygon.as_polygon(vertices);

    auto frame_shape = Shape();
    frame_shape.as_frame(Vector2f(0 + frame, 0 + frame), Vector2f(width - 2*frame, height - 2*frame), 30);

    if (TTF_Init() == -1)
        std::cerr << "In TODO: initialization of ttf engine failed: " << SDL_GetError() << std::endl;

    auto* font = TTF_OpenFont("/home/clem/Workspace/mousetrap/resources/fonts/Roboto-Black.ttf", 25);
    auto glyph = TTF_RenderText_Solid(font, "ABCDestyjl", as_sdl_color(RGBA(0, 0, 0, 1)));//, 150);

    auto get_size = [](SDL_Texture* texture) -> Vector2f
    {
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        return Vector2f(width, height);
    };

    auto glyph_texture = Texture(window.get_renderer());
    glyph_texture.create(glyph);
    glyph_texture.bind();
    glTexParameteri(GL_TEXTURE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glyph_texture.unbind();

    auto glyph_point = CircleShape(Vector2f(frame, frame), 2, 4);
    glyph_point.set_color(RGBA(1, 0, 0, 1));

    auto ascend = TTF_FontAscent(font);
    auto descend = TTF_FontDescent(font);

    auto pos = Vector2f(frame, frame);
    pos.y -= ascend * 0.5;
    auto glyph_shape = RectangleShape(pos, Vector2f(300, 200)); // glyph_texture.get_size());
    glyph_shape.set_texture(&glyph_texture);

    auto image = Image();
    image.create(500, 500, RGBA(1, 0, 0, 1));
    for (size_t i = 0; i < 500; ++i)
        image.set(i, i, RGBA(0, 0, 0, 1));

    auto image_texture = Texture(window.get_renderer());
    image_texture.create(image);

    rect.set_texture(&image_texture);

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        auto time = window.update();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1, 1, 1, 1);

        // glyph_shape.render(window);
        // glyph_point.render(window);

        rect.render(window);
        // tri.render(window);
        // line.render(window);
        // circle.render(window);
        // line_strip.render(window);
        // line_loop.render(window);
        // polygon.render(window);
        // frame_shape.render(window);

        SDL_GL_SwapWindow(window.get_native());
        SDL_Delay(1000 / 15);
    }
     */


}
