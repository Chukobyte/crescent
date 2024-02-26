#include "json_file_loader.h"

#include <seika/asset/asset_file_loader.h>
#include <seika/utils/se_string_util.h>

#include "json_helper.h"
#include "../tilemap/tilemap.h"
#include "../ecs/ecs_globals.h"
#include "../ecs/components/component_defaults.h"
#include "../ecs/components/transform2d_component.h"
#include "../ecs/components/sprite_component.h"
#include "../ecs/components/animated_sprite_component.h"
#include "../ecs/components/text_label_component.h"
#include "../ecs/components/script_component.h"
#include "../ecs/components/collider2d_component.h"
#include "../ecs/components/color_rect_component.h"
#include "../ecs/components/parallax_component.h"
#include "../ecs/components/particles2d_component.h"
#include "../ecs/components/tilemap_component.h"

typedef struct ShaderInstancePaths {
    char* shader;
    char* vertex;
    char* fragment;
} ShaderInstancePaths;

ShaderInstancePaths json_get_shader_instance_paths(cJSON* json, const char* key) {
    ShaderInstancePaths shaderInstancePaths = { .shader = NULL, .vertex = NULL, .fragment = NULL };
    cJSON* shaderInstanceJson = cJSON_GetObjectItemCaseSensitive(json, key);
    if (shaderInstanceJson != NULL) {
        // First check for shader file, if it doesn't exist check for 'raw' shader files (that copy and paste base shaders)
        shaderInstancePaths.shader = json_get_string_new_unchecked(shaderInstanceJson, "shader_path");
        if (!shaderInstancePaths.shader) {
            shaderInstancePaths.vertex = json_get_string_new(shaderInstanceJson, "vertex_path");
            shaderInstancePaths.fragment = json_get_string_new(shaderInstanceJson, "fragment_path");
        }
    }
    return shaderInstancePaths;
}

void json_node_set_shader_instance_paths(JsonSceneNode* node, cJSON* componentJson, const char* key) {
    ShaderInstancePaths shaderInstancePaths = json_get_shader_instance_paths(componentJson, key);
    if (shaderInstancePaths.shader) {
        if (node->shaderInstanceShaderPath) {
            SE_MEM_FREE(node->shaderInstanceShaderPath);
        }
        node->shaderInstanceShaderPath = shaderInstancePaths.shader;
    } else if (shaderInstancePaths.vertex && shaderInstancePaths.fragment) {
        if (node->shaderInstanceVertexPath && node->shaderInstanceFragmentPath) {
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
    node->type = NodeBaseType_INVALID;
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
    if (node->components[TRANSFORM2D_COMPONENT_INDEX]) {
        transform2d_component_delete(node->components[TRANSFORM2D_COMPONENT_INDEX]);
    }
    if (node->components[SPRITE_COMPONENT_INDEX]) {
        sprite_component_delete(node->components[SPRITE_COMPONENT_INDEX]);
    }
    if (node->components[ANIMATED_SPRITE_COMPONENT_INDEX]) {
        animated_sprite_component_data_delete(node->components[ANIMATED_SPRITE_COMPONENT_INDEX]);
    }
    if (node->components[TEXT_LABEL_COMPONENT_INDEX]) {
        text_label_component_delete(node->components[TEXT_LABEL_COMPONENT_INDEX]);
    }
    if (node->components[SCRIPT_COMPONENT_INDEX]) {
        script_component_delete(node->components[SCRIPT_COMPONENT_INDEX]);
    }
    if (node->components[COLLIDER2D_COMPONENT_INDEX]) {
        collider2d_component_delete(node->components[COLLIDER2D_COMPONENT_INDEX]);
    }
    if (node->components[COLOR_RECT_COMPONENT_INDEX]) {
        color_rect_component_delete(node->components[COLOR_RECT_COMPONENT_INDEX]);
    }
    if (node->components[PARALLAX_COMPONENT_INDEX]) {
        parallax_component_delete(node->components[PARALLAX_COMPONENT_INDEX]);
    }
    if (node->components[PARTICLES2D_COMPONENT_INDEX]) {
        particles2d_component_delete(node->components[PARTICLES2D_COMPONENT_INDEX]);
    }

    if (node->shaderInstanceShaderPath) {
        SE_MEM_FREE(node->shaderInstanceShaderPath);
    }
    if (node->shaderInstanceVertexPath) {
        SE_MEM_FREE(node->shaderInstanceVertexPath);
    }
    if (node->shaderInstanceFragmentPath) {
        SE_MEM_FREE(node->shaderInstanceFragmentPath);
    }

    // String Arrays
    SE_MEM_FREE(node->name);
    if (node->fontUID) {
        SE_MEM_FREE(node->fontUID);
    }
    if (node->spriteTexturePath) {
        SE_MEM_FREE(node->spriteTexturePath);
    }
}

//--- Private functions ---//

// Component functions
static void cre_json_transform2d_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    Transform2DComponent* transform2DComponent = NULL;
    if (node->components[TRANSFORM2D_COMPONENT_INDEX] == NULL) {
        transform2DComponent = transform2d_component_create();
        transform2DComponent->localTransform.position = json_get_vec2_default(componentJson, "position", DEFAULT_COMPONENT_TRANSFORM2D_POSITION);
        transform2DComponent->localTransform.scale = json_get_vec2_default(componentJson, "scale", DEFAULT_COMPONENT_TRANSFORM2D_SCALE);
        transform2DComponent->localTransform.rotation = (float) json_get_double_default(componentJson, "rotation", DEFAULT_COMPONENT_TRANSFORM2D_ROTATION);
        transform2DComponent->zIndex = json_get_int_default(componentJson, "z_index", DEFAULT_COMPONENT_TRANSFORM2D_Z_INDEX);
        transform2DComponent->isZIndexRelativeToParent = json_get_bool_default(componentJson, "z_index_relative_to_parent", DEFAULT_COMPONENT_TRANSFORM2D_Z_INDEX_RELATIVE_TO_PARENT);
        transform2DComponent->ignoreCamera = json_get_bool_default(componentJson, "ignore_camera", DEFAULT_COMPONENT_TRANSFORM2D_IGNORE_CAMERA);
        node->components[TRANSFORM2D_COMPONENT_INDEX] = transform2DComponent;
    } else {
        transform2DComponent = (Transform2DComponent*) node->components[TRANSFORM2D_COMPONENT_INDEX];
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

static void cre_json_sprite_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    SpriteComponent* spriteComponent = NULL;
    if (node->components[SPRITE_COMPONENT_INDEX] == NULL) {
        spriteComponent = sprite_component_create();
        node->spriteTexturePath = json_get_string_new(componentJson, "texture_path");
        spriteComponent->drawSource = json_get_rect2(componentJson, "draw_source");
        spriteComponent->origin = json_get_vec2_default(componentJson, "origin", DEFAULT_COMPONENT_SPRITE_ORIGIN);
        spriteComponent->flipH = json_get_bool_default(componentJson, "flip_h", DEFAULT_COMPONENT_SPRITE_FLIP_H);
        spriteComponent->flipV = json_get_bool_default(componentJson, "flip_v", DEFAULT_COMPONENT_SPRITE_FLIP_V);
        spriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", DEFAULT_COMPONENT_SPRITE_MODULATE);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
        node->components[SPRITE_COMPONENT_INDEX] = spriteComponent;
    } else {
        spriteComponent = node->components[SPRITE_COMPONENT_INDEX];
        char* newTexturePath = json_get_string_new_unchecked(componentJson, "texture_path");
        if (newTexturePath != NULL) {
            SE_MEM_FREE(node->spriteTexturePath);
            node->spriteTexturePath = newTexturePath;
        }
        spriteComponent->drawSource = json_get_rect2_default(componentJson, "draw_source", spriteComponent->drawSource);
        spriteComponent->origin = json_get_vec2_default(componentJson, "origin", spriteComponent->origin);
        spriteComponent->flipH = json_get_bool_default(componentJson, "flip_h", spriteComponent->flipH);
        spriteComponent->flipV = json_get_bool_default(componentJson, "flip_v", spriteComponent->flipV);
        spriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", spriteComponent->modulate);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
    }
    se_logger_debug(
        "Sprite\ntexture_path: '%s'\ndraw_source = (%f, %f, %f, %f)\norigin: (%f, %f)\nflip_h: %s\nflip_v: %s\nmodulate: (%f, %f, %f, %f)",
        node->spriteTexturePath,
        spriteComponent->drawSource.x, spriteComponent->drawSource.y, spriteComponent->drawSource.w,
        spriteComponent->drawSource.h,
        spriteComponent->origin.x, spriteComponent->origin.y,
        se_bool_to_string(spriteComponent->flipH), se_bool_to_string(spriteComponent->flipV),
        spriteComponent->modulate.r, spriteComponent->modulate.g, spriteComponent->modulate.b,
        spriteComponent->modulate.a);
}

static void cre_json_animated_sprite_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    AnimatedSpriteComponentData* animatedSpriteComponent = NULL;
    char* currentAnimationName = NULL;
    if (node->components[ANIMATED_SPRITE_COMPONENT_INDEX] == NULL) {
        animatedSpriteComponent = animated_sprite_component_data_create();
        currentAnimationName = json_get_string_new(componentJson, "current_animation_name");
        animatedSpriteComponent->isPlaying = json_get_bool(componentJson, "is_playing");
        animatedSpriteComponent->origin = json_get_vec2_default(componentJson, "origin", DEFAULT_COMPONENT_ANIMATED_SPRITE_ORIGIN);
        animatedSpriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", DEFAULT_COMPONENT_ANIMATED_SPRITE_MODULATE);
        animatedSpriteComponent->flipH = json_get_bool_default(componentJson, "flip_h", DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_H);
        animatedSpriteComponent->flipV = json_get_bool_default(componentJson, "flip_v", DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_V);
        animatedSpriteComponent->staggerStartAnimationTimes = json_get_bool_default(componentJson, "stagger_start_animation_time", DEFAULT_COMPONENT_ANIMATED_SPRITE_SHOULD_RANDOMLY_STAGGER_START_ANIMATION_TIMES);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
        // Animation (Won't look at local anims for now)
        cJSON* animationsJsonArray = cJSON_GetObjectItemCaseSensitive(componentJson, "animations");
        cJSON* animationJson = NULL;
        cJSON_ArrayForEach(animationJson, animationsJsonArray) {
            AnimationData animation;
            se_strcpy(animation.name, json_get_string(animationJson, "name"));
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
                se_strcpy(frameData.texturePath, json_get_string(frameJson, "texture_path"));
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
        node->components[ANIMATED_SPRITE_COMPONENT_INDEX] = animatedSpriteComponent;
    } else {
        animatedSpriteComponent = (AnimatedSpriteComponentData*) node->components[ANIMATED_SPRITE_COMPONENT_INDEX];
        currentAnimationName = json_get_string_default_new(componentJson, "current_animation_name", animatedSpriteComponent->currentAnimation.name);
        animatedSpriteComponent->isPlaying = json_get_bool_default(componentJson, "is_playing", animatedSpriteComponent->isPlaying);
        animatedSpriteComponent->origin = json_get_vec2_default(componentJson, "origin", animatedSpriteComponent->origin);
        animatedSpriteComponent->modulate = json_get_linear_color_default(componentJson, "modulate", animatedSpriteComponent->modulate);
        animatedSpriteComponent->flipH = json_get_bool_default(componentJson, "flip_h", animatedSpriteComponent->flipH);
        animatedSpriteComponent->flipV = json_get_bool_default(componentJson, "flip_v", animatedSpriteComponent->flipV);
        animatedSpriteComponent->staggerStartAnimationTimes = json_get_bool_default(componentJson, "stagger_start_animation_times", animatedSpriteComponent->staggerStartAnimationTimes);
        json_node_set_shader_instance_paths(node, componentJson, "shader_instance");
    }
    se_logger_debug(
        "Animated Sprite\ncurrent animation name: '%s'\nis playing: %s\norigin: (%f, %f)\nmodulate: (%f, %f, %f, %f)\nflip x: %s\nflip y: %s\nstagger start animation times: %s",
        currentAnimationName, se_bool_to_string(animatedSpriteComponent->isPlaying),
        animatedSpriteComponent->origin.x, animatedSpriteComponent->origin.y,
        animatedSpriteComponent->modulate.r, animatedSpriteComponent->modulate.g,
        animatedSpriteComponent->modulate.b, animatedSpriteComponent->modulate.a,
        se_bool_to_string(animatedSpriteComponent->flipH),
        se_bool_to_string(animatedSpriteComponent->flipV),
        se_bool_to_string(animatedSpriteComponent->staggerStartAnimationTimes));
    if (currentAnimationName != NULL) {
        SE_MEM_FREE(currentAnimationName);
    }
}

static void cre_json_text_label_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    TextLabelComponent* textLabelComponent = NULL;
    if (node->components[TEXT_LABEL_COMPONENT_INDEX] == NULL) {
        textLabelComponent = text_label_component_create();
        node->fontUID = json_get_string_new_unchecked(componentJson, "uid");
        se_strcpy(textLabelComponent->text, json_get_string(componentJson, "text"));
        textLabelComponent->color = json_get_linear_color_default(componentJson, "color", DEFAULT_COMPONENT_TEXT_LABEL_COLOR);
        node->components[TEXT_LABEL_COMPONENT_INDEX] = textLabelComponent;
    } else {
        textLabelComponent = (TextLabelComponent*) node->components[TEXT_LABEL_COMPONENT_INDEX];
        char* newFontUID = json_get_string_new_unchecked(componentJson, "uid");
        if (newFontUID != NULL) {
            SE_MEM_FREE(node->fontUID);
            node->fontUID = newFontUID;
        }
        se_strcpy(textLabelComponent->text, json_get_string_default(componentJson, "text", textLabelComponent->text));
        textLabelComponent->color = json_get_linear_color_default(componentJson, "color", textLabelComponent->color);
    }
    se_logger_debug("Text Label\nuid: '%s'\ntext: '%s'\ncolor: (%f, %f, %f, %f)",
                    node->fontUID,
                    textLabelComponent->text,
                    textLabelComponent->color.r, textLabelComponent->color.g, textLabelComponent->color.b,
                    textLabelComponent->color.a);
}

static void cre_json_collider2d_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    Collider2DComponent* collider2DComponent = NULL;
    if (node->components[COLLIDER2D_COMPONENT_INDEX] == NULL) {
        collider2DComponent = collider2d_component_create();
        collider2DComponent->extents = json_get_size2d(componentJson, "extents");
        collider2DComponent->color = json_get_linear_color_default(componentJson, "color", DEFAULT_COMPONENT_COLLIDER2D_COLOR);
        node->components[COLLIDER2D_COMPONENT_INDEX] = collider2DComponent;
    } else {
        collider2DComponent = (Collider2DComponent*) node->components[COLLIDER2D_COMPONENT_INDEX];
        collider2DComponent->extents = json_get_size2d_default(componentJson, "extents", collider2DComponent->extents);
        collider2DComponent->color = json_get_linear_color_default(componentJson, "color", collider2DComponent->color);
    }
    se_logger_debug("Collider2D\nextents: (%f, %f)\ncolor: (%f, %f, %f, %f)",
                    collider2DComponent->extents.w, collider2DComponent->extents.h,
                    collider2DComponent->color.r, collider2DComponent->color.g, collider2DComponent->color.b,
                    collider2DComponent->color.a);
}

static void cre_json_color_rect_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    ColorRectComponent* colorRectComponent = NULL;
    if (node->components[COLOR_RECT_COMPONENT_INDEX] == NULL) {
        colorRectComponent = color_rect_component_create();
        colorRectComponent->size = json_get_size2d(componentJson, "size");
        colorRectComponent->color = json_get_linear_color_default(componentJson, "color", DEFAULT_COMPONENT_COLOR_RECT_COLOR);
        node->components[COLOR_RECT_COMPONENT_INDEX] = colorRectComponent;
    } else {
        colorRectComponent = (ColorRectComponent*) node->components[COLOR_RECT_COMPONENT_INDEX];
        colorRectComponent->size = json_get_size2d_default(componentJson, "size", colorRectComponent->size);
        colorRectComponent->color = json_get_linear_color_default(componentJson, "color", colorRectComponent->color);
    }
    se_logger_debug("Color Rext\nsize: (%f, %f)\ncolor: (%f, %f, %f, %f)",
                    colorRectComponent->size.w, colorRectComponent->size.h,
                    colorRectComponent->color.r, colorRectComponent->color.g, colorRectComponent->color.b,
                    colorRectComponent->color.a);
}

static void cre_json_script_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    ScriptComponent* scriptComponent = NULL;
    if (node->components[SCRIPT_COMPONENT_INDEX] == NULL) {
        scriptComponent = script_component_create(
                              json_get_string(componentJson, "class_path"),
                              json_get_string(componentJson, "class_name")
                          );
        scriptComponent->contextType = ScriptContextType_PYTHON;
        node->components[SCRIPT_COMPONENT_INDEX] = scriptComponent;
    } else {
        // No override for scripts for now...
        scriptComponent = (ScriptComponent*) node->components[SCRIPT_COMPONENT_INDEX];
    }
    se_logger_debug("Script\nclass path: '%s'\nclass name: '%s'",
                    scriptComponent->classPath, scriptComponent->className);
}

static void cre_json_parallax_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    ParallaxComponent* parallaxComponent = NULL;
    if (node->components[PARALLAX_COMPONENT_INDEX] == NULL) {
        parallaxComponent = parallax_component_create();
        parallaxComponent->scrollSpeed =  json_get_vec2_default(componentJson, "scroll_speed", DEFAULT_COMPONENT_PARALLAX_SCROLL_SPEED);
        node->components[PARALLAX_COMPONENT_INDEX] = parallaxComponent;
    } else {
        parallaxComponent = (ParallaxComponent*) node->components[PARALLAX_COMPONENT_INDEX];
        parallaxComponent->scrollSpeed = json_get_vec2_default(componentJson, "scroll_speed", parallaxComponent->scrollSpeed);
    }
    se_logger_debug("Parallax\nscroll_speed: (%f, %f)", parallaxComponent->scrollSpeed.x, parallaxComponent->scrollSpeed.y);
}

static void cre_json_particles2d_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    Particles2DComponent* particles2dComponent = NULL;
    bool isEmitting = false;
    if (node->components[PARTICLES2D_COMPONENT_INDEX] == NULL) {
        particles2dComponent = particles2d_component_create();
        isEmitting = json_get_bool_default(componentJson, "is_emitting", true);
        particles2dComponent->amount = json_get_int_default(componentJson, "amount", DEFAULT_COMPONENT_PARTICLES2D_AMOUNT);
        particles2dComponent->initialVelocity = json_get_minmax_vec2_default(componentJson, "initial_velocity", SKA_MINMAX_VEC2_ZERO);
        particles2dComponent->color = json_get_linear_color_default(componentJson, "color", SKA_COLOR_WHITE);
        particles2dComponent->spread = (float)json_get_double_default(componentJson, "spread", (double)DEFAULT_COMPONENT_PARTICLES2D_SPREAD);
        particles2dComponent->lifeTime = (float)json_get_double_default(componentJson, "life_time", (double)DEFAULT_COMPONENT_PARTICLES2D_LIFE_TIME);
        particles2dComponent->damping = (float)json_get_double_default(componentJson, "damping", (double)DEFAULT_COMPONENT_PARTICLES2D_DAMPING);
        particles2dComponent->explosiveness = (float)json_get_double_default(componentJson, "explosiveness", (double)DEFAULT_COMPONENT_PARTICLES2D_EXPLOSIVENESS);
        node->components[PARTICLES2D_COMPONENT_INDEX] = particles2dComponent;
    } else {
        particles2dComponent = (Particles2DComponent*) node->components[PARTICLES2D_COMPONENT_INDEX];
        isEmitting = json_get_bool_default(componentJson, "is_emitting", particles2dComponent->state != Particle2DComponentState_INACTIVE);
        particles2dComponent->amount = json_get_int_default(componentJson, "amount", particles2dComponent->amount);
        particles2dComponent->initialVelocity = json_get_minmax_vec2_default(componentJson, "initial_velocity", particles2dComponent->initialVelocity);
        particles2dComponent->color = json_get_linear_color_default(componentJson, "color", particles2dComponent->color);
        particles2dComponent->spread = (float)json_get_double_default(componentJson, "spread", (double)particles2dComponent->spread);
        particles2dComponent->lifeTime = (float)json_get_double_default(componentJson, "life_time", (double)particles2dComponent->lifeTime);
        particles2dComponent->damping = (float)json_get_double_default(componentJson, "damping", (double)particles2dComponent->damping);
        particles2dComponent->explosiveness = (float)json_get_double_default(componentJson, "explosiveness", (double)particles2dComponent->explosiveness);
    }
    particles2dComponent->state = isEmitting ? Particle2DComponentState_WAITING_TO_INITIALIZE : Particle2DComponentState_INACTIVE;
    se_logger_debug("Particles2D\namount: %d", particles2dComponent->amount);
}

static void cre_json_tilemap_create_or_set_default(JsonSceneNode* node, cJSON* componentJson) {
    TilemapComponent* tilemapComponent = NULL;
    if (node->components[TILEMAP_COMPONENT_INDEX] == NULL) {
        tilemapComponent = tilemap_component_create();
        node->spriteTexturePath = json_get_string_new(componentJson, "texture_path");
        tilemapComponent->tilemap->tileset.tileSize = json_get_size2di(componentJson, "tile_size");
        node->components[TILEMAP_COMPONENT_INDEX] = tilemapComponent;
    } else {
        tilemapComponent = (TilemapComponent*)node->components[TILEMAP_COMPONENT_INDEX];
        char* tilemapTexturePath = json_get_string_new_unchecked(componentJson, "texture_path");
        if (tilemapTexturePath) {
            SE_MEM_FREE(node->spriteTexturePath);
            node->spriteTexturePath = tilemapTexturePath;
        }
        tilemapComponent->tilemap->tileset.tileSize = json_get_size2di_default(componentJson, "tile_size", tilemapComponent->tilemap->tileset.tileSize);
    }
    se_logger_debug("Tilemap");
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
        } else if (strcmp(componentType, "particles2d") == 0) {
            cre_json_particles2d_create_or_set_default(node, componentJson);
        } else if (strcmp(componentType, "tilemap") == 0) {
            cre_json_tilemap_create_or_set_default(node, componentJson);
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
