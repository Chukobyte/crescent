#include "core.h"

#include <time.h>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "../seika/src/seika.h"
#include "../seika/src/asset_manager.h"
#include "../seika/src/input/input.h"
#include "../seika/src/utils/logger.h"
#include "../seika/src/utils/se_file_system_utils.h"

#include "core_info.h"
#include "game_properties.h"
#include "engine_context.h"
#include "utils/command_line_args_util.h"
#include "scripting/python/rbe_py.h"
#include "ecs/ecs_manager.h"
#include "ecs/system/ec_system.h"
#include "scene/scene_manager.h"
#include "json/json_file_loader.h"

// The default project path if no directory override is provided
#define CRE_PROJECT_CONFIG_FILE_NAME "project.ccfg"

bool rbe_initialize_ecs();
bool rbe_load_assets_from_configuration();
void rbe_process_game_update();
void rbe_render();

static SDL_Window* window = NULL;
static SDL_GLContext openGlContext;
RBEGameProperties* gameProperties = NULL;
RBEEngineContext* engineContext = NULL;

bool rbe_initialize(int argv, char** args) {
    // Set random seed
    srand((int)time(NULL));

    se_logger_set_level(LogLevel_DEBUG);

    engineContext = rbe_engine_context_initialize();
    engineContext->engineRootDir = se_fs_get_cwd();

    // Handle command line flags
    CommandLineFlagResult commandLineFlagResult = rbe_command_line_args_parse(argv, args);
    if (strcmp(commandLineFlagResult.workingDirOverride, "") != 0) {
        se_logger_debug("Changing working directory from override to '%s'.", commandLineFlagResult.workingDirOverride);
        se_fs_chdir(commandLineFlagResult.workingDirOverride);
        se_fs_print_cwd();
    } else {
        se_logger_debug("No directory override given, starting default project at '%s'", DEFAULT_START_PROJECT_PATH);
        se_fs_chdir(DEFAULT_START_PROJECT_PATH);
        se_fs_print_cwd();
    }

    // Internal Assets Override
    if (strcmp(commandLineFlagResult.internalAssetsDirOverride, "") != 0) {
        engineContext->internalAssetsDir = strdup(commandLineFlagResult.internalAssetsDirOverride); // TODO: Clean up properly
    } else {
        engineContext->internalAssetsDir = strdup(engineContext->engineRootDir); // TODO: Clean up properly
    }

    rbe_py_initialize();

    gameProperties = cre_json_load_config_file(CRE_PROJECT_CONFIG_FILE_NAME);
    rbe_game_props_initialize(gameProperties);
    rbe_game_props_print();

    // Setup Game Controller DB Path
    char controllerMappingFilePath[256];
    strcpy(controllerMappingFilePath, engineContext->internalAssetsDir);
    strcat(controllerMappingFilePath, "/assets/resources/game_controller_db.txt");

    // Initialize seika framework
    sf_initialize(gameProperties->gameTitle,
                  gameProperties->windowWidth,
                  gameProperties->windowHeight,
                  gameProperties->resolutionWidth,
                  gameProperties->resolutionHeight,
                  controllerMappingFilePath);

    // Initialize sub systems
    if (!rbe_initialize_ecs()) {
        se_logger_error("Failed to initialize ecs!");
        return false;
    }

    rbe_scene_manager_initialize();

    rbe_load_assets_from_configuration();

    se_logger_info("Crescent Engine v%s initialized!", CRE_CORE_VERSION);
    engineContext->targetFPS = gameProperties->targetFPS;
    engineContext->isRunning = true;

    // Go to initial scene
    rbe_scene_manager_queue_scene_change(gameProperties->initialScenePath);

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
        se_asset_manager_load_audio_source_wav(assetAudioSource.file_path, assetAudioSource.file_path);
    }

    // Textures
    for (size_t i = 0; i < gameProperties->textureCount; i++) {
        const RBEAssetTexture assetTexture = gameProperties->textures[i];
        se_asset_manager_load_texture(assetTexture.file_path, assetTexture.file_path);
    }

    // Fonts
    for (size_t i = 0; i < gameProperties->fontCount; i++) {
        const RBEAssetFont assetFont = gameProperties->fonts[i];
        se_asset_manager_load_font(assetFont.file_path, assetFont.uid, assetFont.size);
    }

    // Inputs
    for (size_t i = 0; i < gameProperties->inputActionCount; i++) {
        const RBEInputAction inputAction = gameProperties->inputActions[i];
        for (size_t valueIndex = 0; valueIndex < inputAction.valueCount; valueIndex++) {
            const char* actionValue = inputAction.values[valueIndex];
            se_logger_debug("action_name = %s, action_value = %s", inputAction.name, actionValue);
            se_input_add_action_value(inputAction.name, actionValue, inputAction.deviceId);
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
    sf_process_inputs();
    rbe_process_game_update();
    rbe_render();

    // Get FPS
    static unsigned int countedFrames = 0;
    if (countedFrames == 0) {
        countedFrames = engineContext->targetFPS;
    } else {
        countedFrames++;
    }
    float averageFPS = (float) countedFrames / ((float) SDL_GetTicks() / 1000.f);
    engineContext->averageFPS = averageFPS;
}

void rbe_process_game_update() {
    static const uint32_t MILLISECONDS_PER_TICK = 1000; // TODO: Put in another place
    static uint32_t lastFrameTime = 0;
    const uint32_t targetFps = engineContext->targetFPS;
    const uint32_t FRAME_TARGET_TIME = MILLISECONDS_PER_TICK / targetFps;
    const uint32_t timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - lastFrameTime);
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        SDL_Delay(timeToWait);
    }

    // Variable Time Step
    const float variableDeltaTime = (float) (SDL_GetTicks() - lastFrameTime) / (float) MILLISECONDS_PER_TICK;
    rbe_ec_system_process_systems(variableDeltaTime);

    // Fixed Time Step
    static double fixedTime = 0.0f;
    static const double PHYSICS_DELTA_TIME = 0.1f;
    static uint32_t fixedCurrentTime = 0;
    static double accumulator = 0.0f;
    uint32_t newTime = SDL_GetTicks();
    uint32_t frameTime = newTime - fixedCurrentTime;
    static const uint32_t MAX_FRAME_TIME = 250;
    if (frameTime > MAX_FRAME_TIME) {
        frameTime = MAX_FRAME_TIME;
    }
    fixedCurrentTime = newTime;
    accumulator += (double) frameTime / 1000.0f;

    while (accumulator >= PHYSICS_DELTA_TIME) {
        fixedTime += PHYSICS_DELTA_TIME;
        accumulator -= PHYSICS_DELTA_TIME;
        rbe_ec_system_physics_process_systems((float) PHYSICS_DELTA_TIME);
        se_input_clean_up_flags();
    }

    se_input_clean_up_flags();
    lastFrameTime = SDL_GetTicks();
}

void rbe_render() {
    // Gather render data from ec systems
    rbe_ec_system_render_systems();
    // Actually render
    sf_render();
}

bool rbe_is_running() {
    return engineContext->isRunning && sf_is_running();
}

void rbe_shutdown() {
    SDL_DestroyWindow(window);
    SDL_GL_DeleteContext(openGlContext);
    SDL_Quit();
    sf_shutdown();
    rbe_game_props_finalize();
    rbe_scene_manager_finalize();
    rbe_ecs_manager_finalize();
    rbe_py_finalize();
    se_logger_info("RBE Engine shutdown!");
}
