#include "seika.h"

#include <time.h>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "input/input.h"
#include "utils/logger.h"
#include "utils/se_assert.h"
#include "rendering/renderer.h"
#include "audio/audio_manager.h"
#include "asset/asset_file_loader.h"
#include "asset/asset_manager.h"

bool initialize_sdl();
bool initialize_rendering(const char* title, int windowWidth, int windowHeight, int resolutionWidth, int resolutionHeight);
bool initialize_audio();
bool initialize_input(const char* controllerDBFilePath);

static SDL_Window* window = NULL;
static SDL_GLContext openGlContext;
static bool isRunning = false;

bool sf_initialize_simple(const char* title, int windowWidth, int windowHeight) {
    return sf_initialize(title, windowWidth, windowHeight, windowWidth, windowHeight, NULL);
}

bool sf_initialize(const char* title,
                   int windowWidth,
                   int windowHeight,
                   int resolutionWidth,
                   int resolutionHeight,
                   const char* controllerDBFilePath) {
    if (isRunning) {
        return false;
    }

    // Set random seed
    srand((int)time(NULL));

    // Initialize sub systems
    sf_asset_file_loader_initialize();

    if (!initialize_sdl()) {
        se_logger_error("Failed to initialize sdl!");
        return false;
    }
    if (!initialize_rendering(title, windowWidth, windowHeight, resolutionWidth, resolutionHeight)) {
        se_logger_error("Failed to initialize rendering!");
        return false;
    }
    if (!initialize_audio()) {
        se_logger_error("Failed to initialize audio!");
        return false;
    }
    if (!initialize_input(controllerDBFilePath)) {
        se_logger_error("Failed to initialize input!");
        return false;
    }

    se_asset_manager_initialize();

    isRunning = true;

    return true;
}

bool initialize_sdl() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        se_logger_error("Failed to initialize sdl!");
        return false;
    }
    return true;
}

bool initialize_rendering(const char* title, int windowWidth, int windowHeight, int resolutionWidth, int resolutionHeight) {
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
        se_logger_error("Failed to create window!");
        return false;
    }

    // Create OpenGL Context
    openGlContext = SDL_GL_CreateContext(window);

    // Initialize Glad
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        se_logger_error("Couldn't initialize glad!");
        return false;
    }

    se_renderer_initialize(windowWidth, windowHeight, resolutionWidth, resolutionHeight);
    return true;
}

bool initialize_audio() {
    return se_audio_manager_init();
}

bool initialize_input(const char* controllerDBFilePath) {
    if (!se_input_initialize(controllerDBFilePath)) {
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
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                const Sint32 windowWidth = event.window.data1;
                const Sint32 windowHeight = event.window.data2;
                se_renderer_update_window_size((float) windowWidth, (float) windowHeight);
                glViewport(0, 0, windowWidth, windowHeight);
                break;
            }
            break;
        default:
            break;
        }
        se_input_process(event);
    }
}

void sf_render() {
    static const Color backgroundColor = { 33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f, 1.0f };
    se_renderer_process_and_flush_batches(&backgroundColor);

    // TODO: Pass window to renderer and swap there?
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
        se_renderer_finalize();
        se_audio_manager_finalize();
        se_input_finalize();
        se_asset_manager_finalize();
        sf_asset_file_loader_finalize();
        isRunning = false;
    }
}
