/* standard headers */
#include <stdio.h>
#include <stdbool.h>

/* library headers */
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "GL/GL.h"

/* Definitions for window parameters */
#include "headers/windowdefs.h"

/* Function declaration */
bool initSDL( SDL_Window** );
void sdlLoop( SDL_Window* );
bool initOpenGL( SDL_Window*, SDL_GLContext* );

/* Program Entry */
int main( int argc, char** argv )
{
 
    // Init SDL
    SDL_Window* sdl_window = NULL;
    if( !initSDL( &sdl_window ) )
    {
        printf( "Could not start SDL, the application will exit.\n" );
        goto main_exit;
    }

    printf("%p\n", sdl_window);
    
    // Init OpenGL
    SDL_GLContext* sdl_glContext = NULL;
    if( !initOpenGL( sdl_window, sdl_glContext ) )
    {
        printf( "Error initializing OpenGL.\n" );
        goto main_exit;
    }

    glClearColor(0,1,0.5,1);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(sdl_window);
    
    // Main loop
    sdlLoop( sdl_window );

main_exit:
    SDL_Quit();
    printf( "Program end.\n");
    return 0;
}

bool initSDL( SDL_Window** wnd )
{
    // Initialise SDL
    if( SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        // Error initializing SDL
        printf( "SDL Error: %s\n", SDL_GetError() );
        return false;
    }

    // Create a window
    *wnd = SDL_CreateWindow( W_PARAMS ); // W_PARAMS is defined in windowdefs.h
    printf( "Window pointer: %p.\n", *wnd );
    if( wnd != NULL )
        return true;

    printf( "Error creating SDL window: %s", SDL_GetError() );
    SDL_Quit();
    return false;
}

void sdlLoop( SDL_Window* wnd )
{
    // poll and process events
    SDL_Event ev;
    int n = 0;
    while ( 1 )
    {
        SDL_PollEvent( &ev );
        // Process event
        switch ( ev.type )
        {
        case SDL_QUIT:
            goto sdlLoop_quit;
        
        default:
            break;
        }

        if( n++ >= 1000){printf( "Event loop...\n" );n = 0;}
        SDL_Delay( 10 );
    }
sdlLoop_quit:
    SDL_Quit();
}

bool initOpenGL(SDL_Window* wnd, SDL_GLContext* ctx)
{
    // Create OpenGL context
    ctx = SDL_GL_CreateContext(wnd);
    
    if( ctx == NULL )
    {
        printf( "Error creating OpenGL context: %s.\n", SDL_GetError());
        return true;
    }
    return true;
}
