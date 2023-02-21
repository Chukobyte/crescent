#include "json_file_loader.h"

#include "../seika/src/asset/asset_file_loader.h"
#include "../seika/src/utils/se_string_util.h"

#include "json_helper.h"
#include "../ecs/component/component_defaults.h"
#include "../ecs/component/transform2d_component.h"
#include "../ecs/component/sprite_component.h"
#include "../ecs/component/animated_sprite_component.h"
#include "../ecs/component/text_label_component.h"
#include "../ecs/component/script_component.h"
#include "../ecs/component/collider2d_component.h"
#include "../ecs/component/color_rect_component.h"
#include "../ecs/component/parallax_component.h"

typedef struct ShaderInstancePaths {
    char* vertex;
    char* fragment;
} ShaderInstancePaths;

ShaderInstancePaths json_get_shader_instance_paths(cJSON* json, const char* key) {
    ShaderInstancePaths shaderInstancePaths = { .vertex = NULL, .fragment = NULL };
    cJSON* shaderInstanceJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (shaderInstanceJson != NULL) {
        shaderInstancePaths.vertex = json_get_string_new(shaderInstanceJson, "vertex_path");
        shaderInstancePaths.fragment = json_get_string_new(shaderInstanceJson, "fragment_path");
    }
    return shaderInstancePaths;
}

void json_node_set_shader_instance_paths(JsonSceneNode* node, cJSON* componentJson, const char* key) {
    ShaderInstancePaths shaderInstancePaths = json_get_shader_instance_paths(componentJson, key);
    if (shaderInstancePaths.vertex != NULL && shaderInstancePaths.fragment != NULL) {
        if (node->shaderInstanceVertexPath != NULL && node->shaderInstanceFragmentPath != NULL) {
            SE_MEM_FREE(node->shaderInstanceVertexPath);
            SE_MEM_FREE(node->shaderInstanceFragmentPath);
        }
        node->shaderInstanceVertexPath = shaderInstancePaths.vertex;
        node->shaderInstanceFragmentPath = shaderInstancePaths.fragment;
    }
}

//--- Scene Files ---//

/*
 * Currently supports scene templates.  Can override default values with the ones currently within the scene.
 * Not currently supporting overriding children nodes values.
 */

JsonSceneNode* cre_json_load_scene_node(cJSON* nodeJson, JsonSceneNode* parentNode);

JsonSceneNode* cre_json_create_new_node() {
    JsonSceneNode* node = SE_MEM_ALLOCATE(JsonSceneNode);
    node->name = NULL;
    node->fontUID = NULL;
    node->spriteTexturePath = NULL;
    node->shaderInstanceVertexPath = NULL;
    node->shaderInstanceFragmentPath = NULL;
    node->externalNodeSource = NULL;
    node->fromExternalNodeSource = false;
    node->parent = NULL;
    node->childrenCount = 0;
    node->type = NodeBaseType_INVALID;
    for (size_t i = 0; i < MAX_COMPONENTS; i++) {
        node->components[i] = NULL;
    }
    return node;
}


JsonSceneNode* cre_json_load_scene_file(const char* filePath) {
    char* fileContent = sf_asset_file_loader_read_file_contents_as_string(filePath, NULL);

    se_logger_debug("Loading scene from path '%s'", filePath);

    cJSON* sceneJson = cJSON_Parse(fileContent);
    SE_MEM_FREE(fileContent);
    if (sceneJson != NULL) {
        return cre_json_load_scene_node(sceneJson, NULL); // Return root node
    }
    se_logger_error("Error loading scene file from path '%s', wasn't able to locate file!", filePath);

    return NULL;
}

// Recursive
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
    if (node->components[ComponentDataIndex_PARALLAX] != NULL) {
        parallax_component_delete(node->components[ComponentDataIndex_PARALLAX]);
    }

    if (node->shaderInstanceVertexPath != NULL) {
        SE_MEM_FREE(node->shaderInstanceVertexPath);
    }
    if (node->shaderInstanceFragmentPath != NULL) {
        SE_MEM_FREE(node->shaderInstanceFragmentPath);
    }

    // String Arrays
    SE_MEM_FREE(node->name);
    SE_MEM_FREE(node->fontUID);
    SE_MEM_FREE(node->spriteTexturePath);
}

//--- Private functions ---//

// Component functions
void cre_json_transform2d_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    Transform2DComponent* transform2DComponent = NULL;
    if (node->components[ComponentDataIndex_TRANSFORM_2D] == NULL) {
        transform2DComponent = transform2d_component_create();
        transform2DComponent->localTransform.position = json_get_vec2_default(componentJson, "position", DEFAULT_COMPONENT_TRANSFORM2D_POSITION);
        transform2DComponent->localTransform.scale = json_get_vec2_default(componentJson, "scale", DEFAULT_COMPONENT_TRANSFORM2D_SCALE);
        transform2DComponent->localTransform.rotation = (float) json_get_double_default(componentJson, "rotation", DEFAULT_COMPONENT_TRANSFORM2D_ROTATION);
        transform2DComponent->zIndex = json_get_int_default(componentJson, "z_index", DEFAULT_COMPONENT_TRANSFORM2D_Z_INDEX);
        transform2DComponent->isZIndexRelativeToParent = json_get_bool_default(componentJson, "z_index_relative_to_parent", DEFAULT_COMPONENT_TRANSFORM2D_Z_INDEX_RELATIVE_TO_PARENT);
        transform2DComponent->ignoreCamera = json_get_bool_default(componentJson, "ignore_camera", DEFAULT_COMPONENT_TRANSFORM2D_IGNORE_CAMERA);
        node->components[ComponentDataIndex_TRANSFORM_2D] = transform2DComponent;
    } else {
        transform2DComponent = (Transform2DComponent*) node->components[ComponentDataIndex_TRANSFORM_2D];
        transform2DComponent->localTransform.position = json_get_vec2_default(componentJson, "position", transform2DComponent->localTransform.position);
        transform2DComponent->localTransform.scale = json_get_vec2_default(componentJson, "scale", transform2DComponent->localTransform.scale);
        transform2DComponent->localTransform.rotation = (float) json_get_double_default(componentJson, "rotation", transform2DComponent->localTransform.rotation);
        transform2DComponent->zIndex = json_get_int_default(componentJson, "z_index", transform2DComponent->zIndex);
        transform2DComponent->isZIndexRelativeToParent = json_get_bool_default(componentJson, "z_index_relative_to_parent", transform2DComponent->isZIndexRelativeToParent);
        transform2DComponent->ignoreCamera = json_get_bool_default(componentJson, "ignore_camera", transform2DComponent->ignoreCamera);
    }
    se_logger_debug(
        "Transform2D\nposition: (%f, %f)\nscale: (%f, %f)\nrotation: %f\nz_index: %d\nz_index_relative_to_parent: %s\nignore_camera: %s",
        transform2DComponent->localTransform.position.x, transform2DComponent->localTransform.position.y,
        transform2DComponent->localTransform.scale.x, transform2DComponent->localTransform.scale.y,
        transform2DComponent->localTransform.rotation,
        transform2DComponent->zIndex, se_bool_to_string(transform2DComponent->isZIndexRelativeToParent),
        se_bool_to_string(transform2DComponent->ignoreCamera));
}

void cre_json_sprite_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    SpriteComponent* spriteComponent = NULL;
    if (node->components[ComponentDataIndex_SPRITE] == NULL) {
        spriteComponent = sprite_component_create();
        node->spriteTexturePath = json_get_string_new(componentJson, "texture_path");
        spriteComponent->drawSource = json_get_rect2(componentJson, "draw_source");
        spriteComponent->origin = json_get_vec2_default(componentJson, "origin", DEFAULT_COMPONENT_SPRITE_ORIGIN);
        spriteComponent->flipX = json_get_bool_default(componentJson, "flip_x", DEFAULT_COMPONENT_SPRITE_FLIP_X);
        spriteComponent->flipY = json_get_bool_default(componentJson, "flip_y", DEFAULT_COMPONENT_SPRITE_FLIP_Y);
        spriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", DEFAULT_COMPONENT_SPRITE_MODULATE);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
        node->components[ComponentDataIndex_SPRITE] = spriteComponent;
    } else {
        spriteComponent = node->components[ComponentDataIndex_SPRITE];
        char* newTexturePath = json_get_string_new_unchecked(componentJson, "texture_path");
        if (newTexturePath != NULL) {
            SE_MEM_FREE(node->spriteTexturePath);
            node->spriteTexturePath = newTexturePath;
        }
        spriteComponent->drawSource = json_get_rect2_default(componentJson, "draw_source", spriteComponent->drawSource);
        spriteComponent->origin = json_get_vec2_default(componentJson, "origin", spriteComponent->origin);
        spriteComponent->flipX = json_get_bool_default(componentJson, "flip_x", spriteComponent->flipX);
        spriteComponent->flipY = json_get_bool_default(componentJson, "flip_y", spriteComponent->flipY);
        spriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", spriteComponent->modulate);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
    }
    se_logger_debug(
        "Sprite\ntexture_path: '%s'\ndraw_source = (%f, %f, %f, %f)\norigin: (%f, %f)\nflip_x: %s\nflip_y: %s\nmodulate: (%f, %f, %f, %f)",
        node->spriteTexturePath,
        spriteComponent->drawSource.x, spriteComponent->drawSource.y, spriteComponent->drawSource.w,
        spriteComponent->drawSource.h,
        spriteComponent->origin.x, spriteComponent->origin.y,
        se_bool_to_string(spriteComponent->flipX), se_bool_to_string(spriteComponent->flipY),
        spriteComponent->modulate.r, spriteComponent->modulate.g, spriteComponent->modulate.b,
        spriteComponent->modulate.a);
}

void cre_json_animated_sprite_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    AnimatedSpriteComponentData* animatedSpriteComponent = NULL;
    char* currentAnimationName = NULL;
    if (node->components[ComponentDataIndex_ANIMATED_SPRITE] == NULL) {
        animatedSpriteComponent = animated_sprite_component_data_create();
        currentAnimationName = json_get_string_new(componentJson, "current_animation_name");
        animatedSpriteComponent->isPlaying = json_get_bool(componentJson, "is_playing");
        animatedSpriteComponent->origin = json_get_vec2_default(componentJson, "origin", DEFAULT_COMPONENT_ANIMATED_SPRITE_ORIGIN);
        animatedSpriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", DEFAULT_COMPONENT_ANIMATED_SPRITE_MODULATE);
        animatedSpriteComponent->flipX = json_get_bool_default(componentJson, "flip_x", DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_X);
        animatedSpriteComponent->flipY = json_get_bool_default(componentJson, "flip_y", DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_Y);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
        // Animation (Won't look at local anims for now)
        cJSON* animationsJsonArray = cJSON_GetObjectItemCaseSensitive(componentJson, "animations");
        cJSON* animationJson = NULL;
        cJSON_ArrayForEach(animationJson, animationsJsonArray) {
            AnimationData animation;
            strcpy(animation.name, json_get_string(animationJson, "name"));
            animation.speed = json_get_int(animationJson, "speed");
            animation.doesLoop = json_get_bool(animationJson, "loops");
            animation.frameCount = 0;
            animation.isValid = true;
            se_logger_debug("Animation\nname: '%s'\nspeed: %d\nloops: %s",
                            animation.name, animation.speed, se_bool_to_string(animation.doesLoop));
            // Animation Frames
            cJSON* framesJsonArray = cJSON_GetObjectItemCaseSensitive(animationJson, "frames");
            cJSON* frameJson = NULL;
            cJSON_ArrayForEach(frameJson, framesJsonArray) {
                AnimationFrameData frameData;
                frameData.frame = json_get_int(frameJson, "frame");
                strcpy(frameData.texturePath, json_get_string(frameJson, "texture_path"));
                frameData.drawSource = json_get_rect2(frameJson, "draw_source");
                se_logger_debug("Frame\nframe: %d\ntexture path: '%s'\ndraw source: (%f, %f, %f, %f)",
                                frameData.frame, frameData.texturePath,
                                frameData.drawSource.x, frameData.drawSource.y, frameData.drawSource.w,
                                frameData.drawSource.h);
                animation.animationFrames[animation.frameCount++] = frameData;
            }
            animated_sprite_component_data_add_animation(animatedSpriteComponent, animation);
            if (strcmp(currentAnimationName, animation.name) == 0) {
                animatedSpriteComponent->currentAnimation = animation;
            }
        }
        node->components[ComponentDataIndex_ANIMATED_SPRITE] = animatedSpriteComponent;
    } else {
        animatedSpriteComponent = (AnimatedSpriteComponentData*) node->components[ComponentDataIndex_ANIMATED_SPRITE];
        currentAnimationName = json_get_string_default_new(componentJson, "current_animation_name", animatedSpriteComponent->currentAnimation.name);
        animatedSpriteComponent->isPlaying = json_get_bool_default(componentJson, "is_playing", animatedSpriteComponent->isPlaying);
        animatedSpriteComponent->origin = json_get_vec2_default(componentJson, "origin", animatedSpriteComponent->origin);
        animatedSpriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", animatedSpriteComponent->modulate);
        animatedSpriteComponent->flipX = json_get_bool_default(componentJson, "flip_x", animatedSpriteComponent->flipX);
        animatedSpriteComponent->flipY = json_get_bool_default(componentJson, "flip_y", animatedSpriteComponent->flipY);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
    }
    se_logger_debug(
        "Animated Sprite\ncurrent animation name: '%s'\nis playing: %s\norigin: (%f, %f)\nmodulate: (%f, %f, %f, %f)\nflip x: %s\nflip y: %s",
        currentAnimationName, se_bool_to_string(animatedSpriteComponent->isPlaying),
        animatedSpriteComponent->origin.x, animatedSpriteComponent->origin.y,
        animatedSpriteComponent->modulate.r, animatedSpriteComponent->modulate.g,
        animatedSpriteComponent->modulate.b, animatedSpriteComponent->modulate.a,
        se_bool_to_string(animatedSpriteComponent->flipX),
        se_bool_to_string(animatedSpriteComponent->flipY));
    if (currentAnimationName != NULL) {
        SE_MEM_FREE(currentAnimationName);
    }
}

void cre_json_text_label_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    TextLabelComponent* textLabelComponent = NULL;
    if (node->components[ComponentDataIndex_TEXT_LABEL] == NULL) {
        textLabelComponent = text_label_component_create();
        node->fontUID = json_get_string_new(componentJson, "uid");
        strcpy(textLabelComponent->text, json_get_string(componentJson, "text"));
        textLabelComponent->color = json_get_linear_color_default(componentJson, "color", DEFAULT_COMPONENT_TEXT_LABEL_COLOR);
        node->components[ComponentDataIndex_TEXT_LABEL] = textLabelComponent;
    } else {
        textLabelComponent = (TextLabelComponent*) node->components[ComponentDataIndex_TEXT_LABEL];
        char* newFontUID = json_get_string_new_unchecked(componentJson, "uid");
        if (newFontUID != NULL) {
            SE_MEM_FREE(node->fontUID);
            node->fontUID = newFontUID;
        }
        strcpy(textLabelComponent->text, json_get_string_default(componentJson, "text", textLabelComponent->text));
        textLabelComponent->color = json_get_linear_color_default(componentJson, "color", textLabelComponent->color);
    }
    se_logger_debug("Text Label\nuid: '%s'\ntext: '%s'\ncolor: (%f, %f, %f, %f)",
                    node->fontUID,
                    textLabelComponent->text,
                    textLabelComponent->color.r, textLabelComponent->color.g, textLabelComponent->color.b,
                    textLabelComponent->color.a);
}

void cre_json_collider2d_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    Collider2DComponent* collider2DComponent = NULL;
    if (node->components[ComponentDataIndex_COLLIDER_2D] == NULL) {
        collider2DComponent = collider2d_component_create();
        collider2DComponent->extents = json_get_size2d(componentJson, "extents");
        collider2DComponent->color = json_get_linear_color_default(componentJson, "color", DEFAULT_COMPONENT_COLLIDER2D_COLOR);
        node->components[ComponentDataIndex_COLLIDER_2D] = collider2DComponent;
    } else {
        collider2DComponent = (Collider2DComponent*) node->components[ComponentDataIndex_COLLIDER_2D];
        collider2DComponent->extents = json_get_size2d_default(componentJson, "extents", collider2DComponent->extents);
        collider2DComponent->color = json_get_linear_color_default(componentJson, "color", collider2DComponent->color);
    }
    se_logger_debug("Collider2D\nextents: (%f, %f)\ncolor: (%f, %f, %f, %f)",
                    collider2DComponent->extents.w, collider2DComponent->extents.h,
                    collider2DComponent->color.r, collider2DComponent->color.g, collider2DComponent->color.b,
                    collider2DComponent->color.a);
}

void cre_json_color_rect_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    ColorRectComponent* colorRectComponent = NULL;
    if (node->components[ComponentDataIndex_COLOR_RECT] == NULL) {
        colorRectComponent = color_rect_component_create();
        colorRectComponent->size = json_get_size2d(componentJson, "size");
        colorRectComponent->color = json_get_linear_color_default(componentJson, "color", DEFAULT_COMPONENT_COLOR_RECT_COLOR);
        node->components[ComponentDataIndex_COLOR_RECT] = colorRectComponent;
    } else {
        colorRectComponent = (ColorRectComponent*) node->components[ComponentDataIndex_COLOR_RECT];
        colorRectComponent->size = json_get_size2d_default(componentJson, "size", colorRectComponent->size);
        colorRectComponent->color = json_get_linear_color_default(componentJson, "color", colorRectComponent->color);
    }
    se_logger_debug("Color Rext\nsize: (%f, %f)\ncolor: (%f, %f, %f, %f)",
                    colorRectComponent->size.w, colorRectComponent->size.h,
                    colorRectComponent->color.r, colorRectComponent->color.g, colorRectComponent->color.b,
                    colorRectComponent->color.a);
}

void cre_json_script_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    ScriptComponent* scriptComponent = NULL;
    if (node->components[ComponentDataIndex_SCRIPT] == NULL) {
        scriptComponent = script_component_create(
                              json_get_string(componentJson, "class_path"),
                              json_get_string(componentJson, "class_name")
                          );
        scriptComponent->contextType = ScriptContextType_PYTHON;
        node->components[ComponentDataIndex_SCRIPT] = scriptComponent;
    } else {
        // No override for scripts for now...
        scriptComponent = (ScriptComponent*) node->components[ComponentDataIndex_SCRIPT];
    }
    se_logger_debug("Script\nclass path: '%s'\nclass name: '%s'",
                    scriptComponent->classPath, scriptComponent->className);
}

void cre_json_parallax_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    ParallaxComponent* parallaxComponent = NULL;
    if (node->components[ComponentDataIndex_PARALLAX] == NULL) {
        parallaxComponent = parallax_component_create();
        parallaxComponent->scrollSpeed =  json_get_vec2_default(componentJson, "scroll_speed", DEFAULT_COMPONENT_PARALLAX_SCROLL_SPEED);
        node->components[ComponentDataIndex_PARALLAX] = parallaxComponent;
    } else {
        parallaxComponent = (ParallaxComponent*) node->components[ComponentDataIndex_PARALLAX];
        parallaxComponent->scrollSpeed = json_get_vec2_default(componentJson, "scroll_speed", parallaxComponent->scrollSpeed);
    }
    se_logger_debug("Parallax\nscroll_speed: (%f, %f)", parallaxComponent->scrollSpeed.x, parallaxComponent->scrollSpeed.y);
}

// Recursive
void cre_json_set_all_child_nodes_from_external_source(JsonSceneNode* node) {
    node->fromExternalNodeSource = true;
    for (size_t i = 0; i < node->childrenCount; i++) {
        cre_json_set_all_child_nodes_from_external_source(node->children[i]);
    }
}

// Recursive
JsonSceneNode* cre_json_load_scene_node(cJSON* nodeJson, JsonSceneNode* parentNode) {
    JsonSceneNode* node = NULL;
    // Store local file children before loading ones from external scene
    cJSON* childrenJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "children");
    // Check if external scene file, use a different code path, and exit out
    char* externalSceneNodeString = json_get_string_default_new(nodeJson, "external_node_source", NULL);
    if (externalSceneNodeString != NULL) {
        node = cre_json_load_scene_file(externalSceneNodeString);
        SE_MEM_FREE(node->name);
        cre_json_set_all_child_nodes_from_external_source(node);
    } else {
        node = cre_json_create_new_node();
    }
    node->name = json_get_string_new(nodeJson, "name");
    node->type = node_get_base_type(json_get_string(nodeJson, "type"));
    node->tags = NULL;
    node->parent = parentNode;
    node->externalNodeSource = externalSceneNodeString;
    se_logger_debug("Node Name: '%s', Base Type: '%s'", node->name, node_get_base_type_string(node->type));

    // Checking if not root
    if (parentNode) {
        parentNode->children[parentNode->childrenCount++] = node;
    }

    // Load Components
    se_logger_debug("Loading components for '%'s", node->name);
    cJSON* componentsJsonArray = cJSON_GetObjectItemCaseSensitive(nodeJson, "components");
    cJSON* componentJson = NULL;
    cJSON_ArrayForEach(componentJson, componentsJsonArray) {
        const char* componentType = json_get_string(componentJson, "type");
        if (strcmp(componentType, "transform_2d") == 0) {
            cre_json_transform2d_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "sprite") == 0) {
            cre_json_sprite_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "animated_sprite") == 0) {
            cre_json_animated_sprite_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "text_label") == 0) {
            cre_json_text_label_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "collider_2d") == 0) {
            cre_json_collider2d_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "color_rect") == 0) {
            cre_json_color_rect_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "script") == 0) {
            cre_json_script_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "parallax") == 0) {
            cre_json_parallax_create_or_set_default(node, componentJson);
        } else {
            se_logger_error("component type '%s' in invalid!", componentType);
        }
    }
    // Load Children
    cJSON* childNodeJson = NULL;
    cJSON_ArrayForEach(childNodeJson, childrenJsonArray) {
        cre_json_load_scene_node(childNodeJson, node);
    }
    return node;
}
