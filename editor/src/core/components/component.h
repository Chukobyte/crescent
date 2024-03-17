#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <functional>
#include <algorithm>
#include <utility>

#include <seika/utils/logger.h>
#include <seika/utils/se_assert.h>

#include "../engine/src/core/tilemap/tilemap.h"
#include "../engine/src/core/ecs/components/animated_sprite_component.h"
#include "../engine/src/core/ecs/components/collider2d_component.h"
#include "../engine/src/core/ecs/components/color_rect_component.h"
#include "../engine/src/core/ecs/components/parallax_component.h"
#include "../engine/src/core/ecs/components/particles2d_component.h"
#include "../engine/src/core/ecs/components/script_component.h"
#include "../engine/src/core/ecs/components/sprite_component.h"
#include "../engine/src/core/ecs/components/text_label_component.h"
#include "../engine/src/core/ecs/components/tilemap_component.h"
#include "../engine/src/core/ecs/components/transform2d_component.h"

struct EditorComponent {};

struct Transform2DComp : public EditorComponent {
    Transform2DComp() = default;

    explicit Transform2DComp(const Transform2DComponent* transform2DComponent)
        : transform2D(transform2DComponent->localTransform),
          zIndex(transform2DComponent->zIndex),
          isZIndexRelativeToParent(transform2DComponent->isZIndexRelativeToParent),
          ignoreCamera(transform2DComponent->ignoreCamera) {}

    SKATransform2D transform2D = SKA_TRANSFORM_IDENTITY;
    int zIndex = 0;
    bool isZIndexRelativeToParent = true;
    bool ignoreCamera = false;
};

struct SpriteComp : public EditorComponent {
    SpriteComp() = default;

    explicit SpriteComp(const SpriteComponent* spriteComponent, std::string texturePath, std::string shaderPath)
        : texturePath(std::move(texturePath)),
          drawSource(spriteComponent->drawSource),
          origin(spriteComponent->origin),
          flipH(spriteComponent->flipH),
          flipV(spriteComponent->flipV),
          modulate(spriteComponent->modulate),
          shaderPath(std::move(shaderPath)) {}

    std::string texturePath;
    SKARect2 drawSource = SKA_RECT2D_ZERO;
    SKAVector2 origin = SKA_VECTOR2_ZERO;
    bool flipH = false;
    bool flipV = false;
    SKAColor modulate = SKA_COLOR_WHITE;
    std::string shaderPath;
};

// TODO: Put editor animation stuff in another file...
struct EditorAnimationFrame {
    std::string texturePath;
    SKARect2 drawSource = SKA_RECT2D_ZERO;
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
            se_logger_error("Tried to remove frame nonexistent frame '%d' from animation '%s'", frameIndex,
                            name.c_str());
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

    [[nodiscard]] EditorAnimationFrame& GetAnimationFrame(int frameIndex) {
        for (auto& animFrame : animationFrames) {
            if (animFrame.frame == frameIndex) {
                return animFrame;
            }
        }
        se_logger_error("Anim '%s' doesn't have frame '%d'", name.c_str(), frameIndex);
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

    explicit AnimatedSpriteComp(const AnimatedSpriteComponentData* animatedSpriteComponentData, std::string shaderPath)
        : currentAnimationName(animatedSpriteComponentData->currentAnimation.name),
          modulate(animatedSpriteComponentData->modulate),
          isPlaying(animatedSpriteComponentData->isPlaying),
          origin(animatedSpriteComponentData->origin),
          flipH(animatedSpriteComponentData->flipH),
          flipV(animatedSpriteComponentData->flipV),
          staggerStartAnimationTimes(animatedSpriteComponentData->staggerStartAnimationTimes),
          shaderPath(std::move(shaderPath)) {
        for (size_t animationIndex = 0; animationIndex < animatedSpriteComponentData->animationCount; animationIndex++) {
            const AnimationData& animData = animatedSpriteComponentData->animations[animationIndex];
            EditorAnimation animation = { animData.name, animData.speed, animData.doesLoop };
            SE_ASSERT_FMT(!animation.name.empty(), "Animation is empty!");
            for (size_t frameIndex = 0; (int) frameIndex < animData.frameCount; frameIndex++) {
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
        se_logger_error("Failed to get anim at with name '%s'", name.c_str());
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
    SKAColor modulate = SKA_COLOR_WHITE;
    bool isPlaying = false;
    SKAVector2 origin = SKA_VECTOR2_ZERO;
    bool flipH = false;
    bool flipV = false;
    bool staggerStartAnimationTimes = false;
    std::string shaderPath;
};

struct TextLabelComp : public EditorComponent {
    TextLabelComp() = default;

    explicit TextLabelComp(const TextLabelComponent* textLabelComponent, std::string fontUID)
        : text(textLabelComponent->text),
          fontUID(std::move(fontUID)),
          color(textLabelComponent->color) {}

    std::string text;
    std::string fontUID;
    SKAColor color = SKA_COLOR_WHITE;
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

    SKASize2D extents = SKA_SIZE2D_ZERO;
    SKAColor color = { .r = 0.0f, .g = 0.0f, .b = 0.8f, .a = 0.8f };
};

struct ColorRectComp : public EditorComponent {
    ColorRectComp() = default;

    explicit ColorRectComp(const ColorRectComponent* colorSquareComp)
        : size(colorSquareComp->size),
          color(colorSquareComp->color) {}

    SKASize2D size = { .w = 32.0f, .h = 32.0f };
    SKAColor color = SKA_COLOR_WHITE;
};

struct ParallaxComp : public EditorComponent {
    ParallaxComp() = default;

    explicit ParallaxComp(const ParallaxComponent* parallaxComponent)
        : scrollSpeed(parallaxComponent->scrollSpeed) {}

    SKAVector2 scrollSpeed = SKA_VECTOR2_ZERO;
};

struct Particles2DComp : public EditorComponent {
    Particles2DComp() {
        // TODO: Clean up test
        internalComp.initialVelocity = { 300.0f, 100.0f };
        particles2d_component_set_default_particles(&internalComp);
    }

    explicit Particles2DComp(const Particles2DComponent* particles2DComp) {
        memcpy(&internalComp, particles2DComp, sizeof(internalComp));
        isEmitting = particles2DComp->state != Particle2DComponentState_INACTIVE;
        particles2d_component_set_default_particles(&internalComp);
    }

    int& amount = internalComp.amount;
    SKAMinMaxVec2& initialVelocity = internalComp.initialVelocity;
    SKAColor& color = internalComp.color;
    float& spread = internalComp.spread;
    float& lifeTime = internalComp.lifeTime;
    float& damping = internalComp.damping;
    float& explosiveness = internalComp.explosiveness;
    Particle2DComponentState& state = internalComp.state;
    bool isEmitting = true;

    [[nodiscard]] Particles2DComponent* GetInternalComp() {
        return &internalComp;
    }

private:
    Particles2DComponent internalComp = {
        .amount = 8,
        .initialVelocity = SKA_VECTOR2_ZERO,
        .color = SKA_COLOR_WHITE,
        .spread = 45.0f,
        .lifeTime = 4.0f,
        .damping = 1.0f,
        .explosiveness = 0.0f,
        .state = Particle2DComponentState_WAITING_TO_INITIALIZE,
        .type = Particle2DComponentType_SQUARE,
        .squareSize = { .w = 4.0f, .h = 4.0f },
        .typeTexture = { .texture = nullptr, .drawSource = { 0.0f, 0.0f, 1.0f, 1.0f } },
        .particles = {0}
    };
};

struct TilemapComp : public EditorComponent {
    TilemapComp() = default;

    explicit TilemapComp(const TilemapComponent* tilemapComp, std::string tilemapTexturePath) : texturePath(std::move(tilemapTexturePath)) {
        memcpy(&internalComp.tilemap->tileset, &tilemapComp->tilemap->tileset, sizeof(CreTileset));
        memcpy(internalComp.tilemap, tilemapComp->tilemap, sizeof(CreTilemap));
        memcpy(&internalComp.origin, &tilemapComp->origin, sizeof(SKAVector2));
        // Copy active tiles
        memcpy(internalComp.tilemap->activeTiles, tilemapComp->tilemap->activeTiles, sizeof(SkaArrayList));
        for (size_t i = 0; i < tilemapComp->tilemap->activeTiles->size; i++) {
            const CreTileData* tileData = (CreTileData*)*(CreTileData**)ska_array_list_get(tilemapComp->tilemap->activeTiles, i);
            cre_tilemap_set_tile_data(internalComp.tilemap, tileData);
        }
        cre_tilemap_commit_active_tile_changes(internalComp.tilemap);
    }

    ~TilemapComp() {
        cre_tilemap_finalize(internalComp.tilemap);
    }

    [[nodiscard]] const SKASize2Di& GetTileSize() const {
        return internalComp.tilemap->tileset.tileSize;
    }

    [[nodiscard]] const SETexture* GetTexture() const {
        return internalComp.tilemap->tileset.texture;
    }

    void ForEachActiveTile(const std::function<void(const CreTileData*)>& func) const {
        for (size_t i = 0; i < internalComp.tilemap->activeTiles->size; i++) {
            const CreTileData* tileData = (CreTileData *) *(CreTileData **) ska_array_list_get(internalComp.tilemap->activeTiles, i);
            func(tileData);
        }
    }

    [[nodiscard]] TilemapComponent& GetInternalComp() {
        return internalComp;
    }

    std::string texturePath;
    SKAVector2& origin = internalComp.origin;

private:
    TilemapComponent internalComp = {
        .tilemap = cre_tilemap_create_and_initialize(),
        .origin = SKA_VECTOR2_ZERO
    };
};
