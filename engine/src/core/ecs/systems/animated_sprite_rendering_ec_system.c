#include "animated_sprite_rendering_ec_system.h"

#include <seika/seika.h>
#include <seika/rendering/renderer.h>
#include <seika/rendering/shader/shader_cache.h>
#include <seika/ecs/ecs.h>
#include <seika/assert.h>

#include "../ecs_globals.h"
#include "../components/animated_sprite_component.h"
#include "../components/transform2d_component.h"
#include "../../scene/scene_manager.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"

static void on_entity_registered(SkaECSSystem* system, SkaEntity entity);
static void animated_sprite_render(SkaECSSystem* system);

void cre_animated_sprite_rendering_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Animated Sprite Rendering");
    systemTemplate.on_entity_registered_func = on_entity_registered;
    systemTemplate.render_func = animated_sprite_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, AnimatedSpriteComponent);
}

void on_entity_registered(SkaECSSystem* system, SkaEntity entity) {
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    SKA_ASSERT(animatedSpriteComponent != NULL);
    animated_sprite_component_refresh_random_stagger_animation_time(animatedSpriteComponent);
    if (animatedSpriteComponent->isPlaying) {
        animatedSpriteComponent->startAnimationTickTime = (int32)ska_get_ticks() + animatedSpriteComponent->randomStaggerTime;
    }
}

void animated_sprite_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();
    const int32 currentTickTime = (int32)ska_get_ticks();
    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Transform2DComponent* spriteTransformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
        CreAnimationFrame* currentFrame = &animatedSpriteComponent->currentAnimation->animationFrames[animatedSpriteComponent->currentAnimation->currentFrame];
        if (animatedSpriteComponent->isPlaying) {
            const f32 entityTimeDilation = cre_scene_manager_get_node_full_time_dilation(entity);
            const f32 spriteCurrentTickTime = (f32) currentTickTime + (f32) animatedSpriteComponent->randomStaggerTime;
            const int32 tickRate = (int32) (((spriteCurrentTickTime - (f32) animatedSpriteComponent->startAnimationTickTime) / (f32) animatedSpriteComponent->currentAnimation->speed) * entityTimeDilation);
            const int32 newIndex = tickRate % animatedSpriteComponent->currentAnimation->frameCount;
            if (newIndex != animatedSpriteComponent->currentAnimation->currentFrame) {
                // Notify observers that frame has changed
                ska_event_notify_observers(&animatedSpriteComponent->onFrameChanged, &(SkaSubjectNotifyPayload){
                    .data = &(AnimatedSpriteFrameChangedPayload){ .entity = entity, .newFrame = newIndex }
                });

                currentFrame = &animatedSpriteComponent->currentAnimation->animationFrames[newIndex];
                const CreAnimation* animationBeforeNotification = animatedSpriteComponent->currentAnimation;
                if (newIndex + 1 == animatedSpriteComponent->currentAnimation->frameCount) {
                    // Notify the observers that the animation has finished
                    ska_event_notify_observers(&animatedSpriteComponent->onAnimationFinished, &(SkaSubjectNotifyPayload){
                        .data = &(AnimatedSpriteAnimationFinishedPayload){ .entity = entity, .animation = animatedSpriteComponent->currentAnimation }
                    });
                    if (!animatedSpriteComponent->currentAnimation->doesLoop) {
                        animatedSpriteComponent->isPlaying = false;
                    }
                }
                // Make sure it's the same animation before assinging new index, if not reset to 0 index
                animatedSpriteComponent->currentAnimation->currentFrame = animationBeforeNotification == animatedSpriteComponent->currentAnimation ? newIndex : 0;
            }
        }
        const CRECamera2D* renderCamera = spriteTransformComp->ignoreCamera ? defaultCamera : camera2D;
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, spriteTransformComp, &animatedSpriteComponent->origin);
        const SkaSize2D destinationSize = {
            .w = currentFrame->drawSource.w * renderCamera->zoom.x,
            .h = currentFrame->drawSource.h * renderCamera->zoom.y
        };

        ska_renderer_queue_sprite_draw(
            currentFrame->texture,
            currentFrame->drawSource,
            destinationSize,
            animatedSpriteComponent->modulate,
            animatedSpriteComponent->flipH,
            animatedSpriteComponent->flipV,
            &renderResource.transform2D,
            renderResource.globalZIndex,
            ska_shader_cache_get_instance_checked(animatedSpriteComponent->shaderInstanceId)
        );
    }
}
