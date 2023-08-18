/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  main 
 *
 *        Version:  1.0
 *        Created:  05/18/2023 05:48:52 PM
 *       Revision:  none
 *       Compiler:  gcc/VisualStudio
 *
 *         Author:  Patchcoat
 *   Organization:  
 *
 * =====================================================================================
 */

#ifdef linux
#include <SDL2/SDL.h>
#endif
#ifdef _WIN32
#include <SDL.h>
#endif
#include <glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* window = NULL;

//The surface contained by the window
SDL_Surface* screenSurface = NULL;


void poll_events(SDL_Event *e, int *quit) {
    while (SDL_PollEvent(e))
    {
        if (e->type == SDL_QUIT)
        {
            *quit = 1;
        }
    }
}

void CRGA_Close() {
    //Destroy window
    SDL_DestroyWindow( window );

    //Quit SDL subsystems
    SDL_Quit();
}

void CRGA_Init() {
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );

        //Quit SDL subsystems
        SDL_Quit();

        exit(1);
    }

    // Create window
    window = SDL_CreateWindow("CRGA Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());

        CRGA_Close();

        exit(1);
    }

    // Get window surface
    screenSurface = SDL_GetWindowSurface(window);

    // Fill the surface white
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

    // Update the surface
    SDL_UpdateWindowSurface(window);
}

void CRGA_Draw() {

}

int main(void) {
    CRGA_Init();

    // main loop 
    SDL_Event e;
    int quit = 0;
    while (!quit) {
        poll_events(&e, &quit);
        CRGA_Draw();
    }

    CRGA_Close();

    return 0;
}
