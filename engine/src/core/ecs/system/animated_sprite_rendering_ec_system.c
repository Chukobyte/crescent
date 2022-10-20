#include "animated_sprite_rendering_ec_system.h"

#include <string.h>

#include <SDL2/SDL.h>

#include "../seika/src/rendering/renderer.h"
#include "../seika/src/utils/rbe_string_util.h"
#include "../seika/src/utils/rbe_assert.h"

#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/animated_sprite_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_utils.h"

EntitySystem* animatedSpriteRenderingSystem = NULL;

void animated_sprite_rendering_system_render();

EntitySystem* animated_sprite_rendering_ec_system_create() {
    animatedSpriteRenderingSystem = rbe_ec_system_create();
    animatedSpriteRenderingSystem->name = rbe_strdup("Animated Sprite Rendering");
    animatedSpriteRenderingSystem->render_func = animated_sprite_rendering_system_render;
    animatedSpriteRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_ANIMATED_SPRITE;
    return animatedSpriteRenderingSystem;
}

void animated_sprite_rendering_system_render() {
    const RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    const RBECamera2D* defaultCamera = rbe_camera_manager_get_default_camera();
    const int currentTickTime = (int) SDL_GetTicks();
    for (size_t i = 0; i < animatedSpriteRenderingSystem->entity_count; i++) {
        const Entity entity = animatedSpriteRenderingSystem->entities[i];
        Transform2DComponent* spriteTransformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_ANIMATED_SPRITE);
        AnimationFrame currentFrame = animatedSpriteComponent->currentAnimation.animationFrames[animatedSpriteComponent->currentAnimation.currentFrame];
        if (animatedSpriteComponent->isPlaying) {
            const int newIndex = ((currentTickTime - (int) animatedSpriteComponent->startAnimationTickTime) / animatedSpriteComponent->currentAnimation.speed) % animatedSpriteComponent->currentAnimation.frameCount;
            if (newIndex != animatedSpriteComponent->currentAnimation.currentFrame) {
                // Index changed
                currentFrame = animatedSpriteComponent->currentAnimation.animationFrames[newIndex];
                if (newIndex + 1 == animatedSpriteComponent->currentAnimation.frameCount) {
                    // Animation Finished
                    if (!animatedSpriteComponent->currentAnimation.doesLoop) {
                        animatedSpriteComponent->isPlaying = false;
                    }
                }
                animatedSpriteComponent->currentAnimation.currentFrame = newIndex;
            }
        }
        const RBECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
        TransformModel2D* globalTransform = rbe_scene_manager_get_scene_node_global_transform(entity, spriteTransformComp);
        cre_scene_utils_apply_camera_and_origin_translation(globalTransform, &animatedSpriteComponent->origin, spriteTransformComp->ignoreCamera);
        spriteTransformComp->isGlobalTransformDirty = true; // TODO: Make global transform const
        const Size2D destinationSize = {
            currentFrame.drawSource.w * renderCamera->zoom.x,
            currentFrame.drawSource.h * renderCamera->zoom.y
        };
        sf_renderer_queue_sprite_draw_call(
            currentFrame.texture,
            currentFrame.drawSource,
            destinationSize,
            animatedSpriteComponent->modulate,
            animatedSpriteComponent->flipX,
            animatedSpriteComponent->flipY,
            globalTransform
        );
    }
}
