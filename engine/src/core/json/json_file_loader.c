#include "json_file_loader.h"

#include <string.h>

#include <cjson/cJSON.h>

#include "../game_properties.h"
#include "../math/rbe_math.h"
#include "../utils/rbe_string_util.h"
#include "../utils/rbe_file_system_utils.h"
#include "../utils/logger.h"
#include "../utils/rbe_assert.h"
#include "../memory/rbe_mem.h"
#include "../ecs/component/node_component.h"

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

Vector2 json_get_vec2(cJSON* json, const char* key) {
    cJSON* vec2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    return (Vector2) {
        .x = (float) json_get_double(vec2Json, "x"),
        .y = (float) json_get_double(vec2Json, "y")
    };
}

Vector2 json_get_vec2_default(cJSON* json, const char* key, Vector2 defaultValue) {
    cJSON* vec2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    if (vec2Json != NULL) {
        return (Vector2) {
            .x = (float) json_get_double_default(vec2Json, "x", defaultValue.x),
            .y = (float) json_get_double_default(vec2Json, "y", defaultValue.y)
        };
    }
    return defaultValue;
}

Size2D json_get_size2d(cJSON* json, const char* key) {
    cJSON* size2dJson = cJSON_GetObjectItemCaseSensitive(json, key);
    return (Size2D) {
        .w = (float) json_get_double(size2dJson, "w"),
        .h = (float) json_get_double(size2dJson, "h")
    };
}

Size2D json_get_size2d_default(cJSON* json, const char* key, Size2D defaultValue) {
    cJSON* size2dJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (size2dJson != NULL) {
        return (Size2D) {
            .w = (float) json_get_double(size2dJson, "w"),
            .h = (float) json_get_double(size2dJson, "h")
        };
    }
    return defaultValue;
}

Rect2 json_get_rect2(cJSON* json, const char* key) {
    cJSON* rect2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    return (Rect2) {
        .x = (float) json_get_double(rect2Json, "x"),
        .y = (float) json_get_double(rect2Json, "y"),
        .w = (float) json_get_double(rect2Json, "w"),
        .h = (float) json_get_double(rect2Json, "h")
    };
}

Rect2 json_get_rect2_default(cJSON* json, const char* key, Rect2 defaultValue) {
    cJSON* rect2Json = cJSON_GetObjectItemCaseSensitive(json, key);
    if (rect2Json != NULL) {
        return (Rect2) {
            .x = (float) json_get_double(rect2Json, "x"),
            .y = (float) json_get_double(rect2Json, "y"),
            .w = (float) json_get_double(rect2Json, "w"),
            .h = (float) json_get_double(rect2Json, "h")
        };
    }
    return defaultValue;
}

Color json_get_linear_color(cJSON* json, const char* key) {
    cJSON* colorJson = cJSON_GetObjectItemCaseSensitive(json, key);
    return (Color) {
        .r = ((float) json_get_int(colorJson, "r") / 255.0f),
        .g = ((float) json_get_int(colorJson, "g") / 255.0f),
        .b = ((float) json_get_int(colorJson, "b") / 255.0f),
        .a = ((float) json_get_int(colorJson, "a") / 255.0f)
    };
}

Color json_get_linear_color_default(cJSON* json, const char* key, Color defaultValue) {
    cJSON* colorJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (colorJson != NULL) {
        return (Color) {
            .r = ((float) json_get_int_default(colorJson, "r", 255) / 255.0f),
            .g = ((float) json_get_int_default(colorJson, "g", 255) / 255.0f),
            .b = ((float) json_get_int_default(colorJson, "b", 255) / 255.0f),
            .a = ((float) json_get_int_default(colorJson, "a", 255) / 255.0f)
        };
    }
    return defaultValue;
}

// Project Configuration Files
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

// Scene Files
void cre_json_load_scene_node(cJSON* nodeJson, cJSON* parentNode) {
    const char* name = json_get_string_new(nodeJson, "name");
    NodeBaseType baseType = node_get_base_type(json_get_string_new(nodeJson, "type"));
    rbe_logger_debug("Node Name: '%s', Base Type: '%s'", name, node_get_base_type_string(baseType));
    // TODO: Load tags
    // TODO: Load external scene file

    // Load Components
    rbe_logger_debug("Loading components for '%'s", name);
    cJSON* componentsJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "components");
    cJSON* componentJson = NULL;
    cJSON_ArrayForEach(componentJson, componentsJsonArray) {
        const char* componentType = json_get_string(componentJson, "type");
        if (strcmp(componentType, "transform_2d") == 0) {
            const Vector2 position = json_get_vec2_default(componentJson, "position", (Vector2) {
                .x = 0.0f, .y = 0.0f
            });
            const Vector2 scale = json_get_vec2_default(componentJson, "scale", (Vector2) {
                .x = 1.0f, .y = 1.0f
            });
            const float rotation = (float) json_get_double_default(componentJson, "rotation", 0.0);
            const int zIndex = json_get_int_default(componentJson, "z_index", 0);
            const bool zIndexRelativeToParent = json_get_bool_default(componentJson, "z_index_relative_to_parent", true);
            const bool ignoreCamera = json_get_bool_default(componentJson, "ignore_camera", false);
            rbe_logger_debug("Transform2D\nposition: (%f, %f)\nscale: (%f, %f)\nrotation: %f\nz_index: %d\nz_index_relative_to_parent: %s\nignore_camera: %s",
                             position.x, position.y, scale.x, scale.y, rotation, zIndex, cre_bool_to_string(zIndexRelativeToParent), cre_bool_to_string(ignoreCamera));
        } else if (strcmp(componentType, "sprite") == 0) {
            const char* texturePath = json_get_string(componentJson, "texture_path");
            const Rect2 drawSource = json_get_rect2(componentJson, "draw_source");
            const Vector2 origin = json_get_vec2_default(componentJson, "origin", (Vector2) {
                .x = 0.0f, .y = 0.0f
            });
            const bool flipX = json_get_bool_default(componentJson, "flip_x", false);
            const bool flipY = json_get_bool_default(componentJson, "flip_y", false);
            const Color modulate = json_get_linear_color_default(componentJson, "modulate", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Sprite\ntexture_path: '%s', draw_source = (%f, %f, %f, %f), origin: (%f, %f), flip_x: %s, flip_y: %s, modulate: (%d, %d, %d, %d)",
                             texturePath, drawSource.x, drawSource.y, drawSource.w, drawSource.h, origin.x, origin.y,
                             cre_bool_to_string(flipX), cre_bool_to_string(flipY), modulate.r, modulate.g, modulate.b, modulate.a);
        } else if (strcmp(componentType, "animated_sprite") == 0) {
            const char* currentAnimationName = json_get_string(componentJson, "current_animation_name");
            const bool isPlaying = json_get_bool(componentJson, "is_playing");
            const Vector2 origin = json_get_vec2_default(componentJson, "origin", (Vector2) {
                .x = 0.0f, .y = 0.0f
            });
            const Color modulate = json_get_linear_color_default(componentJson, "modulate", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            const bool flipX = json_get_bool_default(componentJson, "flip_x", false);
            const bool flipY = json_get_bool_default(componentJson, "flip_y", false);
            rbe_logger_debug("Animated Sprite\ncurrent animation name: '%s'\nis playing: %s\norigin: (%f, %f)\nmodulate: (%f, %f, %f, %f)\nflip x: %s\nflip y: %s",
                             currentAnimationName, cre_bool_to_string(isPlaying), origin.x, origin.y, modulate.r, modulate.g, modulate.b, modulate.a,
                             cre_bool_to_string(flipX), cre_bool_to_string(flipY));
            // Animation
            cJSON* animationsJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "animations");
            cJSON* animationJson = NULL;
            cJSON_ArrayForEach(animationJson, animationsJsonArray) {
                const char* animationName = json_get_string(animationJson, "name");
                const int animationSpeed = json_get_int(animationJson, "speed");
                const bool animationLoops = json_get_bool(animationJson, "loops");
                rbe_logger_debug("Animation\nname: '%s'\nspeed: %d\nloops: %s", animationName, animationSpeed,
                                 cre_bool_to_string(animationLoops));
                // Animation Frames
                cJSON* framesJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "frames");
                cJSON* frameJson = NULL;
                cJSON_ArrayForEach(frameJson, framesJsonArray) {
                    const int frameIndex = json_get_int(frameJson, "frame");
                    const char* frameTexturePath = json_get_string(frameJson, "texture_path");
                    const Rect2 drawSource = json_get_rect2(frameJson, "draw_source");
                    rbe_logger_debug("Frame\nframe: %d\ntexture path: '%s'\ndraw source: (%f, %f, %f, %f)",
                                     frameIndex, frameTexturePath, drawSource.x, drawSource.y, drawSource.w, drawSource.h);
                }
            }
        } else if (strcmp(componentType, "text_label") == 0) {
            const char* uid = json_get_string(componentJson, "uid");
            const char* text = json_get_string(componentJson, "text");
            const Color color = json_get_linear_color_default(componentJson, "color", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Text Label\nuid: '%s'\ntext: '%s'\ncolor: (%f, %f, %f, %f)", uid, text, color.r, color.g, color.b, color.a);
        } else if (strcmp(componentType, "collider_2d") == 0) {
            const Size2D extents = json_get_size2d(componentJson, "extents");
            const Color color = json_get_linear_color_default(componentJson, "color", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Collider2D\nextents: (%f, %f)\ncolor: (%f, %f, %f, %f)", extents.w, extents.h, color.r, color.g, color.b, color.a);
        } else if (strcmp(componentType, "color_rect") == 0) {
            const Size2D size = json_get_size2d(componentJson, "size");
            const Color color = json_get_linear_color_default(componentJson, "color", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Color Rext\nsize: (%f, %f)\ncolor: (%f, %f, %f, %f)", size.w, size.h, color.r, color.g, color.b, color.a);
        } else if (strcmp(componentType, "script") == 0) {
            const char* classPath = json_get_string(componentJson, "class_path");
            const char* className = json_get_string(componentJson, "class_name");
            rbe_logger_debug("Script\nclass path: '%s'\nclass name: '%s'", classPath, className);
        } else {
            rbe_logger_error("component type '%s' in invalid!", componentType);
        }
    }
    // Load Children
    cJSON* childrenJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "children");
    cJSON* childNodeJson = NULL;
    cJSON_ArrayForEach(childNodeJson, childrenJsonArray) {
        cre_json_load_scene_node(childNodeJson, nodeJson);
    }
}

void cre_json_load_scene_file(const char* filePath) {
    char* fileContent = rbe_fs_read_file_contents(filePath, NULL);
    rbe_logger_debug("Loading scene from path '%s'", filePath);

    cJSON* sceneJson = cJSON_Parse(fileContent);
    if (sceneJson != NULL) {
        cre_json_load_scene_node(sceneJson, NULL);
    }
}
