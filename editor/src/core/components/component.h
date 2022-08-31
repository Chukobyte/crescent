#pragma once

#include <string>

#include "../engine/src/core/ecs/component/animated_sprite_component.h"
#include "../engine/src/core/ecs/component/collider2d_component.h"
#include "../engine/src/core/ecs/component/color_square_component.h"
#include "../engine/src/core/ecs/component/script_component.h"
#include "../engine/src/core/ecs/component/sprite_component.h"
#include "../engine/src/core/ecs/component/text_label_component.h"
#include "../engine/src/core/ecs/component/transform2d_component.h"

struct EditorComponent {};

struct Transform2DComp : public EditorComponent {
    Transform2DComp() = default;

    explicit Transform2DComp(const Transform2DComponent* transform2DComponent) {}

    Transform2D transform2D = {
        .position = { 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f },
        .rotation = 0.0f
    };
    int zIndex = 0;
    bool isZIndexRelativeToParent;
    bool ignoreCamera = false;
};

struct SpriteComp : public EditorComponent {
    SpriteComp() = default;

    explicit SpriteComp(const SpriteComponent* spriteComponent, const std::string& texturePath) {}

    std::string texturePath;
    Rect2 drawSource = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f };
    Vector2 origin = { .x = 0.0f, .y = 0.0f };
    bool flipX = false;
    bool flipY = false;
    Color modulate = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
};

struct AnimatedSpriteComp : public EditorComponent {
    AnimatedSpriteComp() = default;

    explicit AnimatedSpriteComp(const AnimatedSpriteComponentData* animatedSpriteComponentData) {}

//    std::vector<Animation> animations;
    Color modulate = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
    bool isPlaying = false;
    Vector2 origin = { .x = 0.0f, .y = 0.0f };
    bool flipX = false;
    bool flipY = false;
};

struct TextLabelComp : public EditorComponent {
    TextLabelComp() = default;

    explicit TextLabelComp(const TextLabelComponent* textLabelComponent, const std::string& fontUID) {}

    std::string text;
    std::string fontUID;
    Color color = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
};

struct ScriptComp : public EditorComponent {
    ScriptComp() = default;

    explicit ScriptComp(const ScriptComponent* scriptComponent) {}

    std::string classPath;
    std::string className;
};

struct Collider2DComp : public EditorComponent {
    Collider2DComp() = default;

    explicit Collider2DComp(const Collider2DComponent* collider2DComponent) {}

    Size2D extents = { .w = 0.0f, .h = 0.0f };
    Color color = { .r = 0.0f, .g = 0.0f, .b = 0.8f, .a = 1.0f };
};

struct ColorSquareComp : public EditorComponent {
    ColorSquareComp() = default;

    explicit ColorSquareComp(const ColorSquareComponent* colorSquareComp) {}

    Size2D extents = { .w = 32.0f, .h = 32.0f };
    Color color = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
};
