//
// Created by clem on 6/26/22.
//

//#include <gtk-3.0/gtk/gtk.h>

#include <functional>

#include <iostream>
#include <string>

#include <include/opengl_common.hpp>
#include <include/gtk/gl_area.hpp>

#include <gtk/gtk.h>
#include <GL/gl.h>


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

static gboolean render (GtkGLArea *area, GdkGLContext *context)
{
    gtk_gl_area_make_current (area);

    static float counter = 0;
    counter += 0.1;
    glClearColor (counter, 0, 1, 1);
    glClear (GL_COLOR_BUFFER_BIT);
    std::cout << "render" << std::endl;

    auto port = rat::get_viewport_size();
    std::cout << port.x << " " << port.y << std::endl;

    // draw your object
    // draw_an_object ();

    // we completed our drawing; the draw commands will be
    // flushed at the end of the signal emission chain, and
    // the buffers will be drawn on the window
    return TRUE;
}

static void on_realize (GtkGLArea* area)
{
    // We need to make the context current if we want to
    // call GL API
    gtk_gl_area_make_current (area);

    auto* error_maybe = gtk_gl_area_get_error(area);
    if (error_maybe != nullptr)
        std::cout << error_maybe->message << std::endl;
}

static void on_shutdown(GtkGLArea* area)
{
    std::cout << "shutdown" << std::endl;
}

int main(int argc, char *argv[]) {

    gtk_init(&argc, &argv);

    // setup window
    auto* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), nullptr);
    gtk_widget_show(window);

    // setup gl
    auto* gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

    GError* error_maybe = nullptr;
    auto* context = gdk_window_create_gl_context(gdk_window, &error_maybe);
    if (error_maybe != nullptr)
        std::cerr << "[ERROR] In gdk_window_create_gl_context: " << error_maybe->message << std::endl;

    gdk_gl_context_set_required_version(context, 3, 2);
    gdk_gl_context_set_use_es(context, TRUE);

    gdk_gl_context_realize(context, &error_maybe);
    if (error_maybe != nullptr)
        std::cerr << "[ERROR] In gdk_gl_context_realize: " << error_maybe->message << std::endl;

    gdk_gl_context_make_current(context);

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
        std::cerr << "[ERROR] In glewInit: " << glewGetErrorString(glewError) << std::endl;

    // setup render area
    using namespace rat;
    auto canvas = GLCanvas({400, 300});

    auto native = canvas.get_native();
    gtk_widget_set_margin_top(native, 10);
    gtk_widget_set_margin_bottom(native, 10);
    gtk_widget_set_margin_start(native, 10);
    gtk_widget_set_margin_end(native, 10);

    gtk_widget_set_halign(native, GtkAlign::GTK_ALIGN_CENTER);
    gtk_widget_set_valign(native, GtkAlign::GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), native);
    gtk_gl_area_set_auto_render((GtkGLArea*) native, FALSE);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;

    /*
    auto* gdk_window = gtk_widget_get_window(GTK_WIDGET(window));

    GError* error_maybe;
    gdk_window_create_gl_context(gdk_window, &error_maybe);

    // gl area
    auto* gl_area = gtk_gl_area_new();

    gtk_gl_area_set_has_alpha(GTK_GL_AREA(gl_area), TRUE);
    gtk_widget_set_size_request(gl_area, 400, 300);

    gtk_widget_set_margin_top(gl_area, 10);
    gtk_widget_set_margin_bottom(gl_area, 10);
    gtk_widget_set_margin_left(gl_area, 10);
    gtk_widget_set_margin_right(gl_area, 10);

    gtk_widget_set_halign(gl_area, GtkAlign::GTK_ALIGN_CENTER);
    gtk_widget_set_valign(gl_area, GtkAlign::GTK_ALIGN_CENTER);
    gtk_container_add(GTK_CONTAINER(window), gl_area);
    gtk_gl_area_set_auto_render((GtkGLArea*) gl_area, FALSE);

    g_signal_connect(gl_area, "render", G_CALLBACK (render), NULL);
    g_signal_connect(gl_area, "realize", G_CALLBACK (on_realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK (on_shutdown), NULL);

    auto* context = gtk_gl_area_get_context(GTK_GL_AREA(gl_area));
    gdk_gl_context_set_required_version(context, 3, 3);
    std::cout << "legacy: " << gdk_gl_context_is_legacy(context) << std::endl;


    gtk_main();
    return 0;
     */
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
