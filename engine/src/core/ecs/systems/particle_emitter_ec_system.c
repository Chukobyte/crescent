#include "particle_emitter_ec_system.h"

#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/string.h>
#include <seika/assert.h>

#include "../ecs_globals.h"
#include "../components/particles2d_component.h"
#include "../components/transform2d_component.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_manager.h"

typedef struct CreParticleRenderItem {
    SkaTexture* texture;
    SkaSize2D size;
}CreParticleRenderItem;

static void on_ec_system_registered(SkaECSSystem* system);
static void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity);
static void ec_system_update(SkaECSSystem* system, f32 deltaTime);
static void ec_system_render(SkaECSSystem* system);

SkaTexture* particleSquareTexture = NULL;

void cre_particle_ec_system_create_and_register() {
    cre_particle_ec_system_create_and_register_ex(NULL);
}

void cre_particle_ec_system_create_and_register_ex(SkaTexture* squareTextureOverride) {
    particleSquareTexture = squareTextureOverride;

    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Particle Emitter");
    systemTemplate.on_ec_system_register = on_ec_system_registered;
    systemTemplate.on_entity_entered_scene_func = on_entity_entered_scene;
    systemTemplate.update_func = ec_system_update;
    systemTemplate.render_func = ec_system_render;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, Particles2DComponent);
}

void on_ec_system_registered(SkaECSSystem* system) {
    if (particleSquareTexture == NULL) {
        particleSquareTexture = ska_texture_create_solid_colored_texture(1, 1, 255);
    }
}

void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity) {
    // Set initial velocity for particles
    Particles2DComponent* particles2DComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2d_component_set_default_particles(particles2DComponent);
}

void ec_system_update(SkaECSSystem* system, float deltaTime) {
    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Particles2DComponent* particles2DComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
        cre_particle_emitter_ec_system_update_component(particles2DComponent, deltaTime);
    }
}

void ec_system_render(SkaECSSystem* system) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();

    for (size_t i = 0; i < system->entity_count; i++) {
        const SkaEntity entity = system->entities[i];
        Transform2DComponent* particleTransformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        Particles2DComponent* particles2DComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
        const CRECamera2D* renderCamera = particleTransformComp->ignoreCamera ? defaultCamera : camera2D;

        // Particle types can only be Particle2DComponentType_SQUARE (default) and Particle2DComponentType_TEXTURE for now
        const CreParticleRenderItem renderItem = particles2DComponent->type == Particle2DComponentType_TEXTURE
                                                 ? (CreParticleRenderItem){ .texture = particles2DComponent->typeTexture.texture, .size = particles2DComponent->squareSize }
                                                 : (CreParticleRenderItem){ .texture = particleSquareTexture, .size = particles2DComponent->squareSize };

        SkaTexture* texture = renderItem.texture;
        const SkaRect2 particleDrawSource = (SkaRect2){ 0.0f, 0.0f, 1.0f, 1.0f };
        const SkaSize2D particleSize = (SkaSize2D){ renderItem.size.w * renderCamera->zoom.x, renderItem.size.h * renderCamera->zoom.y };
        const SceneNodeRenderResource renderResource = cre_scene_manager_get_scene_node_global_render_resource(entity, particleTransformComp, &SKA_VECTOR2_ZERO);

        // Draw individual particles
        const SkaTransform2D baseParticleTransform = renderResource.transform2D;
        for (int32 pi = 0; pi < particles2DComponent->amount; pi++) {
            CreParticle2D* particle2D = &particles2DComponent->particles[pi];
            if (particle2D->state != Particle2DState_ACTIVE) {
                continue;
            }
            // TODO: Setting inactive here so we can render
            if (particle2D->timeActive >= particles2DComponent->lifeTime) {
                particle2D->state = Particle2DState_TIMED_WAITING_TO_BE_ACTIVE;
            }
            const SkaTransform2D particle2DTransform = {
                .position = { .x = baseParticleTransform.position.x + particle2D->position.x, .y = baseParticleTransform.position.y + particle2D->position.y },
                .scale = baseParticleTransform.scale,
                .rotation = baseParticleTransform.rotation
            };

#ifdef CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA
            const SkaTransform2D lerpedTransform2D = ska_transform2d_lerp(&particle2D->prevTransform, &particle2DTransform, CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA);
            particle2D->prevTransform = particle2DTransform;
            ska_renderer_queue_sprite_draw(
                texture,
                particleDrawSource,
                particleSize,
                particle2D->color,
                false,
                false,
                &lerpedTransform2D,
                particleTransformComp->zIndex,
                NULL
            );
#else
            ska_renderer_queue_sprite_draw(
                texture,
                particleDrawSource,
                particleSize,
                particle2D->color,
                false,
                false,
                &particle2DTransform,
                particleTransformComp->zIndex,
                NULL
            );
#endif
        }
    }
}

static void particle_emitter_emit_particle(Particles2DComponent* particles2DComponent, float deltaTime) {
    for (int32 i = 0; i < particles2DComponent->amount; i++) {
        CreParticle2D* currentParticle = &particles2DComponent->particles[i];
        switch (currentParticle->state) {
            case Particle2DState_INACTIVE: {
                currentParticle->state = Particle2DState_ACTIVE;
            }
            case Particle2DState_ACTIVE: {
                cre_particle_system_integrate(currentParticle, deltaTime);
                break;
            }
            case Particle2DState_TIMED_WAITING_TO_BE_ACTIVE: {
                const bool isComingFromActive = currentParticle->timeActive >= particles2DComponent->lifeTime;
                if (isComingFromActive) {
                    particles2d_component_reset_particle(particles2DComponent, currentParticle);
                    currentParticle->timeActive = -particles2DComponent->lifeTime;

                }
                // Uses negative time of 'currentParticle->timeActive' as time inactive
                const float timeInactive = -currentParticle->timeActive + deltaTime;
                if (timeInactive >= particles2DComponent->lifeTime) {
                    currentParticle->timeActive = 0.0f;
                    currentParticle->state = Particle2DState_ACTIVE;
                } else {
                    currentParticle->timeActive -= deltaTime;
                }
            }
                break;
        }
    }
}

void cre_particle_emitter_ec_system_update_component(Particles2DComponent* particles2DComponent, float deltaTime) {
    switch (particles2DComponent->state) {
        case Particle2DComponentState_WAITING_TO_INITIALIZE: {
            // Spread out time active of particles on initialize so that they correspond to explosiveness
            const float explosivenessAdjustedTimeInactive = ska_math_map_to_range(particles2DComponent->explosiveness, 1.0f, 0.0f, 0.0f, particles2DComponent->lifeTime);
            for (int32 pi = 0; pi < particles2DComponent->amount; pi++) {
                // Reset particle to default first
                particles2DComponent->particles[pi] = CRE_PARTICLE2D_DEFAULT;
                CreParticle2D* currentParticle = &particles2DComponent->particles[pi];
                particles2d_component_reset_particle(particles2DComponent, currentParticle);
                currentParticle->state = Particle2DState_TIMED_WAITING_TO_BE_ACTIVE;
                const float currentParticleLerpAlpha = ska_math_map_to_unit((float)pi, 0.0f, (float)particles2DComponent->amount);
                currentParticle->timeActive = -ska_math_lerpf(explosivenessAdjustedTimeInactive * currentParticleLerpAlpha, particles2DComponent->lifeTime, particles2DComponent->explosiveness);
            }
            particles2DComponent->state = Particle2DComponentState_EMITTING;
        }
        case Particle2DComponentState_EMITTING: {
            particle_emitter_emit_particle(particles2DComponent, deltaTime);
            break;
        }
        case Particle2DComponentState_INACTIVE: {
            break;
        }
    }
}
