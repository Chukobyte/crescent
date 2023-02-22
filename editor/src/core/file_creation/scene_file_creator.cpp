#include "scene_file_creator.h"

#include "../engine/src/core/ecs/component/component_defaults.h"

#include "../scene/scene_manager.h"
#include "../utils/json_helper.h"

namespace {
nlohmann::ordered_json Vector2ToJson(Vector2 value) {
    nlohmann::ordered_json vec;
    vec["x"] = value.x;
    vec["y"] = value.y;
    return vec;
}

nlohmann::ordered_json Size2DToJson(Size2D value) {
    nlohmann::ordered_json size;
    size["w"] = value.w;
    size["h"] = value.h;
    return size;
}

nlohmann::ordered_json Rect2ToJson(const Rect2& value) {
    nlohmann::ordered_json rect;
    rect["x"] = value.x;
    rect["y"] = value.y;
    rect["w"] = value.w;
    rect["h"] = value.h;
    return rect;
}

nlohmann::ordered_json ColorToJson(const Color& value) {
    nlohmann::ordered_json rect;
    rect["r"] = (int) (value.r * 255);
    rect["g"] = (int) (value.g * 255);
    rect["b"] = (int) (value.b * 255);
    rect["a"] = (int) (value.a * 255);
    return rect;
}

nlohmann::ordered_json GetComponentsJsonArray(SceneNode* sceneNode) {
    nlohmann::ordered_json componentsJsonArray = nlohmann::ordered_json::array();
    if (const Transform2DComp* transform2DComp = sceneNode->GetComponentSafe<Transform2DComp>()) {
        nlohmann::ordered_json transform2dJson;
        transform2dJson["type"] = "transform_2d";
        if (transform2DComp->transform2D.position.x != 0.0f || transform2DComp->transform2D.position.y != 0.0f) {
            transform2dJson["position"] = Vector2ToJson(transform2DComp->transform2D.position);
        }
        if (transform2DComp->transform2D.scale.x != 1.0f || transform2DComp->transform2D.scale.y != 1.0f) {
            transform2dJson["scale"] = Vector2ToJson(transform2DComp->transform2D.scale);
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
    if (const SpriteComp* spriteComp = sceneNode->GetComponentSafe<SpriteComp>()) {
        nlohmann::ordered_json spriteJson;
        spriteJson["type"] = "sprite";
        spriteJson["texture_path"] = spriteComp->texturePath;
        spriteJson["draw_source"] = Rect2ToJson(spriteComp->drawSource);
        if (spriteComp->origin.x != 0.0f || spriteComp->origin.y != 0.0f) {
            spriteJson["origin"] = Vector2ToJson(spriteComp->origin);
        }
        if (spriteComp->modulate.r != 1.0f || spriteComp->modulate.g != 1.0f || spriteComp->modulate.b != 1.0f || spriteComp->modulate.a != 1.0f) {
            spriteJson["modulate"] = ColorToJson(spriteComp->modulate);
        }
        if (spriteComp->flipX != DEFAULT_COMPONENT_SPRITE_FLIP_X) {
            spriteJson["flip_x"] = spriteComp->flipX;
        }
        if (spriteComp->flipY != DEFAULT_COMPONENT_SPRITE_FLIP_Y) {
            spriteJson["flip_y"] = spriteComp->flipY;
        }
        if (!spriteComp->vertexShaderPath.empty() && !spriteComp->fragmentShaderPath.empty()) {
            spriteJson["shader_instance"]["vertex_path"] = spriteComp->vertexShaderPath;
            spriteJson["shader_instance"]["fragment_path"] = spriteComp->fragmentShaderPath;
        }
        componentsJsonArray.emplace_back(spriteJson);
    }
    if (const AnimatedSpriteComp* animatedSpriteComp = sceneNode->GetComponentSafe<AnimatedSpriteComp>()) {
        nlohmann::ordered_json animSpriteJson;
        animSpriteJson["type"] = "animated_sprite";
        animSpriteJson["current_animation_name"] = animatedSpriteComp->currentAnimationName;
        animSpriteJson["is_playing"] = animatedSpriteComp->isPlaying;
        if (animatedSpriteComp->origin.x != 0.0f || animatedSpriteComp->origin.y != 0.0f) {
            animSpriteJson["origin"] = Vector2ToJson(animatedSpriteComp->origin);
        }
        if (animatedSpriteComp->modulate.r != 1.0f || animatedSpriteComp->modulate.g != 1.0f || animatedSpriteComp->modulate.b != 1.0f || animatedSpriteComp->modulate.a != 1.0f) {
            animSpriteJson["modulate"] = ColorToJson(animatedSpriteComp->modulate);
        }
        if (animatedSpriteComp->flipX != DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_X) {
            animSpriteJson["flip_x"] = animatedSpriteComp->flipX;
        }
        if (animatedSpriteComp->flipY != DEFAULT_COMPONENT_ANIMATED_SPRITE_FLIP_Y) {
            animSpriteJson["flip_y"] = animatedSpriteComp->flipY;
        }
        if (!animatedSpriteComp->vertexShaderPath.empty() && !animatedSpriteComp->fragmentShaderPath.empty()) {
            animSpriteJson["shader_instance"]["vertex_path"] = animatedSpriteComp->vertexShaderPath;
            animSpriteJson["shader_instance"]["fragment_path"] = animatedSpriteComp->fragmentShaderPath;
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
                frameJson["draw_source"] = Rect2ToJson(frame.drawSource);
                framesJsonArray.emplace_back(frameJson);
            }
            animJson["frames"] = framesJsonArray;
            animationsJsonArray.emplace_back(animJson);
        }
        animSpriteJson["animations"] = animationsJsonArray;
        componentsJsonArray.emplace_back(animSpriteJson);
    }
    if (const TextLabelComp* textLabelComp = sceneNode->GetComponentSafe<TextLabelComp>()) {
        nlohmann::ordered_json textLabelJson;
        textLabelJson["type"] = "text_label";
        textLabelJson["uid"] = textLabelComp->fontUID;
        textLabelJson["text"] = textLabelComp->text;
        if (textLabelComp->color.r != 1.0f || textLabelComp->color.g != 1.0f || textLabelComp->color.b != 1.0f || textLabelComp->color.a != 1.0f) {
            textLabelJson["color"] = ColorToJson(textLabelComp->color);
        }
        componentsJsonArray.emplace_back(textLabelJson);
    }
    if (const ScriptComp* scriptComp = sceneNode->GetComponentSafe<ScriptComp>()) {
        nlohmann::ordered_json scriptJson;
        scriptJson["type"] = "script";
        scriptJson["class_path"] = scriptComp->classPath;
        scriptJson["class_name"] = scriptComp->className;
        componentsJsonArray.emplace_back(scriptJson);
    }
    if (const Collider2DComp* collider2DComp = sceneNode->GetComponentSafe<Collider2DComp>()) {
        nlohmann::ordered_json collider2DJson;
        collider2DJson["type"] = "collider_2d";
        collider2DJson["extents"] = Size2DToJson(collider2DComp->extents);
        if (collider2DComp->color.r != 1.0f || collider2DComp->color.g != 1.0f || collider2DComp->color.b != 1.0f || collider2DComp->color.a != 1.0f) {
            collider2DJson["color"] = ColorToJson(collider2DComp->color);
        }
        componentsJsonArray.emplace_back(collider2DJson);
    }
    if (const ColorRectComp* colorRectComp = sceneNode->GetComponentSafe<ColorRectComp>()) {
        nlohmann::ordered_json colorRectJson;
        colorRectJson["type"] = "color_rect";
        colorRectJson["size"] = Size2DToJson(colorRectComp->size);
        if (colorRectComp->color.r != 1.0f || colorRectComp->color.g != 1.0f || colorRectComp->color.b != 1.0f || colorRectComp->color.a != 1.0f) {
            colorRectJson["color"] = ColorToJson(colorRectComp->color);
        }
        componentsJsonArray.emplace_back(colorRectJson);
    }
    if (const ParallaxComp* parallaxComp = sceneNode->GetComponentSafe<ParallaxComp>()) {
        nlohmann::ordered_json parallaxJson;
        parallaxJson["type"] = "parallax";
        if (parallaxComp->scrollSpeed.x != 0.0f || parallaxComp->scrollSpeed.y != 0.0f) {
            parallaxJson["scroll_speed"] = Vector2ToJson(parallaxComp->scrollSpeed);
        }
        componentsJsonArray.emplace_back(parallaxJson);
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
