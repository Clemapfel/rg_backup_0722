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

    //Get vertex attribute location
    auto gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D" );
    if( gVertexPos2DLocation == -1 )
    {
        printf( "LVertexPos2D is not a valid glsl program variable!\n" );
    }

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //VBO data
    GLfloat vertexData[] =
    {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f,  0.5f,
            -0.5f,  0.5f
    };

    //IBO data
    GLuint indexData[] = { 0, 1, 2, 3 };
    GLNativeHandle gVBO, gIBO;

    //Create VBO
    glGenBuffers( 1, &gVBO );
    glBindBuffer( GL_ARRAY_BUFFER, gVBO );
    glBufferData( GL_ARRAY_BUFFER, 2 * 4 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW );

    //Create IBO
    glGenBuffers( 1, &gIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW );

    while (not InputHandler::exit_requested())
    {
        InputHandler::update();
        //auto time = window.update();

        glClear( GL_COLOR_BUFFER_BIT );

        //Bind program
        glUseProgram( gProgramID );

        //Enable vertex position
        glEnableVertexAttribArray( gVertexPos2DLocation );

        //Set vertex data
        glBindBuffer( GL_ARRAY_BUFFER, gVBO );
        glVertexAttribPointer( gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL );

        //Set index data and render
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, gIBO );
        glDrawElements( GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL );

        //Disable vertex position
        glDisableVertexAttribArray( gVertexPos2DLocation );

        //Unbind program
        glUseProgram( NULL );

        SDL_GL_SwapWindow( gWindow );
    }
}
