#include "core.h"

#include <time.h>

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "../seika/src/seika.h"
#include "../seika/src/asset/asset_file_loader.h"
#include "../seika/src/asset/asset_manager.h"
#include "../seika/src/input/input.h"
#include "../seika/src/memory/se_mem.h"
#include "../seika/src/utils/logger.h"
#include "../seika/src/utils/se_file_system_utils.h"
#include "../seika/src/utils/se_string_util.h"
#include "../seika/src/utils/se_assert.h"

#include "core_info.h"
#include "game_properties.h"
#include "engine_context.h"
#include "utils/command_line_args_util.h"
#include "scripting/python/cre_py.h"
#include "ecs/ecs_manager.h"
#include "ecs/system/ec_system.h"
#include "scene/scene_manager.h"
#include "json/json_file_loader.h"
#include "math/curve_float_manager.h"

// The default project path if no directory override is provided
#define CRE_PROJECT_CONFIG_FILE_NAME "project.ccfg"

bool cre_initialize_ecs();
bool cre_load_assets_from_configuration();
void cre_process_game_update();
void cre_render();

CREGameProperties* gameProperties = NULL;
CREEngineContext* engineContext = NULL;

bool cre_initialize(int argv, char** args) {
    // Set random seed
    srand((int)time(NULL));

    se_logger_set_level(LogLevel_DEBUG);

    engineContext = cre_engine_context_initialize();
    engineContext->engineRootDir = se_fs_get_cwd();

    // Handle command line flags
    CommandLineFlagResult commandLineFlagResult = cre_command_line_args_parse(argv, args);
    // log level
    if (strcmp(commandLineFlagResult.logLevel, "") != 0) {
        se_logger_set_level(se_logger_get_log_level_enum(commandLineFlagResult.logLevel));
        se_logger_debug("Log level override set to '%s'", commandLineFlagResult.logLevel);
    }
    // working dir override
    if (strcmp(commandLineFlagResult.workingDirOverride, "") != 0) {
        se_logger_debug("Changing working directory from override to '%s'.", commandLineFlagResult.workingDirOverride);
        se_fs_chdir(commandLineFlagResult.workingDirOverride);
        se_fs_print_cwd();
    }
    // Check if default project is present, if so change to the directory.  Will probably want to change at some point...
    else if (se_fs_does_dir_exist(DEFAULT_START_PROJECT_PATH)) {
        se_logger_debug("No directory override given and default project found.  Starting default project at '%s'", DEFAULT_START_PROJECT_PATH);
        se_fs_chdir(DEFAULT_START_PROJECT_PATH);
        se_fs_print_cwd();
    }
    // Load project archive if it exists
#ifdef _WIN32
    engineContext->projectArchivePath = se_str_trim_and_replace(args[0], '.', ".pck");
#else
    char packArchivePath[1024];
    snprintf(packArchivePath, sizeof(packArchivePath), "%s.pck", args[0]);
    engineContext->projectArchivePath = se_strdup(packArchivePath);
#endif
    if (sf_asset_file_loader_load_archive(engineContext->projectArchivePath)) {
        se_logger_debug("Setting asset read mode to 'archive', found pck file at '%s'", engineContext->projectArchivePath);
        sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode_ARCHIVE);
    } else {
        se_logger_debug("Not able to find .pck file at '%s', setting asset read mode to 'disk'", engineContext->projectArchivePath);
        sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode_DISK);
    }
    // Internal Assets Override
    if (strcmp(commandLineFlagResult.internalAssetsDirOverride, "") != 0) {
        engineContext->internalAssetsDir = se_strdup(commandLineFlagResult.internalAssetsDirOverride); // TODO: Clean up properly
    } else {
        engineContext->internalAssetsDir = se_strdup(engineContext->engineRootDir); // TODO: Clean up properly
    }

    // TODO: Determine if python needs to be initialized programmatically
    cre_py_initialize(engineContext->internalAssetsDir);

    cre_curve_float_manager_init();

    gameProperties = cre_json_load_config_file(CRE_PROJECT_CONFIG_FILE_NAME);
    cre_game_props_initialize(gameProperties);
    cre_game_props_print();

    // Setup Game Controller DB Path
    static const char* gameControllerDBFilePath = "assets/resources/game_controller_db.txt";
    char controllerMappingFilePath[256];
    if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_ARCHIVE) {
        strcpy(controllerMappingFilePath, gameControllerDBFilePath);
    } else { // Can only be disk
        strcpy(controllerMappingFilePath, engineContext->internalAssetsDir);
        strcat(controllerMappingFilePath, "/");
        strcat(controllerMappingFilePath, gameControllerDBFilePath);
    }

    // Initialize seika framework
    const bool hasSeikaInitialized = sf_initialize(
                                         gameProperties->gameTitle,
                                         gameProperties->windowWidth,
                                         gameProperties->windowHeight,
                                         gameProperties->resolutionWidth,
                                         gameProperties->resolutionHeight,
                                         controllerMappingFilePath
                                     );
    if (!hasSeikaInitialized) {
        se_logger_error("Failed to initialize seika framework!");
        return false;
    }

    // Initialize sub systems
    if (!cre_initialize_ecs()) {
        se_logger_error("Failed to initialize ecs!");
        return false;
    }

    cre_scene_manager_initialize();

    cre_load_assets_from_configuration();

    se_logger_info("Crescent Engine v%s initialized!", CRE_CORE_VERSION);
    engineContext->targetFPS = gameProperties->targetFPS;
    engineContext->isRunning = true;

    // Go to initial scene
    cre_scene_manager_queue_scene_change(gameProperties->initialScenePath);

    return true;
}

bool cre_initialize_ecs() {
    cre_ecs_manager_initialize();
    return true;
}

bool cre_load_assets_from_configuration() {
    // Audio Sources
    for (size_t i = 0; i < gameProperties->audioSourceCount; i++) {
        const CREAssetAudioSource assetAudioSource = gameProperties->audioSources[i];
        se_asset_manager_load_audio_source_wav(assetAudioSource.file_path, assetAudioSource.file_path);
    }

    // Textures
    for (size_t i = 0; i < gameProperties->textureCount; i++) {
        const CREAssetTexture assetTexture = gameProperties->textures[i];
        se_asset_manager_load_texture_ex(assetTexture.file_path, assetTexture.file_path, assetTexture.wrap_s, assetTexture.wrap_t, assetTexture.filter_min, assetTexture.filter_mag);
    }

    // Fonts
    for (size_t i = 0; i < gameProperties->fontCount; i++) {
        const CREAssetFont assetFont = gameProperties->fonts[i];
        se_asset_manager_load_font(assetFont.file_path, assetFont.uid, assetFont.size);
    }

    // Inputs
    for (size_t i = 0; i < gameProperties->inputActionCount; i++) {
        const CREInputAction inputAction = gameProperties->inputActions[i];
        for (size_t valueIndex = 0; valueIndex < inputAction.valueCount; valueIndex++) {
            const char* actionValue = inputAction.values[valueIndex];
            se_logger_debug("action_name = %s, action_value = %s", inputAction.name, actionValue);
            se_input_add_action_value(inputAction.name, actionValue, inputAction.deviceId);
        }
    }

    return true;
}

void cre_update() {
    // Process Scene change if exists
    cre_scene_manager_process_queued_scene_change();

    // Clear out queued nodes for deletion
    cre_scene_manager_process_queued_deletion_entities();

    // Create nodes queued for creation a.k.a. '_start()'
    cre_scene_manager_process_queued_creation_entities();

    // Main loop
    sf_process_inputs();
    cre_process_game_update();
    cre_render();

    // Update FPS
    cre_engine_context_update_stats();
}

void cre_process_game_update() {
    cre_ec_system_pre_process_all_systems();

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
    cre_ec_system_process_systems(variableDeltaTime);

    // Fixed Time Step
    static uint32_t fixedCurrentTime = 0;
    static float accumulator = 0.0f;
    uint32_t newTime = SDL_GetTicks();
    uint32_t frameTime = newTime - fixedCurrentTime;
    static const uint32_t MAX_FRAME_TIME = 250;
    if (frameTime > MAX_FRAME_TIME) {
        frameTime = MAX_FRAME_TIME;
    }
    fixedCurrentTime = newTime;
    accumulator += (float) frameTime / 1000.0f;

    while (accumulator >= CRE_GLOBAL_PHYSICS_DELTA_TIME) {
        accumulator -= CRE_GLOBAL_PHYSICS_DELTA_TIME;
        cre_ec_system_physics_process_systems(CRE_GLOBAL_PHYSICS_DELTA_TIME);
        se_input_clean_up_flags();
    }

    se_input_clean_up_flags();
    lastFrameTime = SDL_GetTicks();

    cre_ec_system_post_process_all_systems();
}

void cre_render() {
    // Gather render data from ec systems
    cre_ec_system_render_systems();
    // Actually render
    sf_render();
}

bool cre_is_running() {
    return engineContext->isRunning && sf_is_running();
}

void cre_shutdown() {
    sf_shutdown();
    cre_game_props_finalize();
    cre_scene_manager_finalize();
    cre_ecs_manager_finalize();
    cre_py_finalize();
    cre_curve_float_manager_finalize();
    SDL_Quit();
    se_logger_info("RBE Engine shutdown!");
}
