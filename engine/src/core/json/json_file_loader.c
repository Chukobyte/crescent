#include "json_file_loader.h"

#include "json_helper.h"
#include "../game_properties.h"
#include "../ecs/component/transform2d_component.h"
#include "../ecs/component/sprite_component.h"
#include "../ecs/component/animated_sprite_component.h"
#include "../ecs/component/text_label_component.h"
#include "../ecs/component/script_component.h"
#include "../ecs/component/collider2d_component.h"
#include "../ecs/component/color_rect_component.h"
#include "../utils/rbe_string_util.h"
#include "../utils/rbe_file_system_utils.h"
#include "../memory/rbe_mem.h"

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
JsonSceneNode* cre_json_create_new_node() {
    JsonSceneNode* node = RBE_MEM_ALLOCATE(JsonSceneNode);
    node->name = NULL;
    node->fontUID = NULL;
    node->spriteTexturePath = NULL;
    node->parent = NULL;
    node->childrenCount = 0;
    node->type = NodeBaseType_INVALID;
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        node->components[i] = NULL;
    }
    return node;
}

JsonSceneNode* cre_json_load_scene_node(cJSON* nodeJson, JsonSceneNode* parentNode) {
    JsonSceneNode* node = cre_json_create_new_node();
    node->parent = parentNode;
    node->name = json_get_string_new(nodeJson, "name");
    node->type = node_get_base_type(json_get_string_new(nodeJson, "type"));
    rbe_logger_debug("Node Name: '%s', Base Type: '%s'", node->name, node_get_base_type_string(node->type));

    // Checking if not root
    if (parentNode) {
        parentNode->children[parentNode->childrenCount++] = node;
    }
    // TODO: Load tags
    // TODO: Load external scene file

    // Load Components
    rbe_logger_debug("Loading components for '%'s", node->name);
    cJSON* componentsJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "components");
    cJSON* componentJson = NULL;
    cJSON_ArrayForEach(componentJson, componentsJsonArray) {
        const char* componentType = json_get_string(componentJson, "type");
        if (strcmp(componentType, "transform_2d") == 0) {
            Transform2DComponent* transform2DComponent = transform2d_component_create();
            transform2DComponent->localTransform.position = json_get_vec2_default(componentJson, "position", (Vector2) {
                .x = 0.0f, .y = 0.0f
            });
            transform2DComponent->localTransform.scale = json_get_vec2_default(componentJson, "scale", (Vector2) {
                .x = 1.0f, .y = 1.0f
            });
            transform2DComponent->localTransform.rotation = (float) json_get_double_default(componentJson, "rotation", 0.0);
            transform2DComponent->zIndex = json_get_int_default(componentJson, "z_index", 0);
            transform2DComponent->isZIndexRelativeToParent = json_get_bool_default(componentJson, "z_index_relative_to_parent", true);
            transform2DComponent->ignoreCamera = json_get_bool_default(componentJson, "ignore_camera", false);
            rbe_logger_debug("Transform2D\nposition: (%f, %f)\nscale: (%f, %f)\nrotation: %f\nz_index: %d\nz_index_relative_to_parent: %s\nignore_camera: %s",
                             transform2DComponent->localTransform.position.x, transform2DComponent->localTransform.position.y,
                             transform2DComponent->localTransform.scale.x, transform2DComponent->localTransform.scale.y,
                             transform2DComponent->localTransform.rotation,
                             transform2DComponent->zIndex, cre_bool_to_string(transform2DComponent->isZIndexRelativeToParent),
                             cre_bool_to_string(transform2DComponent->ignoreCamera));
            node->components[ComponentDataIndex_TRANSFORM_2D] = transform2DComponent;
        } else if (strcmp(componentType, "sprite") == 0) {
            SpriteComponent* spriteComponent = sprite_component_create();
            node->spriteTexturePath = json_get_string_new(componentJson, "texture_path");
            spriteComponent->drawSource = json_get_rect2(componentJson, "draw_source");
            spriteComponent->origin = json_get_vec2_default(componentJson, "origin", (Vector2) {
                .x = 0.0f, .y = 0.0f
            });
            spriteComponent->flipX = json_get_bool_default(componentJson, "flip_x", false);
            spriteComponent->flipY = json_get_bool_default(componentJson, "flip_y", false);
            spriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Sprite\ntexture_path: '%s'\ndraw_source = (%f, %f, %f, %f)\norigin: (%f, %f)\nflip_x: %s\nflip_y: %s\nmodulate: (%f, %f, %f, %f)",
                             node->spriteTexturePath,
                             spriteComponent->drawSource.x, spriteComponent->drawSource.y, spriteComponent->drawSource.w, spriteComponent->drawSource.h,
                             spriteComponent->origin.x, spriteComponent->origin.y,
                             cre_bool_to_string(spriteComponent->flipX), cre_bool_to_string(spriteComponent->flipY),
                             spriteComponent->modulate.r, spriteComponent->modulate.g, spriteComponent->modulate.b, spriteComponent->modulate.a);
            node->components[ComponentDataIndex_SPRITE] = spriteComponent;
        } else if (strcmp(componentType, "animated_sprite") == 0) {
            AnimatedSpriteComponentData* animatedSpriteComponent = animated_sprite_component_data_create();
            const char* currentAnimationName = json_get_string(componentJson, "current_animation_name");
            animatedSpriteComponent->isPlaying = json_get_bool(componentJson, "is_playing");
            animatedSpriteComponent->origin = json_get_vec2_default(componentJson, "origin", (Vector2) {
                .x = 0.0f, .y = 0.0f
            });
            animatedSpriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            animatedSpriteComponent->flipX = json_get_bool_default(componentJson, "flip_x", false);
            animatedSpriteComponent->flipY = json_get_bool_default(componentJson, "flip_y", false);
            rbe_logger_debug("Animated Sprite\ncurrent animation name: '%s'\nis playing: %s\norigin: (%f, %f)\nmodulate: (%f, %f, %f, %f)\nflip x: %s\nflip y: %s",
                             currentAnimationName, cre_bool_to_string(animatedSpriteComponent->isPlaying),
                             animatedSpriteComponent->origin.x, animatedSpriteComponent->origin.y,
                             animatedSpriteComponent->modulate.r, animatedSpriteComponent->modulate.g, animatedSpriteComponent->modulate.b, animatedSpriteComponent->modulate.a,
                             cre_bool_to_string(animatedSpriteComponent->flipX), cre_bool_to_string(animatedSpriteComponent->flipY));
            // Animation
            cJSON* animationsJsonArray = cJSON_GetObjectItemCaseSensitive(componentJson, "animations");
            cJSON* animationJson = NULL;
            cJSON_ArrayForEach(animationJson, animationsJsonArray) {
                AnimationData animation;
                strcpy(animation.name, json_get_string(animationJson, "name"));
                animation.speed = json_get_int(animationJson, "speed");
                animation.doesLoop = json_get_bool(animationJson, "loops");
                animation.frameCount = 0;
                animation.isValid = true;
                rbe_logger_debug("Animation\nname: '%s'\nspeed: %d\nloops: %s",
                                 animation.name, animation.speed, cre_bool_to_string(animation.doesLoop));
                // Animation Frames
                cJSON* framesJsonArray = cJSON_GetObjectItemCaseSensitive(animationJson, "frames");
                cJSON* frameJson = NULL;
                cJSON_ArrayForEach(frameJson, framesJsonArray) {
                    AnimationFrameData frameData;
                    frameData.frame = json_get_int(frameJson, "frame");
                    strcpy(frameData.texturePath, json_get_string(frameJson, "texture_path"));
                    frameData.drawSource = json_get_rect2(frameJson, "draw_source");
                    rbe_logger_debug("Frame\nframe: %d\ntexture path: '%s'\ndraw source: (%f, %f, %f, %f)",
                                     frameData.frame, frameData.texturePath,
                                     frameData.drawSource.x, frameData.drawSource.y, frameData.drawSource.w, frameData.drawSource.h);
                    animation.animationFrames[animation.frameCount++] = frameData;
                }
                animatedSpriteComponent->animations[animatedSpriteComponent->animationCount++] = animation;
                if (strcmp(currentAnimationName, animation.name) == 0) {
                    animatedSpriteComponent->currentAnimation = animation;
                }
            }
            node->components[ComponentDataIndex_ANIMATED_SPRITE] = animatedSpriteComponent;
        } else if (strcmp(componentType, "text_label") == 0) {
            TextLabelComponent* textLabelComponent = text_label_component_create();
            node->fontUID = json_get_string_new(componentJson, "uid");
            strcpy(textLabelComponent->text, json_get_string(componentJson, "text"));
            textLabelComponent->color = json_get_linear_color_default(componentJson, "color", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Text Label\nuid: '%s'\ntext: '%s'\ncolor: (%f, %f, %f, %f)",
                             node->fontUID,
                             textLabelComponent->text,
                             textLabelComponent->color.r, textLabelComponent->color.g, textLabelComponent->color.b, textLabelComponent->color.a);
            node->components[ComponentDataIndex_TEXT_LABEL] = textLabelComponent;
        } else if (strcmp(componentType, "collider_2d") == 0) {
            Collider2DComponent* collider2DComponent = collider2d_component_create();
            collider2DComponent->extents = json_get_size2d(componentJson, "extents");
            collider2DComponent->color = json_get_linear_color_default(componentJson, "color", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Collider2D\nextents: (%f, %f)\ncolor: (%f, %f, %f, %f)",
                             collider2DComponent->extents.w, collider2DComponent->extents.h,
                             collider2DComponent->color.r, collider2DComponent->color.g, collider2DComponent->color.b, collider2DComponent->color.a);
            node->components[ComponentDataIndex_COLLIDER_2D] = collider2DComponent;
        } else if (strcmp(componentType, "color_rect") == 0) {
            ColorRectComponent* colorRectComponent = color_rect_component_create();
            colorRectComponent->size = json_get_size2d(componentJson, "size");
            colorRectComponent->color = json_get_linear_color_default(componentJson, "color", (Color) {
                .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f
            });
            rbe_logger_debug("Color Rext\nsize: (%f, %f)\ncolor: (%f, %f, %f, %f)",
                             colorRectComponent->size.w, colorRectComponent->size.h,
                             colorRectComponent->color.r, colorRectComponent->color.g, colorRectComponent->color.b, colorRectComponent->color.a);
            node->components[ComponentDataIndex_COLOR_RECT] = colorRectComponent;
        } else if (strcmp(componentType, "script") == 0) {
            ScriptComponent* scriptComponent = script_component_create(
                                                   json_get_string(componentJson, "class_path"),
                                                   json_get_string(componentJson, "class_name")
                                               );
            scriptComponent->contextType = ScriptContextType_PYTHON;
            rbe_logger_debug("Script\nclass path: '%s'\nclass name: '%s'",
                             scriptComponent->classPath, scriptComponent->className);
            node->components[ComponentDataIndex_SCRIPT] = scriptComponent;
        } else {
            rbe_logger_error("component type '%s' in invalid!", componentType);
        }
    }
    // Load Children
    cJSON* childrenJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "children");
    cJSON* childNodeJson = NULL;
    cJSON_ArrayForEach(childNodeJson, childrenJsonArray) {
        cre_json_load_scene_node(childNodeJson, node);
    }
    return node;
}

JsonSceneNode* cre_json_load_scene_file(const char* filePath) {
    char* fileContent = rbe_fs_read_file_contents(filePath, NULL);
    rbe_logger_debug("Loading scene from path '%s'", filePath);

    cJSON* sceneJson = cJSON_Parse(fileContent);
    if (sceneJson != NULL) {
        return cre_json_load_scene_node(sceneJson, NULL); // Return root node
    }
    rbe_logger_error("Error loading scene file from path '%s', wasn't able to locate file!", filePath);

    return NULL;
}

void cre_json_delete_json_scene_node(JsonSceneNode* node) {
    // Children
    for (size_t i = 0; i < node->childrenCount; i++) {
        cre_json_delete_json_scene_node(node->children[i]);
    }
    // Components
    if (node->components[ComponentDataIndex_TRANSFORM_2D] != NULL) {
        transform2d_component_delete(node->components[ComponentDataIndex_TRANSFORM_2D]);
    }
    if (node->components[ComponentDataIndex_SPRITE] != NULL) {
        sprite_component_delete(node->components[ComponentDataIndex_SPRITE]);
    }
    if (node->components[ComponentDataIndex_ANIMATED_SPRITE] != NULL) {
        animated_sprite_component_data_delete(node->components[ComponentDataIndex_ANIMATED_SPRITE]);
    }
    if (node->components[ComponentDataIndex_TEXT_LABEL] != NULL) {
        text_label_component_delete(node->components[ComponentDataIndex_TEXT_LABEL]);
    }
    if (node->components[ComponentDataIndex_SCRIPT] != NULL) {
        script_component_delete(node->components[ComponentDataIndex_SCRIPT]);
    }
    if (node->components[ComponentDataIndex_COLLIDER_2D] != NULL) {
        collider2d_component_delete(node->components[ComponentDataIndex_COLLIDER_2D]);
    }
    if (node->components[ComponentDataIndex_COLOR_RECT] != NULL) {
        color_rect_component_delete(node->components[ComponentDataIndex_COLOR_RECT]);
    }
    // String Arrays
    RBE_MEM_FREE(node->name);
    RBE_MEM_FREE(node->fontUID);
    RBE_MEM_FREE(node->spriteTexturePath);
}
