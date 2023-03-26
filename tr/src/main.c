#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "tr_renderer.h"
#include "tr_texture.h"

#define TR_WINDOW_WIDTH 800
#define TR_WINDOW_HEIGHT 600

bool tr_init_sdl();

SDL_Window* window = NULL;
SDL_GLContext openGlContext;

int main(int argv, char** args) {
    // Init SDL
    if (!tr_init_sdl()) {
        return EXIT_FAILURE;
    }

    // Init Renderer
    tr_renderer_init(TR_WINDOW_WIDTH, TR_WINDOW_HEIGHT);

    // Main
    TRTexture* bgImage = tr_texture_create_texture("tr/assets/bottom_ui.png");
    tr_texture_delete(bgImage);

    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(openGlContext);
    SDL_Quit();
    return EXIT_SUCCESS;
}

bool tr_init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER ) != 0) {
        printf("Failed to initialize SDL!  Error: '%s'", SDL_GetError());
        return false;
    }
    // Rendering
    // OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    const SDL_WindowFlags windowFlags = (SDL_WindowFlags)(
                                            SDL_WINDOW_OPENGL
                                            | SDL_WINDOW_RESIZABLE
                                            | SDL_WINDOW_ALLOW_HIGHDPI
                                        );
    window = SDL_CreateWindow(
                 "tr test",
                 SDL_WINDOWPOS_CENTERED,
                 SDL_WINDOWPOS_CENTERED,
                 TR_WINDOW_WIDTH,
                 TR_WINDOW_HEIGHT,
                 windowFlags
             );
    if (!window) {
        printf("Failed to create window!  SDL error: '%s'", SDL_GetError());
        return false;
    }

    // Create OpenGL Context
    openGlContext = SDL_GL_CreateContext(window);

    // Initialize Glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Couldn't initialize glad!");
        return false;
    }
    return true;
}

