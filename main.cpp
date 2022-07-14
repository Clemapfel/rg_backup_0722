//
// Created by clem on 6/26/22.
//

//#include <gtk-3.0/gtk/gtk.h>

#include <functional>

#include <iostream>
#include <string>

#include <gtk/gtk.h>

void clicked()
{
    std::cout << "clicked" << std::endl;
}

template<typename Return_t, typename... Args_t>
struct LambdaProxy
{
    static inline std::function<Return_t(Args_t...)> _lambda = [](Args_t...) -> Return_t {};

    static Return_t invoke(Args_t... args) {
        return _lambda(args...);
    }
};

template<typename Return_t, typename... Args_t, typename Lambda_t>
auto forward_as_static(Lambda_t lambda)
{
    LambdaProxy<Return_t, Args_t...>::_lambda = lambda;
    return &LambdaProxy<Return_t, Args_t...>::invoke;
}

int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);

    auto* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit), nullptr);

    auto* button = gtk_button_new_with_label("Button");
    gtk_widget_set_tooltip_text(button, "Button widget");
    gtk_widget_set_halign(button, GtkAlign::GTK_ALIGN_CENTER);
    gtk_widget_set_valign(button, GtkAlign::GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), button);

    auto lambda = [](GtkMenuItem* menuitem, gpointer data) -> void {
        std::cout << *((std::string*) data) << std::endl;
    };

    auto* as_static = forward_as_static<void, GtkMenuItem*, gpointer>(lambda);
    std::string data = "string called";

    g_signal_connect(button, "clicked", G_CALLBACK(as_static), &data);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
/*

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

    auto content = "I'm rendering in real-time to OpenGL and you have to <b>constantly</b> fight the rasterizer because if your alignment is one fragment off letters get all crunchy and compressed, but now they look crisp and smooth so I'm proud of the engine.\n\n<col=(0.1, 0.1, 0.1)>Also it can do <fx_s>this</fx_s></col>";
    auto position = {50, 50};
    auto width = window.get_size().x - 2 * 50;
    auto text = Text(48, "Roboto");
    text.set_alignment(Text::JUSTIFIED);
    text.create(window, {50, 50}, content, width, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_SRC_COLOR);
    glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_SUBTRACT);

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
        //for (auto& shape : shapes)
          //  shape.render(&window);

        window.flush();
        window.display();
    }

    return 0;
}

*/
