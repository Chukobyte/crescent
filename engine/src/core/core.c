#include "core.h"

#include <time.h>
#include <string.h>

#include <seika/assert.h>
#include <seika/logger.h>
#include <seika/file_system.h>
#include <seika/string.h>
#include <seika/time.h>
#include <seika/audio/audio.h>
#include <seika/ecs/ec_system.h>
#include <seika/asset/asset_file_loader.h>
#include <seika/asset/asset_manager.h>
#include <seika/input/input.h>
#include <seika/rendering/window.h>

#include "core_info.h"
#include "game_properties.h"
#include "engine_context.h"
#include "embedded_assets.h"
#include "world.h"
#include "utils/command_line_args_util.h"
#include "ecs/ecs_manager.h"
#include "scene/scene_manager.h"
#include "json/json_file_loader.h"
#include "math/curve_float_manager.h"
#include "seika/input/sdl_input.h"
#include "seika/rendering/renderer.h"

// The default project path if no directory override is provided
#define CRE_PROJECT_CONFIG_FILE_NAME "project.ccfg"

static bool cre_initialize_ecs();
static bool cre_load_built_in_assets();
static bool cre_load_assets_from_configuration();
static void cre_process_game_update();
static void cre_render();

CREGameProperties* gameProperties = NULL;
CREEngineContext* engineContext = NULL;

bool cre_initialize(int32 argv, char** args) {
    // Set random seed
    srand((int32)time(NULL));

    ska_logger_set_level(SkaLogLevel_ERROR);

    engineContext = cre_engine_context_initialize();
    engineContext->engineRootDir = ska_fs_get_cwd();

    // Handle command line flags
    CommandLineFlagResult commandLineFlagResult = cre_command_line_args_parse(argv, args);
    // log level
    if (strcmp(commandLineFlagResult.logLevel, "") != 0) {
        ska_logger_set_level(ska_logger_get_log_level_enum(commandLineFlagResult.logLevel));
        ska_logger_debug("Log level override set to '%s'", commandLineFlagResult.logLevel);
    }
    // working dir override
    if (strcmp(commandLineFlagResult.workingDirOverride, "") != 0) {
        ska_logger_debug("Changing working directory from override to '%s'.", commandLineFlagResult.workingDirOverride);
        ska_fs_chdir(commandLineFlagResult.workingDirOverride);
        ska_fs_print_cwd();
    }
    // Check if default project is present, if so change to the directory.  Will probably want to change at some point...
    else if (ska_fs_does_dir_exist(DEFAULT_START_PROJECT_PATH)) {
        ska_logger_debug("No directory override given and default project found.  Starting default project at '%s'", DEFAULT_START_PROJECT_PATH);
        ska_fs_chdir(DEFAULT_START_PROJECT_PATH);
        ska_fs_print_cwd();
    }
    // Load project archive if it exists
#ifdef _WIN32
    engineContext->projectArchivePath = ska_str_trim_and_replace(args[0], '.', ".pck");
#else
    char packArchivePath[1024];
    snprintf(packArchivePath, sizeof(packArchivePath), "%s.pck", args[0]);
    engineContext->projectArchivePath = ska_strdup(packArchivePath);
#endif
    if (ska_asset_file_loader_load_archive(engineContext->projectArchivePath)) {
        ska_logger_debug("Setting asset read mode to 'archive', found pck file at '%s'", engineContext->projectArchivePath);
        ska_asset_file_loader_set_read_mode(SkaAssetFileLoaderReadMode_ARCHIVE);
    } else {
        ska_logger_debug("Not able to find .pck file at '%s', setting asset read mode to 'disk'", engineContext->projectArchivePath);
        ska_asset_file_loader_set_read_mode(SkaAssetFileLoaderReadMode_DISK);
    }
    // Internal Assets Override
    if (strcmp(commandLineFlagResult.internalAssetsDirOverride, "") != 0) {
        engineContext->internalAssetsDir = ska_strdup(commandLineFlagResult.internalAssetsDirOverride); // TODO: Clean up properly
    } else {
        engineContext->internalAssetsDir = ska_strdup(engineContext->engineRootDir); // TODO: Clean up properly
    }

    cre_curve_float_manager_init();

    gameProperties = cre_json_load_config_file(CRE_PROJECT_CONFIG_FILE_NAME);
    cre_game_props_initialize(gameProperties);
    cre_game_props_print();

    // Initialize seika framework
    ska_asset_manager_initialize();
    bool hasInitialized = ska_window_initialize((SkaWindowProperties){
        .title = gameProperties->gameTitle,
        .windowWidth = gameProperties->windowWidth,
        .windowHeight = gameProperties->windowHeight,
        .resolutionWidth = gameProperties->resolutionWidth,
        .resolutionHeight = gameProperties->resolutionHeight,
        .maintainAspectRatio = gameProperties->maintainAspectRatio,
    });
    SKA_ASSERT(hasInitialized);
    hasInitialized = ska_input_initialize();
    SKA_ASSERT(hasInitialized);
    hasInitialized = ska_audio_initialize();
    SKA_ASSERT(hasInitialized);
    // const bool hasSeikaInitialized = ska_init_all2(
    //                                      gameProperties->gameTitle,
    //                                      gameProperties->windowWidth,
    //                                      gameProperties->windowHeight,
    //                                      gameProperties->resolutionWidth,
    //                                      gameProperties->resolutionHeight,
    //                                      gameProperties->audioWavSampleRate,
    //                                      gameProperties->maintainAspectRatio);
    // if (!hasSeikaInitialized) {
    //     ska_logger_error("Failed to initialize seika framework!");
    //     return false;
    // }

    ska_window_set_vsync(gameProperties->vsyncEnabled);

    // Initialize sub systems
    if (!cre_initialize_ecs()) {
        ska_logger_error("Failed to initialize ecs!");
        return false;
    }

    cre_scene_manager_initialize();

    cre_load_built_in_assets();
    cre_load_assets_from_configuration();

    ska_logger_info("Crescent Engine v%s initialized!", CRE_CORE_VERSION);
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

bool cre_load_built_in_assets() {
    // Load default font
    ska_asset_manager_load_font_from_memory(CRE_DEFAULT_FONT_KEY, CRE_EMBEDDED_ASSET_FONT_VERDANA_TTF_HEX, CRE_EMBEDDED_ASSET_FONT_VERDANA_TTF_SIZE, CRE_DEFAULT_FONT_ASSET.size, CRE_DEFAULT_FONT_ASSET.applyNearestNeighbor);

    return true;
}

bool cre_load_assets_from_configuration() {
    // Audio Sources
    for (size_t i = 0; i < gameProperties->audioSourceCount; i++) {
        const CREAssetAudioSource assetAudioSource = gameProperties->audioSources[i];
        ska_asset_manager_load_audio_source_wav(assetAudioSource.file_path, assetAudioSource.file_path);
    }

    // Textures
    for (size_t i = 0; i < gameProperties->textureCount; i++) {
        const CREAssetTexture assetTexture = gameProperties->textures[i];
        ska_asset_manager_load_texture_ex(assetTexture.file_path, assetTexture.file_path, assetTexture.wrap_s, assetTexture.wrap_t, assetTexture.applyNearestNeighbor);
    }

    // Fonts
    for (size_t i = 0; i < gameProperties->fontCount; i++) {
        const CREAssetFont assetFont = gameProperties->fonts[i];
        ska_asset_manager_load_font(assetFont.file_path, assetFont.uid, assetFont.size, assetFont.applyNearestNeighbor);
    }

    // Inputs
    SkaInputActionValue actionValues[SKA_INPUT_MAX_INPUT_ACTION_VALUES] = {0};
    for (size_t i = 0; i < gameProperties->inputActionCount; i++) {
        const CREInputAction inputAction = gameProperties->inputActions[i];
        if (inputAction.valueCount == 0) {
            continue;
        }
        for (size_t valueIndex = 0; valueIndex < inputAction.valueCount; valueIndex++) {
            const char* actionValue = inputAction.values[valueIndex];
            const SkaInputKey actionValueKey = ska_input_string_to_key(actionValue);
            SKA_ASSERT(actionValueKey);
            actionValues[valueIndex] = (SkaInputActionValue){ .key = actionValueKey, .strengthThreshold = 0.5f };
            ska_logger_debug("action_name = %s, action_value = %s", inputAction.name, actionValue);
        }
        actionValues[inputAction.valueCount] = (SkaInputActionValue){ SkaInputKey_INVALID };
        ska_input_add_input_action(inputAction.name, actionValues, inputAction.deviceId);
    }

    return true;
}

void cre_update() {
    const uint32_t startFrameTime = ska_get_ticks();

    // Process Scene change if exists
    cre_scene_manager_process_queued_scene_change();

    // Clear out queued nodes for deletion
    cre_scene_manager_process_queued_deletion_entities();

    // Create nodes queued for creation a.k.a. '_start()'
    cre_scene_manager_process_queued_creation_entities();

    // Main loop
    // ska_update();
    cre_process_game_update();
    cre_render();

    const uint32_t endFrameTime = ska_get_ticks();

    // Update FPS
    cre_engine_context_update_stats(endFrameTime - startFrameTime);
}

void cre_process_game_update() {
    ska_input_new_frame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_EVENT_QUIT: {
                engineContext->isRunning = false;
                break;
            }
            case SDL_EVENT_WINDOW_RESIZED: {
                const Sint32 windowWidth = event.window.data1;
                const Sint32 windowHeight = event.window.data2;
                ska_renderer_update_window_size(windowWidth, windowHeight);
                break;
            }
            default: {
                ska_sdl_process_event(event);
                break;
            }
        }
    }
    ska_sdl_process_axis_events();

    ska_ecs_system_event_pre_update_all_systems();

    static const uint32 MILLISECONDS_PER_TICK = 1000; // TODO: Put in another place
    static uint32 lastFrameTime = 0;
    const uint32 targetFps = engineContext->targetFPS;
    const uint32 FRAME_TARGET_TIME = MILLISECONDS_PER_TICK / targetFps;
    const uint32 timeToWait = FRAME_TARGET_TIME - (ska_get_ticks() - lastFrameTime);
    if (timeToWait > 0 && timeToWait <= FRAME_TARGET_TIME) {
        ska_delay(timeToWait);
    }

    // Variable Time Step
    const f32 variableDeltaTime = (f32) (ska_get_ticks() - lastFrameTime) / (f32) MILLISECONDS_PER_TICK;
    cre_world_set_frame_delta_time(variableDeltaTime);
    ska_ecs_system_event_update_systems(variableDeltaTime);

    // Fixed Time Step
    static uint32 fixedCurrentTime = 0;
    static float accumulator = 0.0f;
    uint32 newTime = ska_get_ticks();
    uint32 frameTime = newTime - fixedCurrentTime;
    static const uint32 MAX_FRAME_TIME = 250;
    if (frameTime > MAX_FRAME_TIME) {
        frameTime = MAX_FRAME_TIME;
    }
    fixedCurrentTime = newTime;
    accumulator += (f32)frameTime / 1000.0f;

    while (accumulator >= CRE_GLOBAL_PHYSICS_DELTA_TIME) {
        accumulator -= CRE_GLOBAL_PHYSICS_DELTA_TIME;
        // ska_fixed_update(CRE_GLOBAL_PHYSICS_DELTA_TIME);

        static f32 globalTime = 0.0f;
        globalTime += CRE_GLOBAL_PHYSICS_DELTA_TIME;
        ska_renderer_set_global_shader_param_time(globalTime);

        ska_ecs_system_event_fixed_update_systems(CRE_GLOBAL_PHYSICS_DELTA_TIME);
        ska_input_new_frame();
    }

    ska_ecs_system_event_post_update_all_systems();

    lastFrameTime = ska_get_ticks();
}

void cre_render() {
    // Gather render data from ec systems
    ska_ecs_system_event_render_systems();
    // Actually render
    ska_window_render(&gameProperties->windowBackgroundColor);
}

bool cre_is_running() {
    // return engineContext->isRunning && ska_is_running();
    return engineContext->isRunning;
}

int32 cre_shutdown() {
    // ska_shutdown_all();
    ska_window_finalize();
    ska_input_finalize();
    ska_audio_finalize();
    ska_asset_manager_finalize();

    cre_game_props_finalize();
    cre_scene_manager_finalize();
    cre_ecs_manager_finalize();
    cre_curve_float_manager_finalize();
    const int finalExitCode = engineContext->exitCode;
    engineContext = NULL;
    cre_engine_context_finalize();
    ska_logger_info("Crescent Engine shutdown!");
    return finalExitCode;
}
