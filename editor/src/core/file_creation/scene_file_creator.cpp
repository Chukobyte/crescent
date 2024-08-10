#include "scene_file_creator.h"

#include "../engine/src/core/ecs/components/component_defaults.h"

#include "../scene/scene_manager.h"
#include "../utils/json_helper.h"

namespace {
nlohmann::ordered_json GetComponentsJsonArray(SceneNode* sceneNode) {
    nlohmann::ordered_json componentsJsonArray = nlohmann::ordered_json::array();
    if (const auto transform2DComp = sceneNode->GetComponentSafe<Transform2DComp>()) {
        nlohmann::ordered_json transform2dJson;
        transform2dJson["type"] = "transform_2d";
        if (transform2DComp->transform2D.position.x != 0.0f || transform2DComp->transform2D.position.y != 0.0f) {
            transform2dJson["position"] = JsonHelper::Vector2ToJson(transform2DComp->transform2D.position);
        }
        if (transform2DComp->transform2D.scale.x != 1.0f || transform2DComp->transform2D.scale.y != 1.0f) {
            transform2dJson["scale"] = JsonHelper::Vector2ToJson(transform2DComp->transform2D.scale);
        }
        if (transform2DComp->transform2D.rotation != 0.0f) {
            transform2dJson["rotation"] = transform2DComp->transform2D.rotation;
        }
        if (transform2DComp->zIndex != 0) {
            transform2dJson["z_index"] = transform2DComp->zIndex;
        }
        if (transform2DComp->isZIndexRelativeToParent != DEFAULT_COMPONENT_TRANSFORM2D_Z_INDEX_RELATIVE_TO_PARENT) {
            transform2dJson["z_index_relative_to_parent"] = transform2DComp->isZIndexRelativeToParent;
        }
        if (transform2DComp->ignoreCamera != DEFAULT_COMPONENT_TRANSFORM2D_IGNORE_CAMERA) {
            transform2dJson["ignore_camera"] = transform2DComp->ignoreCamera;
        }
        componentsJsonArray.emplace_back(transform2dJson);
    }
    if (const auto spriteComp = sceneNode->GetComponentSafe<SpriteComp>()) {
        nlohmann::ordered_json spriteJson;
        spriteJson["type"] = "sprite";
        spriteJson["texture_path"] = spriteComp->texturePath;
        spriteJson["draw_source"] = JsonHelper::Rect2ToJson(spriteComp->drawSource);
        if (spriteComp->origin.x != 0.0f || spriteComp->origin.y != 0.0f) {
            spriteJson["origin"] = JsonHelper::Vector2ToJson(spriteComp->origin);
        }
        if (spriteComp->modulate.r != 1.0f || spriteComp->modulate.g != 1.0f || spriteComp->modulate.b != 1.0f || spriteComp->modulate.a != 1.0f) {
            spriteJson["modulate"] = JsonHelper::ColorToJson(spriteComp->modulate);
        }
        if (spriteComp->flipH != DEFAULT_COMPONENT_SPRITE_FLIP_H) {
            spriteJson["flip_h"] = spriteComp->flipH;
        }
        if (spriteComp->flipV != DEFAULT_COMPONENT_SPRITE_FLIP_V) {
            spriteJson["flip_v"] = spriteComp->flipV;
        }
        if (!spriteComp->shaderPath.empty()) {
            spriteJson["shader_instance"]["shader_path"] = spriteComp->shaderPath;
        }
        componentsJsonArray.emplace_back(spriteJson);
    }
    if (const auto animatedSpriteComp = sceneNode->GetComponentSafe<AnimatedSpriteComp>()) {
        nlohmann::ordered_json animSpriteJson;
        animSpriteJson["type"] = "animated_sprite";
        animSpriteJson["current_animation_name"] = animatedSpriteComp->currentAnimationName;
        animSpriteJson["is_playing"] = animatedSpriteComp->isPlaying;
        if (animatedSpriteComp->origin.x != 0.0f || animatedSpriteComp->origin.y != 0.0f) {
            animSpriteJson["origin"] = JsonHelper::Vector2ToJson(animatedSpriteComp->origin);
        }
        if (animatedSpriteComp->modulate.r != 1.0f || animatedSpriteComp->modulate.g != 1.0f || animatedSpriteComp->modulate.b != 1.0f || animatedSpriteComp->modulate.a != 1.0f) {
            animSpriteJson["modulate"] = JsonHelper::ColorToJson(animatedSpriteComp->modulate);
        }
        if (animatedSpriteComp->flipH != DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_H) {
            animSpriteJson["flip_h"] = animatedSpriteComp->flipH;
        }
        if (animatedSpriteComp->flipV != DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_V) {
            animSpriteJson["flip_v"] = animatedSpriteComp->flipV;
        }
        if (animatedSpriteComp->staggerStartAnimationTimes != DEFAULT_COMPONENT_ANIMATED_SPRITE_SHOULD_RANDOMLY_STAGGER_START_ANIMATION_TIMES) {
            animSpriteJson["stagger_start_animation_times"] = animatedSpriteComp->staggerStartAnimationTimes;
        }

        if (!animatedSpriteComp->shaderPath.empty()) {
            animSpriteJson["shader_instance"]["shader_path"] = animatedSpriteComp->shaderPath;
        }
        // Animations
        nlohmann::ordered_json animationsJsonArray = nlohmann::ordered_json::array();
        for (const auto& animation : animatedSpriteComp->animations) {
            nlohmann::ordered_json animJson;
            animJson["name"] = animation.name;
            animJson["speed"] = animation.speed;
            animJson["loops"] = animation.doesLoop;
            // Frames
            nlohmann::ordered_json framesJsonArray = nlohmann::ordered_json::array();
            for (const auto& frame : animation.animationFrames) {
                nlohmann::ordered_json frameJson;
                frameJson["frame"] = frame.frame;
                frameJson["texture_path"] = frame.texturePath;
                frameJson["draw_source"] = JsonHelper::Rect2ToJson(frame.drawSource);
                framesJsonArray.emplace_back(frameJson);
            }
            animJson["frames"] = framesJsonArray;
            animationsJsonArray.emplace_back(animJson);
        }
        animSpriteJson["animations"] = animationsJsonArray;
        componentsJsonArray.emplace_back(animSpriteJson);
    }
    if (const auto textLabelComp = sceneNode->GetComponentSafe<TextLabelComp>()) {
        nlohmann::ordered_json textLabelJson;
        textLabelJson["type"] = "text_label";
        if (!textLabelComp->fontUID.empty()) {
            textLabelJson["uid"] = textLabelComp->fontUID;
        }
        textLabelJson["text"] = textLabelComp->text;
        if (textLabelComp->color.r != 1.0f || textLabelComp->color.g != 1.0f || textLabelComp->color.b != 1.0f || textLabelComp->color.a != 1.0f) {
            textLabelJson["color"] = JsonHelper::ColorToJson(textLabelComp->color);
        }
        componentsJsonArray.emplace_back(textLabelJson);
    }
    if (const auto scriptComp = sceneNode->GetComponentSafe<ScriptComp>()) {
        nlohmann::ordered_json scriptJson;
        scriptJson["type"] = "script";
        scriptJson["class_path"] = scriptComp->classPath;
        scriptJson["class_name"] = scriptComp->className;
        componentsJsonArray.emplace_back(scriptJson);
    }
    if (const auto collider2DComp = sceneNode->GetComponentSafe<Collider2DComp>()) {
        nlohmann::ordered_json collider2DJson;
        collider2DJson["type"] = "collider_2d";
        collider2DJson["extents"] = JsonHelper::Size2DToJson(collider2DComp->extents);
        if (collider2DComp->color.r != 1.0f || collider2DComp->color.g != 1.0f || collider2DComp->color.b != 1.0f || collider2DComp->color.a != 1.0f) {
            collider2DJson["color"] = JsonHelper::ColorToJson(collider2DComp->color);
        }
        componentsJsonArray.emplace_back(collider2DJson);
    }
    if (const auto colorRectComp = sceneNode->GetComponentSafe<ColorRectComp>()) {
        nlohmann::ordered_json colorRectJson;
        colorRectJson["type"] = "color_rect";
        colorRectJson["size"] = JsonHelper::Size2DToJson(colorRectComp->size);
        if (colorRectComp->color.r != 1.0f || colorRectComp->color.g != 1.0f || colorRectComp->color.b != 1.0f || colorRectComp->color.a != 1.0f) {
            colorRectJson["color"] = JsonHelper::ColorToJson(colorRectComp->color);
        }
        componentsJsonArray.emplace_back(colorRectJson);
    }
    if (const auto parallaxComp = sceneNode->GetComponentSafe<ParallaxComp>()) {
        nlohmann::ordered_json parallaxJson;
        parallaxJson["type"] = "parallax";
        if (parallaxComp->scrollSpeed.x != 0.0f || parallaxComp->scrollSpeed.y != 0.0f) {
            parallaxJson["scroll_speed"] = JsonHelper::Vector2ToJson(parallaxComp->scrollSpeed);
        }
        componentsJsonArray.emplace_back(parallaxJson);
    }
    if (const auto particles2DComp = sceneNode->GetComponentSafe<Particles2DComp>()) {
        nlohmann::ordered_json particles2DJson;
        particles2DJson["type"] = "particles2d";
        if (!particles2DComp->isEmitting) {
            particles2DJson["is_emitting"] = particles2DComp->isEmitting;
        }
        if (particles2DComp->amount != 8) {
            particles2DJson["amount"] = particles2DComp->amount;
        }
        if (particles2DComp->initialVelocity.min.x != 0.0f || particles2DComp->initialVelocity.min.y != 0.0f || particles2DComp->initialVelocity.max.x != 0.0f || particles2DComp->initialVelocity.max.y != 0.0f) {
            particles2DJson["initial_velocity"] = JsonHelper::MinMaxVector2ToJson(particles2DComp->initialVelocity);
        }
        if (particles2DComp->color.r != 1.0f || particles2DComp->color.g != 1.0f || particles2DComp->color.b != 1.0f || particles2DComp->color.a != 1.0f) {
            particles2DJson["color"] = JsonHelper::ColorToJson(particles2DComp->color);
        }
        if (particles2DComp->spread != 45.0f) {
            particles2DJson["spread"] = particles2DComp->spread;
        }
        if (particles2DComp->lifeTime != 4.0f) {
            particles2DJson["life_time"] = particles2DComp->lifeTime;
        }
        if (particles2DComp->damping != 1.0f) {
            particles2DJson["damping"] = particles2DComp->damping;
        }
        if (particles2DComp->explosiveness != 0.0f) {
            particles2DJson["explosiveness"] = particles2DComp->explosiveness;
        }
        componentsJsonArray.emplace_back(particles2DJson);
    }
    if (const auto tilemapComp = sceneNode->GetComponentSafe<TilemapComp>()) {
        nlohmann::ordered_json tilemapJson;
        tilemapJson["type"] = "tilemap";
        const char* tilemapTexturePath = "TODO: Add!";
        tilemapJson["texture_path"] = tilemapTexturePath;
        const auto& tileSize = tilemapComp->GetTileSize();
        if (tileSize.w != 32 || tileSize.h != 32) {
            tilemapJson["tile_size"] = JsonHelper::Size2DToJson(tileSize);
        }
        nlohmann::ordered_json activeTilesJsonArray = nlohmann::ordered_json::array();
        tilemapComp->ForEachActiveTile([&activeTilesJsonArray](const CreTileData* tileData) {
            nlohmann::ordered_json tileDataJson;
            tileDataJson["position"]["x"] = tileData->position.x;
            tileDataJson["position"]["y"] = tileData->position.y;
            tileDataJson["texture_coord"]["x"] = tileData->renderCoords.x;
            tileDataJson["texture_coord"]["y"] = tileData->renderCoords.y;
            activeTilesJsonArray.emplace_back(tileDataJson);
        });
        tilemapJson["active_tiles"] = activeTilesJsonArray;
        componentsJsonArray.emplace_back(tilemapJson);
    }
    return componentsJsonArray;
}

nlohmann::ordered_json GetSceneNodeJson(SceneNode* sceneNode) {
    nlohmann::ordered_json sceneJson;
    sceneJson["name"] = sceneNode->name;
    sceneJson["type"] = sceneNode->GetTypeString();
    if (!sceneNode->tags.empty()) {
        // TODO: Create tag array
        sceneJson["tags"] = nullptr;
    }
    if (sceneNode->IsExternalSceneNode()) {
        sceneJson["external_node_source"] = sceneNode->externalNodeSource;
    }
    //Components
    sceneJson["components"] = GetComponentsJsonArray(sceneNode);
    // Children
    nlohmann::ordered_json childrenJsonArray = nlohmann::ordered_json::array();
    for (const auto& childNode : sceneNode->children) {
        if (!childNode->doesOriginateFromExternalScene || childNode->IsExternalSceneNode()) {
            const nlohmann::ordered_json& childSceneNode = GetSceneNodeJson(childNode);
            childrenJsonArray.emplace_back(childSceneNode);
        }
    }
    sceneJson["children"] = childrenJsonArray;
    return sceneJson;
}
} // namespace

void SceneFileCreator::GenerateSceneFile(SceneNodeFile *nodeFile, const char *filePath) {
    nlohmann::ordered_json sceneJson = GetSceneNodeJson(nodeFile->rootNode);
    JsonHelper::SaveFile(filePath, sceneJson);
}
