#pragma once

#include <string>

#include "../engine/src/core/ecs/component/animated_sprite_component.h"
#include "../engine/src/core/ecs/component/collider2d_component.h"
#include "../engine/src/core/ecs/component/color_rect_component.h"
#include "../engine/src/core/ecs/component/script_component.h"
#include "../engine/src/core/ecs/component/sprite_component.h"
#include "../engine/src/core/ecs/component/text_label_component.h"
#include "../engine/src/core/ecs/component/transform2d_component.h"
#include "../engine/src/core/utils/logger.h"

struct EditorComponent {};

struct Transform2DComp : public EditorComponent {
    Transform2DComp() = default;

    explicit Transform2DComp(const Transform2DComponent* transform2DComponent)
        : transform2D(transform2DComponent->localTransform),
          zIndex(transform2DComponent->zIndex),
          isZIndexRelativeToParent(transform2DComponent->isZIndexRelativeToParent),
          ignoreCamera(transform2DComponent->ignoreCamera) {}

    Transform2D transform2D = {
        .position = { 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f },
        .rotation = 0.0f
    };
    int zIndex = 0;
    bool isZIndexRelativeToParent = true;
    bool ignoreCamera = false;
};

struct SpriteComp : public EditorComponent {
    SpriteComp() = default;

    explicit SpriteComp(const SpriteComponent* spriteComponent, const std::string& texturePath)
        : texturePath(texturePath),
          drawSource(spriteComponent->drawSource),
          origin(spriteComponent->origin),
          flipX(spriteComponent->flipX),
          flipY(spriteComponent->flipY),
          modulate(spriteComponent->modulate) {}

    std::string texturePath;
    Rect2 drawSource = { .x = 0.0f, .y = 0.0f, .w = 0.0f, .h = 0.0f };
    Vector2 origin = { .x = 0.0f, .y = 0.0f };
    bool flipX = false;
    bool flipY = false;
    Color modulate = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
};

// TODO: Put editor animation stuff in another file...
struct EditorAnimationFrame {
    std::string texturePath;
    Rect2 drawSource = { 0.0f, 0.0f, 0.0f, 0.0f };
    int frame = -1;
};

struct EditorAnimation {
    void RemoveAnimatationFrameByIndex(int frameIndex) {
        // Erase index from vector
        bool hasRemoved = false;
        animationFrames.erase(std::remove_if(animationFrames.begin(), animationFrames.end(), [frameIndex, &hasRemoved](const EditorAnimationFrame& animFrame) {
            const bool indexMatches = frameIndex == animFrame.frame;
            if (indexMatches) {
                hasRemoved = true;
            }
            return indexMatches;
        }), animationFrames.end());
        if (hasRemoved) {
            // Shift previous frames that were in front back a frame
            for (auto& animFrame : animationFrames) {
                if (animFrame.frame >= frameIndex) {
                    animFrame.frame--;
                }
            }
        } else {
            rbe_logger_error("Tried to remove frame nonexistent frame '%d' from animation '%s'", frameIndex, name.c_str());
        }
    }

    bool HasAnimationFrame(int frameIndex) const {
        for (auto& animFrame : animationFrames) {
            if (animFrame.frame == frameIndex) {
                return true;
            }
        }
        return false;
    }

    EditorAnimationFrame& GetAnimationFrame(int frameIndex) {
        for (auto& animFrame : animationFrames) {
            if (animFrame.frame == frameIndex) {
                return animFrame;
            }
        }
        rbe_logger_error("Anim '%s' doesn't have frame '%d'", name.c_str(), frameIndex);
        static EditorAnimationFrame errorAnimFrame;
        return errorAnimFrame;
    }

    std::string name;
    int speed = 100;
    bool doesLoop = true;
    std::vector<EditorAnimationFrame> animationFrames;
};

struct AnimatedSpriteComp : public EditorComponent {
    AnimatedSpriteComp() = default;

    explicit AnimatedSpriteComp(const AnimatedSpriteComponentData* animatedSpriteComponentData)
        : currentAnimationName(animatedSpriteComponentData->currentAnimation.name),
          modulate(animatedSpriteComponentData->modulate),
          isPlaying(animatedSpriteComponentData->isPlaying),
          origin(animatedSpriteComponentData->origin),
          flipX(animatedSpriteComponentData->flipX),
          flipY(animatedSpriteComponentData->flipY) {
        for (size_t animationIndex = 0; animationIndex < animatedSpriteComponentData->animationCount; animationIndex++) {
            const AnimationData& animData = animatedSpriteComponentData->animations[animatedSpriteComponentData->animationCount];
            EditorAnimation animation = { animData.name, animData.speed, animData.doesLoop };
            for (size_t frameIndex = 0; frameIndex < animData.frameCount; frameIndex++) {
                const AnimationFrameData& frameData = animData.animationFrames[frameIndex];
                const EditorAnimationFrame animationFrame = { frameData.texturePath, frameData.drawSource, frameData.frame };
                animation.animationFrames.emplace_back(animationFrame);
            }
            animations.emplace_back(animation);
        }
    }

    void AddDefaultAnimation() {
        std::string animNameCandidate = "default";
        int nameIndex = 0;
        while (HasAnimationWithName(animNameCandidate)) {
            animNameCandidate = "default" + std::to_string(nameIndex++);
        }
        animations.emplace_back(EditorAnimation{ .name = animNameCandidate });
    }

    [[nodiscard]] bool HasAnimationWithName(const std::string& name) const {
        for (auto& anim : animations) {
            if (anim.name == name) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] EditorAnimation& GetAnimationByName(const std::string& name) const {
        for (auto& anim : animations) {
            if (anim.name == name) {
                return const_cast<EditorAnimation &>(anim);
            }
        }
        rbe_logger_error("Failed to get anim at with name '%s'", name.c_str());
        static EditorAnimation failedAnim;
        return failedAnim;
    }

    void RemoveAnimationByName(const std::string& name) {
        animations.erase(std::remove_if(animations.begin(), animations.end(), [name](const EditorAnimation& anim) {
            return name == anim.name;
        }), animations.end());
    }

    std::string currentAnimationName;
    std::vector<EditorAnimation> animations;
    Color modulate = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
    bool isPlaying = false;
    Vector2 origin = { .x = 0.0f, .y = 0.0f };
    bool flipX = false;
    bool flipY = false;
};

struct TextLabelComp : public EditorComponent {
    TextLabelComp() = default;

    explicit TextLabelComp(const TextLabelComponent* textLabelComponent, const std::string& fontUID)
        : text(textLabelComponent->text),
          fontUID(fontUID),
          color(textLabelComponent->color) {}

    std::string text;
    std::string fontUID;
    Color color = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
};

struct ScriptComp : public EditorComponent {
    ScriptComp() = default;

    explicit ScriptComp(const ScriptComponent* scriptComponent) :
        classPath(scriptComponent->classPath),
        className(scriptComponent->className) {}

    std::string classPath;
    std::string className;
};

struct Collider2DComp : public EditorComponent {
    Collider2DComp() = default;

    explicit Collider2DComp(const Collider2DComponent* collider2DComponent)
        : extents(collider2DComponent->extents),
          color(collider2DComponent->color) {}

    Size2D extents = { .w = 0.0f, .h = 0.0f };
    Color color = { .r = 0.0f, .g = 0.0f, .b = 0.8f, .a = 1.0f };
};

struct ColorRectComp : public EditorComponent {
    ColorRectComp() = default;

    explicit ColorRectComp(const ColorRectComponent* colorSquareComp)
        : size(colorSquareComp->size),
          color(colorSquareComp->color) {}

    Size2D size = { .w = 32.0f, .h = 32.0f };
    Color color = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f };
};
