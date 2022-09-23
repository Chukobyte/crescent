#include "json_file_loader.h"

#include <string.h>

#include <cjson/cJSON.h>

#include "../game_properties.h"
#include "../utils/rbe_file_system_utils.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"
#include "../memory/rbe_mem.h"

// Utility Functions
const char* json_get_string(cJSON* json, const char* key) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    RBE_ASSERT_FMT(cJSON_IsString(stringJson) && (stringJson->valuestring != NULL), "Not able to load json string value for key '%s'", key);
    return stringJson->valuestring;
}

const char* json_get_string_default(cJSON* json, const char* key, const char* defaultValue) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(stringJson) && (stringJson->valuestring != NULL)) {
        return stringJson->valuestring;
    }
    return defaultValue;
}

char* json_get_string_new(cJSON* json, const char* key) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    RBE_ASSERT_FMT(cJSON_IsString(stringJson) && (stringJson->valuestring != NULL), "Not able to load json string value for key '%s'", key);
    return strdup(stringJson->valuestring);
}

char* json_get_string_default_new(cJSON* json, const char* key, const char* defaultValue) {
    cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsString(stringJson) && (stringJson->valuestring != NULL)) {
        return strdup(stringJson->valuestring);
    }
    return strdup(defaultValue);
}

int json_get_int(cJSON* json, const char* key) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    RBE_ASSERT_FMT(cJSON_IsNumber(numJson), "Not able to load json int value for key '%s'", key);
    return (int) numJson->valuedouble;
}

int json_get_int_default(cJSON* json, const char* key, int defaultValue) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(numJson)) {
        return (int) numJson->valuedouble;
    }
    return defaultValue;
}

double json_get_double(cJSON* json, const char* key) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    RBE_ASSERT_FMT(cJSON_IsNumber(numJson), "Not able to load json int value for key '%s'", key);
    return numJson->valuedouble;
}

double json_get_double_default(cJSON* json, const char* key, double defaultValue) {
    cJSON* numJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsNumber(numJson)) {
        return numJson->valuedouble;
    }
    return defaultValue;
}

bool json_get_bool(cJSON* json, const char* key) {
    cJSON* boolJson = cJSON_GetObjectItemCaseSensitive(json, key);
    RBE_ASSERT_FMT(cJSON_IsBool(boolJson), "Not able to load json bool value for key '%s'", key);
    return cJSON_IsTrue(boolJson) ? true : false;
}

bool json_get_bool_default(cJSON* json, const char* key, bool defaultValue) {
    cJSON* boolJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (cJSON_IsBool(boolJson)) {
        return cJSON_IsTrue(boolJson) ? true : false;
    }
    return defaultValue;
}

// Main
void cre_json_configure_assets(cJSON* configJson, RBEGameProperties* properties) {
    cJSON* assetsJson = cJSON_GetObjectItemCaseSensitive(configJson, "assets");
    RBE_ASSERT_FMT(assetsJson != NULL, "Assets json object is NULL!");
    // Audio Sources
    rbe_logger_debug("Loading audio sources...");
    cJSON* audioSourcesJsonArray = cJSON_GetObjectItemCaseSensitive(assetsJson, "audio_sources");
    cJSON* audioSourceJson = NULL;
    cJSON_ArrayForEach(audioSourceJson, audioSourcesJsonArray) {
        RBE_ASSERT_FMT(properties->audioSourceCount < CRE_PROPERTIES_ASSET_LIMIT, "Audio source count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        properties->audioSources[properties->audioSourceCount].file_path = json_get_string_new(audioSourceJson, "file_path");
        rbe_logger_debug("Audio Source File Path: '%s'", properties->audioSources[properties->audioSourceCount].file_path);
        properties->audioSourceCount++;
    }
    // Textures
    rbe_logger_debug("Loading textures...");
    cJSON* texturesJsonArray = cJSON_GetObjectItemCaseSensitive(assetsJson, "textures");
    cJSON* textureJson = NULL;
    cJSON_ArrayForEach(textureJson, texturesJsonArray) {
        RBE_ASSERT_FMT(properties->textureCount < CRE_PROPERTIES_ASSET_LIMIT, "Audio source count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        properties->textures[properties->textureCount].file_path = json_get_string_new(textureJson, "file_path");
        rbe_logger_debug("Texture File Path: '%s'", properties->textures[properties->textureCount].file_path);
        properties->textures[properties->textureCount].wrap_s = json_get_string_default_new(textureJson, "wrap_s", "clamp_to_border");
        rbe_logger_debug("Texture Wrap S: '%s'", properties->textures[properties->textureCount].wrap_s);
        properties->textures[properties->textureCount].wrap_t = json_get_string_default_new(textureJson, "wrap_t", "clamp_to_border");
        rbe_logger_debug("Texture Wrap T: '%s'", properties->textures[properties->textureCount].wrap_t);
        properties->textures[properties->textureCount].filter_min = json_get_string_default_new(textureJson, "filter_min", "nearest");
        rbe_logger_debug("Texture Filter Min: '%s'", properties->textures[properties->textureCount].filter_min);
        properties->textures[properties->textureCount].filter_mag = json_get_string_default_new(textureJson, "filter_mag", "nearest");
        rbe_logger_debug("Texture Filter Mag: '%s'", properties->textures[properties->textureCount].filter_mag);
        properties->textureCount++;
    }
    // Fonts
    rbe_logger_debug("Loading fonts...");
    cJSON* fontsJsonArray = cJSON_GetObjectItemCaseSensitive(assetsJson, "fonts");
    cJSON* fontJson = NULL;
    cJSON_ArrayForEach(fontJson, fontsJsonArray) {
        RBE_ASSERT_FMT(properties->fontCount < CRE_PROPERTIES_ASSET_LIMIT, "Font count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        properties->fonts[properties->fontCount].file_path = json_get_string_new(fontJson, "file_path");
        rbe_logger_debug("Font File Path: '%s'", properties->fonts[properties->fontCount].file_path);
        properties->fonts[properties->fontCount].uid = json_get_string_new(fontJson, "uid");
        rbe_logger_debug("Font UID: '%s'", properties->fonts[properties->fontCount].uid);
        properties->fonts[properties->fontCount].size = json_get_int(fontJson, "size");
        rbe_logger_debug("Font UID: '%d'", properties->fonts[properties->fontCount].size);
        properties->fontCount++;
    }
}

void cre_json_configure_inputs(cJSON* configJson, RBEGameProperties* properties) {
    rbe_logger_debug("Loading inputs...");
    cJSON* inputsJsonArray = cJSON_GetObjectItemCaseSensitive(configJson, "inputs");
    cJSON* inputJson = NULL;
    cJSON_ArrayForEach(inputJson, inputsJsonArray) {
        RBE_ASSERT_FMT(properties->inputActionCount < CRE_PROPERTIES_ASSET_LIMIT, "Input count is over the max limit of %d, increase the limit of CRE_PROPERTIES_ASSET_LIMIT", CRE_INPUT_VALUES_LIMIT);
        RBEInputAction* inputAction = &properties->inputActions[properties->inputActionCount];
        inputAction->name = json_get_string_new(inputJson, "name");
        rbe_logger_debug("Input Action Name: '%s'", inputAction->name);
        cJSON* valuesJsonArray = cJSON_GetObjectItemCaseSensitive(inputJson, "values");
        cJSON* valueJson = NULL;
        cJSON_ArrayForEach(valueJson, valuesJsonArray) {
            RBE_ASSERT_FMT(inputAction->valueCount < CRE_INPUT_VALUES_LIMIT, "Reading input action '%s' value that is over the max limit of %d, increase the limit of CRE_INPUT_VALUES_LIMIT", inputAction->name, CRE_INPUT_VALUES_LIMIT);
            const char* value = cJSON_GetStringValue(valueJson);
            inputAction->values[inputAction->valueCount] = strdup(value);
            rbe_logger_debug("Input Action Value: '%s'", inputAction->values[inputAction->valueCount]);
            inputAction->valueCount++;
        }
        properties->inputActionCount++;
    }
}

RBEGameProperties* cre_json_load_config_file(const char* filePath) {
    RBEGameProperties* properties = rbe_game_props_create();
    char* fileContent = rbe_fs_read_file_contents(filePath, NULL);

    rbe_logger_debug("Loading game properties...");

    cJSON* configJson = cJSON_Parse(fileContent);
    if (configJson != NULL) {
        // Name
        properties->gameTitle = json_get_string_new(configJson, "name");
        rbe_logger_debug("Game Title: '%s'", properties->gameTitle);
        // Window Width
        properties->windowWidth = json_get_int(configJson, "window_width");
        rbe_logger_debug("Window Width: '%d'", properties->windowWidth);
        // Window Height
        properties->windowHeight = json_get_int(configJson, "window_height");
        rbe_logger_debug("Window Height '%d'", properties->windowHeight);
        // Resolution Width
        properties->resolutionWidth = json_get_int(configJson, "resolution_width");
        rbe_logger_debug("Resolution Width: '%d'", properties->resolutionWidth);
        // Resolution Height
        properties->resolutionHeight = json_get_int(configJson, "resolution_height");
        rbe_logger_debug("Resolution Height: '%d'", properties->resolutionHeight);
        // Target FPS
        properties->targetFPS = json_get_int(configJson, "target_fps");
        rbe_logger_debug("Target FPS '%d'", properties->targetFPS);
        // Initial Node Path
        properties->initialScenePath = json_get_string_new(configJson, "initial_node_path");
        rbe_logger_debug("Printing json initial node path '%s'", properties->initialScenePath);
        // Colliders Visible
        properties->areCollidersVisible = json_get_bool(configJson, "colliders_visible");
        rbe_logger_debug("Colliders Visible '%s'", properties->areCollidersVisible == true ? "true": "false");

        cre_json_configure_assets(configJson, properties);

        cre_json_configure_inputs(configJson, properties);
    }


    cJSON_Delete(configJson);
    RBE_MEM_FREE(fileContent);

    return properties;
}

void cre_json_load_scene_file(const char* filePath) {}
