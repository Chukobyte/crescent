#include "seika.h"

#include <time.h>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "input/input.h"
#include "utils/logger.h"
#include "utils/rbe_assert.h"
#include "rendering/renderer.h"
#include "audio/audio_manager.h"
#include "asset_manager.h"

bool initialize_sdl();
bool initialize_rendering(const char* title, int windowWidth, int windowHeight);
bool initialize_audio();
bool initialize_input();

static SDL_Window* window = NULL;
static SDL_GLContext openGlContext;
static bool isRunning = false;

bool sf_initialize(const char* title, int windowWidth, int windowHeight) {
    if (isRunning) {
        return false;
    }

    // Set random seed
    srand((int)time(NULL));

    // Initialize sub systems
    if (!initialize_sdl()) {
        rbe_logger_error("Failed to initialize sdl!");
        return false;
    }
    if (!initialize_rendering(title, windowWidth, windowHeight)) {
        rbe_logger_error("Failed to initialize rendering!");
        return false;
    }
    if (!initialize_audio()) {
        rbe_logger_error("Failed to initialize audio!");
        return false;
    }
    if (!initialize_input()) {
        rbe_logger_error("Failed to initialize input!");
        return false;
    }

    rbe_asset_manager_initialize();

    isRunning = true;

    return true;
}

bool initialize_sdl() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        rbe_logger_error("Failed to initialize sdl!");
        return false;
    }
    return true;
}

bool initialize_rendering(const char* title, int windowWidth, int windowHeight) {
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
                 title,
                 SDL_WINDOWPOS_CENTERED,
                 SDL_WINDOWPOS_CENTERED,
                 windowWidth,
                 windowHeight,
                 windowFlags
             );
    if (!window) {
        rbe_logger_error("Failed to create window!");
        return false;
    }

    // Create OpenGL Context
    openGlContext = SDL_GL_CreateContext(window);

    // Initialize Glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        rbe_logger_error("Couldn't initialize glad!");
        return false;
    }

    sf_renderer_initialize();
    return true;
}

bool initialize_audio() {
    return rbe_audio_manager_init();
}

bool initialize_input() {
    if (!cre_input_initialize()) {
        return false;
    }
    return true;
}

void sf_update() {}

void sf_process_inputs() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        default:
            break;
        }
        cre_input_process(event);
    }
}

void sf_render() {
//    static const Color backgroundColor = { 33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f, 1.0f };
//    cre_renderer_process_and_flush_batches(&backgroundColor);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    SDL_GL_SwapWindow(window);
}

bool sf_is_running() {
    return isRunning;
}

void sf_shutdown() {
    if (isRunning) {
        SDL_DestroyWindow(window);
        SDL_GL_DeleteContext(openGlContext);
        SDL_Quit();
        sf_renderer_finalize();
        rbe_audio_manager_finalize();
        cre_input_finalize();
        rbe_asset_manager_finalize();
        isRunning = false;
    }
}
