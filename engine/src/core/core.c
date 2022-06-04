#include "core.h"

#include <time.h>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "game_properties.h"
#include "asset_manager.h"
#include "input/input.h"
#include "utils/logger.h"
#include "utils/command_line_args_util.h"
#include "utils/rbe_file_system_utils.h"
#include "utils/rbe_assert.h"
#include "scripting/python/rbe_py.h"
#include "rendering/renderer.h"
#include "audio/audio_manager.h"

bool rbe_initialize_sdl();
bool rbe_initialize_rendering();
bool rbe_initialize_audio();
bool rbe_initialize_input();
void rbe_process_inputs();
void rbe_process_game_logic();
void rbe_render();

static SDL_Window* window = NULL;
static SDL_GLContext openGlContext;
static bool isRunning = false;
RBEGameProperties* gameProperties = NULL;

bool rbe_initialize(int argv, char** args) {
    // Set random seed
    srand((int)time(NULL));

    rbe_logger_set_level(LogLevel_DEBUG);

    // TODO: Check for working directory overrides
    CommandLineFlagResult commandLineFlagResult = rbe_command_line_args_parse(argv, args);
    if (strcmp(commandLineFlagResult.workingDirOverride, "") != 0) {
        rbe_logger_debug("Changing working directory from override to '%s'.", commandLineFlagResult.workingDirOverride);
        rbe_fs_chdir(commandLineFlagResult.workingDirOverride);
        rbe_fs_print_cwd();
    }

    rbe_py_initialize();
    rbe_asset_manager_initialize();

    rbe_game_props_initialize(true);
    gameProperties = rbe_game_props_get();
    rbe_game_props_print();

    // Initialize sub systems
    if (!rbe_initialize_sdl()) {
        rbe_logger_error("Failed to initialize sdl!");
        return false;
    }
    if (!rbe_initialize_rendering()) {
        rbe_logger_error("Failed to initialize rendering!");
        return false;
    }
    if (!rbe_initialize_audio()) {
        rbe_logger_error("Failed to initialize audio!");
        return false;
    }
    if (!rbe_initialize_input()) {
        rbe_logger_error("Failed to initialize input!");
        return false;
    }

    rbe_logger_info("RBE Engine v%s initialized!", RBE_CORE_VERSION);
    isRunning = true;
    return true;
}

bool rbe_initialize_sdl() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        rbe_logger_error("Failed to initialize sdl!");
        return false;
    }
    return true;
}

bool rbe_initialize_rendering() {
    // OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    SDL_WindowFlags windowFlags = (SDL_WindowFlags)(
                                      SDL_WINDOW_OPENGL
                                      | SDL_WINDOW_RESIZABLE
                                      | SDL_WINDOW_ALLOW_HIGHDPI
                                  );
    RBEGameProperties* props = rbe_game_props_get();
    window = SDL_CreateWindow(
                 props->gameTitle,
                 SDL_WINDOWPOS_CENTERED,
                 SDL_WINDOWPOS_CENTERED,
                 props->windowWidth,
                 props->windowHeight,
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

    // Initialize renderer
    rbe_renderer_initialize();
    // Test texture
    rbe_asset_manager_load_texture("test_games/fighter_test/assets/images/melissa_walk_animation.png", "walk");
    Texture* texture = rbe_asset_manager_get_texture("walk");
    RBE_ASSERT(texture != NULL);
    return true;
}

bool rbe_initialize_audio() {
    return rbe_audio_manager_init();
}

bool rbe_initialize_input() {
    if (!rbe_input_initialize()) {
        return false;
    }
    // Temp shutdown input
    rbe_input_add_action_value("exit", "esc");
    // Temp play sfx
    rbe_input_add_action_value("play_sfx", "space");
    return true;
}

void rbe_update() {
    rbe_process_inputs();
    rbe_process_game_logic();
    rbe_audio_manager_process();
    rbe_render();
}

void rbe_process_inputs() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        default:
            break;
        }
    }
    rbe_input_process(event);
    // Temp shutdown
    if (rbe_input_is_action_just_pressed("exit")) {
        isRunning = false;
    }
    // Temp play sfx
    if (rbe_input_is_action_just_pressed("play_sfx")) {
        rbe_audio_manager_play_sound("test_games/fighter_test/assets/audio/sfx/rainbow_orb.wav");
    }
}

void rbe_process_game_logic() {
    static int lastFrameTime = 0;
    const int targetFps = gameProperties->targetFPS;
    const int MILLISECONDS_PER_TICK = 1000;
    const unsigned int FRAME_TARGET_TIME = MILLISECONDS_PER_TICK / targetFps;
    unsigned int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - lastFrameTime);
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        SDL_Delay(timeToWait);
    }
    lastFrameTime = (int) SDL_GetTicks();

    rbe_input_clean_up_flags();
}

void rbe_render() {
    static Color backgroundColor = { 33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f, 1.0f };
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Texture* walkTexture = rbe_asset_manager_get_texture("walk");
    RBE_ASSERT(walkTexture != NULL);
    static Rect2 sourceRect = { 0.0f, 0.0f, 32.0f, 32.0f };
    static Rect2 destRect = { 300.0f, 300.0f, 32.0f * 2.0f, 32.0f * 2.0f };
    static Color walkTextureColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    rbe_renderer_queue_sprite_draw_call(walkTexture, sourceRect, destRect, 0.0f, walkTextureColor, false, false);
    rbe_renderer_flush_batches();

    SDL_GL_SwapWindow(window);
}

bool rbe_is_running() {
    return isRunning;
}

void rbe_shutdown() {
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(openGlContext);
    SDL_Quit();
    rbe_py_finalize();
    rbe_logger_info("RBE Engine shutdown!");
}
