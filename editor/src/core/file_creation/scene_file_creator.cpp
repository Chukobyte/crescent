#include "scene_file_creator.h"

#include "../scene/scene_manager.h"
#include "../utils/file_system_helper.h"
#include "../utils/helper.h"

namespace {
template<typename T> std::string ToString(const T& t) {
    std::ostringstream os;
    os << t;
    return os.str();
}

std::string Vector2ToString(const Vector2& v) {
    return "Vector2(" + ToString(v.x) + ", " + ToString(v.y) + ")";
}

std::string Size2DToString(const Size2D& s) {
    return "Size2D(" + ToString(s.w) + ", " + ToString(s.h) + ")";
}

std::string Rect2ToString(const Rect2& r) {
    return "Rect2(" + ToString(r.x) + ", " + ToString(r.y) + ", " + ToString(r.w) + ", " + ToString(r.h) + ")";
}

std::string ColorToString(const Color& c) {
    return "Color(" + ToString((int) (c.r * 255.0f)) + ", " + ToString((int) (c.g * 255.0f)) + ", " + ToString((int) (c.b * 255.0f)) + ", " + ToString((int) (c.a * 255.0f)) + ")";
}

std::string GetStageAnimationText(const EditorAnimation& animation, int tabSpace) {
    const std::string tabSpaceText = std::string(tabSpace, ' ');
    std::string fileContents;

    for (const auto& animFrame : animation.animationFrames) {
        fileContents += tabSpaceText + "                    AnimationFrame(\n";
        fileContents += tabSpaceText + "                        frame=" + ToString(animFrame.frame) + ",\n";
        fileContents += tabSpaceText + "                        texture_path=\"" + animFrame.texturePath + "\",\n";
        fileContents += tabSpaceText + "                        draw_source=" + Rect2ToString(animFrame.drawSource) + ",\n";
        fileContents += tabSpaceText + "                    ),\n";
    }

    return fileContents;
}

std::string GetStageAnimatedSpriteComponentText(const AnimatedSpriteComp* animatedSpriteComp, int tabSpace) {
    const std::string tabSpaceText = std::string(tabSpace, ' ');
    std::string fileContents;

    for (const auto& anim : animatedSpriteComp->animations) {
        fileContents += tabSpaceText + "                    Animation(\n";
        fileContents += tabSpaceText + "                        name=\"" + anim.name + "\",\n";
        fileContents += tabSpaceText + "                        speed=" + ToString(anim.speed) + ",\n";
        fileContents += tabSpaceText + "                        loops=" + Helper::BoolToStringCapital(anim.doesLoop) + ",\n";

        if (anim.animationFrames.empty()) {
            fileContents += tabSpaceText + "                        frames=[],\n";
        } else {
            fileContents += tabSpaceText + "                        frames=[\n";
            fileContents += GetStageAnimationText(anim, tabSpace + 8);
            fileContents += tabSpaceText + "                        ],\n";
        }

        fileContents += tabSpaceText + "                    ),\n";
    }

    return fileContents;
}

std::string GetStageNodeComponentsText(SceneNode* sceneNode, int tabSpace) {
    if (!sceneNode->HasComponents()) {
        return "[]";
    }
    const std::string tabSpaceText = std::string(tabSpace, ' ');
    std::string fileContents;

    if (const Transform2DComp* transform2DComp = sceneNode->GetComponentSafe<Transform2DComp>()) {
        fileContents += tabSpaceText + "                Transform2DComponent(\n";
        fileContents += tabSpaceText + "                    position=" + Vector2ToString(transform2DComp->transform2D.position) + ",\n";
        fileContents += tabSpaceText + "                    scale=" + Vector2ToString(transform2DComp->transform2D.scale) + ",\n";
        fileContents += tabSpaceText + "                    rotation=" + ToString(transform2DComp->transform2D.rotation) + ",\n";
        fileContents += tabSpaceText + "                    z_index=" + ToString(transform2DComp->zIndex) + ",\n";
        fileContents += tabSpaceText + "                    z_index_relative_to_parent=" + Helper::BoolToStringCapital(transform2DComp->isZIndexRelativeToParent) + ",\n";
        fileContents += tabSpaceText + "                    ignore_camera=" + Helper::BoolToStringCapital(transform2DComp->ignoreCamera) + ",\n";
        fileContents += tabSpaceText + "                ),\n";
    }
    if (const SpriteComp* spriteComp = sceneNode->GetComponentSafe<SpriteComp>()) {
        fileContents += tabSpaceText + "                SpriteComponent(\n";
        fileContents += tabSpaceText + "                    texture_path=\"" + spriteComp->texturePath + "\",\n";
        fileContents += tabSpaceText + "                    draw_source=" + Rect2ToString(spriteComp->drawSource) + ",\n";
        fileContents += tabSpaceText + "                    origin=" + Vector2ToString(spriteComp->origin) + ",\n";
        fileContents += tabSpaceText + "                    flip_x=" + Helper::BoolToStringCapital(spriteComp->flipX) + ",\n";
        fileContents += tabSpaceText + "                    flip_y=" + Helper::BoolToStringCapital(spriteComp->flipY) + ",\n";
        fileContents += tabSpaceText + "                    modulate=" + ColorToString(spriteComp->modulate) + ",\n";
        fileContents += tabSpaceText + "                ),\n";
    }
    if (const AnimatedSpriteComp* animatedSpriteComp = sceneNode->GetComponentSafe<AnimatedSpriteComp>()) {
        fileContents += tabSpaceText + "                AnimatedSpriteComponent(\n";
        fileContents += tabSpaceText + "                    current_animation_name=\"" + animatedSpriteComp->currentAnimationName + "\",\n";
        fileContents += tabSpaceText + "                    is_playing=" + Helper::BoolToStringCapital(animatedSpriteComp->isPlaying) + ",\n";
        fileContents += tabSpaceText + "                    origin=" + Vector2ToString(animatedSpriteComp->origin) + ",\n";
        fileContents += tabSpaceText + "                    flip_x=" + Helper::BoolToStringCapital(animatedSpriteComp->flipX) + ",\n";
        fileContents += tabSpaceText + "                    flip_y=" + Helper::BoolToStringCapital(animatedSpriteComp->flipY) + ",\n";
        fileContents += tabSpaceText + "                    modulate=" + ColorToString(animatedSpriteComp->modulate) + ",\n";

        if (animatedSpriteComp->animations.empty()) {
            fileContents += tabSpaceText + "                    animations=[],\n";
        } else {
            fileContents += tabSpaceText + "                    animations=[\n";
            fileContents += GetStageAnimatedSpriteComponentText(animatedSpriteComp, tabSpace + 4);
            fileContents += tabSpaceText + "                    ],\n";
        }

        fileContents += tabSpaceText + "                ),\n";
    }
    if (const TextLabelComp* textLabelComp = sceneNode->GetComponentSafe<TextLabelComp>()) {
        fileContents += tabSpaceText + "                TextLabelComponent(\n";
        fileContents += tabSpaceText + "                    uid=\"" + textLabelComp->fontUID + "\",\n";
        fileContents += tabSpaceText + "                    text=\"" + textLabelComp->text + "\",\n";
        fileContents += tabSpaceText + "                    color=" + ColorToString(textLabelComp->color) + ",\n";
        fileContents += tabSpaceText + "                ),\n";
    }
    if (const ScriptComp* scriptComp = sceneNode->GetComponentSafe<ScriptComp>()) {
        fileContents += tabSpaceText + "                ScriptComponent(\n";
        fileContents += tabSpaceText + "                    class_path=\"" + scriptComp->classPath + "\",\n";
        fileContents += tabSpaceText + "                    class_name=\"" + scriptComp->className + "\",\n";
        fileContents += tabSpaceText + "                ),\n";
    }
    if (const Collider2DComp* collider2DComp = sceneNode->GetComponentSafe<Collider2DComp>()) {
        fileContents += tabSpaceText + "                Collider2DComponent(\n";
        fileContents += tabSpaceText + "                    extents=" + Size2DToString(collider2DComp->extents) + ",\n";
        fileContents += tabSpaceText + "                    color=" + ColorToString(collider2DComp->color) + ",\n";
        fileContents += tabSpaceText + "                ),\n";
    }
    if (const ColorRectComp* colorRectComp = sceneNode->GetComponentSafe<ColorRectComp>()) {
        fileContents += tabSpaceText + "                ColorRectComponent(\n";
        fileContents += tabSpaceText + "                    size=" + Size2DToString(colorRectComp->size) + ",\n";
        fileContents += tabSpaceText + "                    color=" + ColorToString(colorRectComp->color) + ",\n";
        fileContents += tabSpaceText + "                ),\n";
    }

    return fileContents;
}

std::string GetStageNodeText(SceneNode* sceneNode, int tabSpace = 0) {
    const std::string tabSpaceText = std::string(tabSpace, ' ');
    const std::string childrenText = sceneNode->children.empty() ? "[]" : "[\n";
    std::string fileContents;
    fileContents += tabSpaceText + "        StageNode(\n";
    fileContents += tabSpaceText + "            name=\"" + sceneNode->name + "\",\n";
    fileContents += tabSpaceText + "            type=\"" + sceneNode->GetTypeString() + "\",\n";
    fileContents += tabSpaceText + "            tags=None,\n";
    fileContents += tabSpaceText + "            external_node_source=None,\n";

    if (!sceneNode->HasComponents()) {
        fileContents += tabSpaceText + "            components=[]\n";
    } else {
        fileContents += tabSpaceText + "            components=[\n";
        fileContents += GetStageNodeComponentsText(sceneNode, tabSpace);
        fileContents += tabSpaceText + "            ],\n";
    }

    if (sceneNode->children.empty()) {
        fileContents += tabSpaceText + "            children=[],\n";
    } else {
        fileContents += tabSpaceText + "            children=[\n";
        for (auto* childNode : sceneNode->children) {
            fileContents += GetStageNodeText(childNode, tabSpace + 8);
        }
        fileContents += tabSpaceText + "            ],\n";
    }

    fileContents += tabSpaceText + "        ),\n";
    return fileContents;
}
} // namespace

void SceneFileCreator::GenerateSceneFile(SceneNodeFile *nodeFile, const char *filePath) {
    std::string fileContents = "from crescent_api import *\n";
    fileContents += "\n";
    // Begin
    fileContents += "create_stage_nodes(\n";
    fileContents += "    stage_nodes=[\n";
    // Stage Nodes
    fileContents += GetStageNodeText(nodeFile->rootNode);
    // End
    fileContents += "    ],\n";
    fileContents += ")\n";

    FileSystemHelper::WriteFile(filePath, fileContents);
}
