#include "core.h"

#include <time.h>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "asset_manager.h"
#include "audio/audio_manager.h"
#include "ecs/ecs_manager.h"
#include "ecs/system/ec_system.h"
#include "engine_context.h"
#include "game_properties.h"
#include "input/input.h"
#include "rendering/renderer.h"
#include "scene/scene_manager.h"
#include "scripting/python/rbe_py.h"
#include "utils/command_line_args_util.h"
#include "utils/logger.h"
#include "utils/rbe_assert.h"
#include "utils/rbe_file_system_utils.h"

bool rbe_initialize_sdl();

bool rbe_initialize_rendering();

bool rbe_initialize_audio();

bool rbe_initialize_input();

bool rbe_initialize_ecs();

bool rbe_load_assets_from_configuration();

void rbe_process_inputs();

void rbe_process_game_update();

void rbe_render();

static SDL_Window* window = NULL;
static SDL_GLContext openGlContext;
RBEGameProperties* gameProperties = NULL;
RBEEngineContext* engineContext   = NULL;

bool rbe_initialize(int argv, char** args) {
    // Set random seed
    srand((int) time(NULL));

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
    if (!rbe_initialize_ecs()) {
        rbe_logger_error("Failed to initialize ecs!");
        return false;
    }

    rbe_scene_manager_initialize();

    rbe_load_assets_from_configuration();

    rbe_logger_info("RBE Engine v%s initialized!", RBE_CORE_VERSION);
    engineContext            = rbe_engine_context_initialize();
    engineContext->targetFPS = gameProperties->targetFPS;
    engineContext->isRunning = true;

    // Go to initial scene
    rbe_scene_manager_queue_scene_change(gameProperties->initialScenePath);

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
    SDL_WindowFlags windowFlags =
        (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    RBEGameProperties* props = rbe_game_props_get();
    window = SDL_CreateWindow(props->gameTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, props->windowWidth,
                              props->windowHeight, windowFlags);
    if (!window) {
        rbe_logger_error("Failed to create window!");
        return false;
    }

    // Create OpenGL Context
    openGlContext = SDL_GL_CreateContext(window);

    // Initialize Glad
    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        rbe_logger_error("Couldn't initialize glad!");
        return false;
    }

    rbe_renderer_initialize();
    return true;
}

bool rbe_initialize_audio() { return rbe_audio_manager_init(); }

bool rbe_initialize_input() {
    if (!rbe_input_initialize()) {
        return false;
    }
    return true;
}

bool rbe_initialize_ecs() {
    rbe_ecs_manager_initialize();
    return true;
}

bool rbe_load_assets_from_configuration() {
    // Audio Sources
    for (size_t i = 0; i < gameProperties->audioSourceCount; i++) {
        const RBEAssetAudioSource assetAudioSource = gameProperties->audioSources[i];
        rbe_asset_manager_load_audio_source_wav(assetAudioSource.file_path, assetAudioSource.file_path);
    }

    // Textures
    for (size_t i = 0; i < gameProperties->textureCount; i++) {
        const RBEAssetTexture assetTexture = gameProperties->textures[i];
        rbe_asset_manager_load_texture(assetTexture.file_path, assetTexture.file_path);
    }

    // Fonts
    for (size_t i = 0; i < gameProperties->fontCount; i++) {
        const RBEAssetFont assetFont = gameProperties->fonts[i];
        rbe_asset_manager_load_font(assetFont.file_path, assetFont.uid, assetFont.size);
    }

    // Inputs
    for (size_t i = 0; i < gameProperties->inputActionCount; i++) {
        const RBEInputAction inputAction = gameProperties->inputActions[i];
        for (size_t valueIndex = 0; valueIndex < inputAction.valueCount; valueIndex++) {
            const char* actionValue = inputAction.values[valueIndex];
            rbe_logger_debug("action_name = %s, action_value = %s", inputAction.name, actionValue);
            rbe_input_add_action_value(inputAction.name, actionValue, inputAction.deviceId);
        }
    }

    return true;
}

void rbe_update() {
    // Process Scene change if exists
    rbe_scene_manager_process_queued_scene_change();

    // Clear out queued nodes for deletion
    rbe_scene_manager_process_queued_deletion_entities();

    // Create nodes queued for creation a.k.a. '_start()'
    rbe_scene_manager_process_queued_creation_entities();

    // Main loop
    rbe_process_inputs();
    rbe_process_game_update();
    rbe_render();

    // Get FPS
    static unsigned int countedFrames = 0;
    if (countedFrames == 0) {
        countedFrames = engineContext->targetFPS;
    } else {
        countedFrames++;
    }
    float averageFPS          = (float) countedFrames / ((float) SDL_GetTicks() / 1000.f);
    engineContext->averageFPS = averageFPS;
}

void rbe_process_inputs() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: engineContext->isRunning = false; break;
        default: break;
        }
        rbe_input_process(event);
    }
}

void rbe_process_game_update() {
    static const uint32_t MILLISECONDS_PER_TICK = 1000; // TODO: Put in another place
    static uint32_t lastFrameTime               = 0;
    const uint32_t targetFps                    = engineContext->targetFPS;
    const uint32_t FRAME_TARGET_TIME            = MILLISECONDS_PER_TICK / targetFps;
    const uint32_t timeToWait                   = FRAME_TARGET_TIME - (SDL_GetTicks() - lastFrameTime);
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        SDL_Delay(timeToWait);
    }

    // Variable Time Step
    const float variableDeltaTime = (float) (SDL_GetTicks() - lastFrameTime) / (float) MILLISECONDS_PER_TICK;
    rbe_ec_system_process_systems(variableDeltaTime);

    // Fixed Time Step
    static double fixedTime                = 0.0f;
    static const double PHYSICS_DELTA_TIME = 0.1f;
    static uint32_t fixedCurrentTime       = 0;
    static double accumulator              = 0.0f;
    uint32_t newTime                       = SDL_GetTicks();
    uint32_t frameTime                     = newTime - fixedCurrentTime;
    static const uint32_t MAX_FRAME_TIME   = 250;
    if (frameTime > MAX_FRAME_TIME) {
        frameTime = MAX_FRAME_TIME;
    }
    fixedCurrentTime = newTime;
    accumulator += (double) frameTime / 1000.0f;

    while (accumulator >= PHYSICS_DELTA_TIME) {
        fixedTime += PHYSICS_DELTA_TIME;
        accumulator -= PHYSICS_DELTA_TIME;
        rbe_ec_system_physics_process_systems((float) PHYSICS_DELTA_TIME);
        rbe_input_clean_up_flags();
    }

    rbe_input_clean_up_flags();
    lastFrameTime = SDL_GetTicks();
}

void rbe_render() {
    static Color backgroundColor = {33.0f / 255.0f, 33.0f / 255.0f, 33.0f / 255.0f, 1.0f};
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    rbe_ec_system_render_systems();

    rbe_renderer_flush_batches();

    SDL_GL_SwapWindow(window);
}

bool rbe_is_running() { return engineContext->isRunning; }

void rbe_shutdown() {
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(openGlContext);
    SDL_Quit();
    rbe_renderer_finalize();
    rbe_game_props_finalize();
    rbe_audio_manager_finalize();
    rbe_input_finalize();
    rbe_asset_manager_finalize();
    rbe_scene_manager_finalize();
    rbe_ecs_manager_finalize();
    rbe_py_finalize();
    rbe_logger_info("RBE Engine shutdown!");
}
