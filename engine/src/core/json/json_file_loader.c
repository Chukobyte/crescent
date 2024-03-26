#include "json_file_loader.h"

#include <seika/string.h>
#include <seika/logger.h>
#include <seika/memory.h>
#include <seika/assert.h>
#include <seika/asset/asset_file_loader.h>
#include <seika/audio/audio.h>

#include "json_helper.h"
#include "../game_properties.h"

//--- Project Configuration Files ---//
void cre_json_configure_assets(cJSON* configJson, CREGameProperties* properties) {
    cJSON* assetsJson = cJSON_GetObjectItemCaseSensitive(configJson, "assets");
    SKA_ASSERT_FMT(assetsJson != NULL, "Assets json object is NULL!");
    // Audio Sources
    ska_logger_debug("Loading audio sources...");
    cJSON* audioSourcesJsonArray = cJSON_GetObjectItemCaseSensitive(assetsJson, "audio_sources");
    cJSON* audioSourceJson = NULL;
    cJSON_ArrayForEach(audioSourceJson, audioSourcesJsonArray) {
        SKA_ASSERT_FMT(properties->audioSourceCount < CRE_PROPERTIES_ASSET_LIMIT, "Audio source count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        properties->audioSources[properties->audioSourceCount].file_path = json_get_string_new(audioSourceJson, "file_path");
        ska_logger_debug("Audio Source File Path: '%s'",
                        properties->audioSources[properties->audioSourceCount].file_path);
        properties->audioSourceCount++;
    }
    // Textures
    ska_logger_debug("Loading textures...");
    cJSON* texturesJsonArray = cJSON_GetObjectItemCaseSensitive(assetsJson, "textures");
    cJSON* textureJson = NULL;
    cJSON_ArrayForEach(textureJson, texturesJsonArray) {
        SKA_ASSERT_FMT(properties->textureCount < CRE_PROPERTIES_ASSET_LIMIT, "Texture count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        properties->textures[properties->textureCount].file_path = json_get_string_new(textureJson, "file_path");
        ska_logger_debug("Texture File Path: '%s'", properties->textures[properties->textureCount].file_path);
        properties->textures[properties->textureCount].wrap_s = json_get_string_default_new(textureJson, "wrap_s", "clamp_to_border");
        ska_logger_debug("Texture Wrap S: '%s'", properties->textures[properties->textureCount].wrap_s);
        properties->textures[properties->textureCount].wrap_t = json_get_string_default_new(textureJson, "wrap_t", "clamp_to_border");
        ska_logger_debug("Texture Wrap T: '%s'", properties->textures[properties->textureCount].wrap_t);
        properties->textures[properties->textureCount].applyNearestNeighbor = json_get_bool_default(textureJson, "nearest_neighbor", true);
        ska_logger_debug("Texture Nearest Neighbor: '%s'", ska_bool_to_string(properties->textures[properties->textureCount].applyNearestNeighbor));
        properties->textureCount++;
    }
    // Fonts
    ska_logger_debug("Loading fonts...");
    cJSON* fontsJsonArray = cJSON_GetObjectItemCaseSensitive(assetsJson, "fonts");
    cJSON* fontJson = NULL;
    cJSON_ArrayForEach(fontJson, fontsJsonArray) {
        SKA_ASSERT_FMT(properties->fontCount < CRE_PROPERTIES_ASSET_LIMIT, "Font count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        properties->fonts[properties->fontCount].file_path = json_get_string_new(fontJson, "file_path");
        ska_logger_debug("Font File Path: '%s'", properties->fonts[properties->fontCount].file_path);
        properties->fonts[properties->fontCount].uid = json_get_string_new(fontJson, "uid");
        ska_logger_debug("Font UID: '%s'", properties->fonts[properties->fontCount].uid);
        properties->fonts[properties->fontCount].size = json_get_int(fontJson, "size");
        ska_logger_debug("Font Size: '%d'", properties->fonts[properties->fontCount].size);
        properties->fonts[properties->fontCount].applyNearestNeighbor = json_get_bool_default(fontJson, "nearest_neighbor", true);
        ska_logger_debug("Font Nearest Neighbor: '%s'", ska_bool_to_string(properties->fonts[properties->fontCount].applyNearestNeighbor));
        properties->fontCount++;
    }
}

void cre_json_configure_inputs(cJSON* configJson, CREGameProperties* properties) {
    ska_logger_debug("Loading inputs...");
    cJSON* inputsJsonArray = cJSON_GetObjectItemCaseSensitive(configJson, "inputs");
    cJSON* inputJson = NULL;
    cJSON_ArrayForEach(inputJson, inputsJsonArray) {
        SKA_ASSERT_FMT(properties->inputActionCount < CRE_PROPERTIES_ASSET_LIMIT, "Input count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        CREInputAction* inputAction = &properties->inputActions[properties->inputActionCount];
        inputAction->name = json_get_string_new(inputJson, "name");
        inputAction->deviceId = json_get_int_default(inputJson, "device_id", 0);
        ska_logger_debug("Input Action -  name: '%s', device_id: '%d'", inputAction->name, inputAction->deviceId);
        cJSON* valuesJsonArray = cJSON_GetObjectItemCaseSensitive(inputJson, "values");
        cJSON* valueJson = NULL;
        cJSON_ArrayForEach(valueJson, valuesJsonArray) {
            SKA_ASSERT_FMT(inputAction->valueCount < CRE_INPUT_VALUES_LIMIT, "Reading input action '%s' value that is over the max limit of %d, increase the limit of CRE_INPUT_VALUES_LIMIT", inputAction->name, CRE_INPUT_VALUES_LIMIT);
            const char* value = cJSON_GetStringValue(valueJson);
            inputAction->values[inputAction->valueCount] = ska_strdup(value);
            ska_logger_debug("Input Action Value: '%s'", inputAction->values[inputAction->valueCount]);
            inputAction->valueCount++;
        }
        properties->inputActionCount++;
    }
}

CREGameProperties* cre_json_load_config_file(const char* filePath) {
    CREGameProperties* properties = cre_game_props_create();
    char* fileContent = ska_asset_file_loader_read_file_contents_as_string(filePath, NULL);

    ska_logger_debug("Loading game properties...");

    cJSON* configJson = cJSON_Parse(fileContent);
    if (configJson != NULL) {
        // Name
        properties->gameTitle = json_get_string_new(configJson, "name");
        ska_logger_debug("Game Title: '%s'", properties->gameTitle);
        // Window Width
        properties->windowWidth = json_get_int(configJson, "window_width");
        ska_logger_debug("Window Width: '%d'", properties->windowWidth);
        // Window Height
        properties->windowHeight = json_get_int(configJson, "window_height");
        ska_logger_debug("Window Height '%d'", properties->windowHeight);
        // Resolution Width
        properties->resolutionWidth = json_get_int(configJson, "resolution_width");
        ska_logger_debug("Resolution Width: '%d'", properties->resolutionWidth);
        // Resolution Height
        properties->resolutionHeight = json_get_int(configJson, "resolution_height");
        ska_logger_debug("Resolution Height: '%d'", properties->resolutionHeight);
        // Maintain Aspect Ratio
        properties->maintainAspectRatio = json_get_bool_default(configJson, "maintain_aspect_ratio", false);
        ska_logger_debug("Maintain Aspect Ratio '%s'", properties->maintainAspectRatio == true ? "true" : "false");
        // Wav Sample Rate
        properties->audioWavSampleRate = json_get_int_default(configJson, "audio_wav_sample_rate", SKA_AUDIO_SOURCE_DEFAULT_WAV_SAMPLE_RATE);
        ska_logger_debug("Wav Sample Rate '%u'", properties->audioWavSampleRate);
        // Target FPS
        properties->targetFPS = json_get_int(configJson, "target_fps");
        ska_logger_debug("Target FPS '%d'", properties->targetFPS);
        // Initial Node Path
        properties->initialScenePath = json_get_string_new(configJson, "initial_node_path");
        ska_logger_debug("Printing json initial node path '%s'", properties->initialScenePath);
        // Colliders Visible
        properties->areCollidersVisible = json_get_bool(configJson, "colliders_visible");
        ska_logger_debug("Colliders Visible '%s'", properties->areCollidersVisible == true ? "true" : "false");
        // VSync Enabled
        properties->vsyncEnabled = json_get_bool_default(configJson, "vsync_enabled", properties->vsyncEnabled);
        ska_logger_debug("VSync Enabled '%s'", properties->vsyncEnabled == true ? "true" : "false");

        cre_json_configure_assets(configJson, properties);

        cre_json_configure_inputs(configJson, properties);
    }


    cJSON_Delete(configJson);
    SKA_MEM_FREE(fileContent);

    return properties;
}

//--- Curve Float ---//
SkaCurveFloat cre_json_load_curve_float_file(const char* filePath, bool *isSuccessful) {
    SkaCurveFloat newCurve = { .controlPointCount = 0 };
    char* fileContent = ska_asset_file_loader_read_file_contents_as_string(filePath, NULL);
    ska_logger_debug("Loading curve file from path '%s'", filePath);
    cJSON* curveJson = cJSON_Parse(fileContent);
    SKA_MEM_FREE(fileContent);

    // Make sure curve file is valid json
    if (curveJson == NULL) {
        if (isSuccessful != NULL) {
            *isSuccessful = false;
        }
        return newCurve;
    }

    // Test if valid asset type
    char* assetType = json_get_string_new(curveJson, "type");
    if (strcmp(assetType, "curve") != 0) {
        if (isSuccessful != NULL) {
            *isSuccessful = false;
        }
        return newCurve;
    }
    SKA_MEM_FREE(assetType);

    // Now we know it's a valid file, read in the control points
    SkaCurveControlPoint controlPoints[SKA_CURVE_MAX_CONTROL_POINTS];
    size_t controlPointCount = 0;
    cJSON* controlPointsJsonArray = cJSON_GetObjectItemCaseSensitive(curveJson, "control_points");
    SKA_ASSERT_FMT(controlPointsJsonArray != NULL, "Failed to read in control points json!");
    cJSON* pointJson = NULL;
    cJSON_ArrayForEach(pointJson, controlPointsJsonArray) {
        const double x = json_get_double(pointJson, "x");
        const double y = json_get_double(pointJson, "y");
        const double tangentIn = json_get_double(pointJson, "tangent_in");
        const double tangentOut = json_get_double(pointJson, "tangent_out");
        controlPoints[controlPointCount++] = (SkaCurveControlPoint) {
            .x = x, .y = y, .tangentIn = tangentIn, .tangentOut = tangentOut
        };
    }
    ska_curve_float_add_control_points(&newCurve, controlPoints, controlPointCount);

    if (isSuccessful != NULL) {
        *isSuccessful = true;
    }
    return newCurve;
}
