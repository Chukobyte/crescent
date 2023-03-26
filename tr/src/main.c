#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "tr_renderer.h"
#include "tr_texture.h"
#include "../seika/src/math/se_math.h"

#define TR_WINDOW_WIDTH 640
#define TR_WINDOW_HEIGHT 576
#define TR_GAME_RESOLUTION_WIDTH 160
#define TR_GAME_RESOLUTION_HEIGHT 144

bool tr_init_sdl();
void tr_render();

SDL_Window* window = NULL;
SDL_GLContext openGlContext;
bool isRunning = false;

TRTexture* bgImage = NULL;

int main(int argv, char** args) {
    // Init SDL
    if (!tr_init_sdl()) {
        return EXIT_FAILURE;
    }

    // Init Renderer
    tr_renderer_init(TR_GAME_RESOLUTION_WIDTH, TR_GAME_RESOLUTION_HEIGHT);

    // Main
    bgImage = tr_texture_create_texture("tr/assets/bottom_ui.png");

    isRunning = true;

    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                        case SDL_WINDOWEVENT_SIZE_CHANGED: {
                            const Sint32 windowWidth = event.window.data1;
                            const Sint32 windowHeight = event.window.data2;
                            break;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        tr_render();
    }

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


void tr_render() {
    static const SEColor backgroundColor = { 33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f, 1.0f };
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Render stuff
    tr_renderer_draw_sprite(bgImage, &(SEVector2){ .x = 0.0f, .y = 112.0f }, &(SESize2Di){ .w = 160, .h = 32 });

    SDL_GL_SwapWindow(window);
}
