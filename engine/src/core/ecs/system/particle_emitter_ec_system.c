#include "particle_emitter_ec_system.h"

#include <seika/rendering/renderer.h>
#include <seika/memory/se_mem.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "ec_system.h"
#include "../component/particles2d_component.h"
#include "../component/transform2d_component.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../scene/scene_manager.h"
#include "../../scene/scene_utils.h"

typedef struct CreParticleRenderItem {
    SETexture* texture;
    SKASize2D size;
}CreParticleRenderItem;

CreEntitySystem* particle_emitter_system = NULL;
SETexture* particleSquareTexture = NULL;

static void particle_emitter_system_on_ec_system_destroy();
static void particle_emitter_system_on_entity_registered(CreEntity entity);
static void particle_emitter_system_on_entity_unregistered(CreEntity entity);
static void particle_emitter_system_on_entity_entered_scene(CreEntity entity);
static void particle_emitter_system_update(float deltaTime);
static void particle_emitter_system_render();

CreEntitySystem* cre_particle_emitter_ec_system_create() {
    return cre_particle_emitter_ec_system_create_ex(NULL);
}

CreEntitySystem* cre_particle_emitter_ec_system_create_ex(SETexture* squareTextureOverride) {
    SE_ASSERT(!particle_emitter_system);
    particle_emitter_system = cre_ec_system_create();
    particle_emitter_system->name = se_strdup("Particle Emitter");
    particle_emitter_system->on_entity_registered_func = particle_emitter_system_on_entity_registered;
    particle_emitter_system->on_entity_unregistered_func = particle_emitter_system_on_entity_unregistered;
    particle_emitter_system->on_entity_entered_scene_func = particle_emitter_system_on_entity_entered_scene;
    particle_emitter_system->on_ec_system_destroy = particle_emitter_system_on_ec_system_destroy;
    particle_emitter_system->update_func = particle_emitter_system_update;
    particle_emitter_system->render_func = particle_emitter_system_render;
    particle_emitter_system->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_PARTICLES_2D;

    SE_ASSERT(!particleSquareTexture);
    particleSquareTexture = squareTextureOverride != NULL ? squareTextureOverride : se_texture_create_solid_colored_texture(1, 1, 255);
    return particle_emitter_system;
}

void particle_emitter_system_on_ec_system_destroy() {
    SE_ASSERT(particleSquareTexture);
    se_texture_delete(particleSquareTexture);
    particleSquareTexture = NULL;
    SE_ASSERT(particle_emitter_system);
    particle_emitter_system = NULL;
}

void particle_emitter_system_on_entity_registered(CreEntity entity) {

}

void particle_emitter_system_on_entity_unregistered(CreEntity entity) {

}

void particle_emitter_system_on_entity_entered_scene(CreEntity entity) {
    // Set initial velocity for particles
    Particles2DComponent* particles2DComponent = (Particles2DComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2d_component_set_default_particles(particles2DComponent);
}

static void particle_emitter_emit_particle(Particles2DComponent* particles2DComponent, float deltaTime) {
    for (int i = 0; i < particles2DComponent->amount; i++) {
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
            const float explosivenessAdjustedTimeInactive = -ska_math_map_to_range(particles2DComponent->explosiveness, 1.0f, 0.0f, 0.0f, particles2DComponent->lifeTime);
            for (int pi = 0; pi < particles2DComponent->amount; pi++) {
                CreParticle2D* currentParticle = &particles2DComponent->particles[pi];
                currentParticle->state = Particle2DState_TIMED_WAITING_TO_BE_ACTIVE;
                const float currentParticleLerpAlpha = ska_math_map_to_unit((float)pi, 0.0f, (float)particles2DComponent->amount);
                currentParticle->timeActive = ska_math_lerpf(explosivenessAdjustedTimeInactive * currentParticleLerpAlpha, -particles2DComponent->lifeTime, particles2DComponent->explosiveness);
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

void particle_emitter_system_update(float deltaTime) {
    for (size_t i = 0; i < particle_emitter_system->entity_count; i++) {
        const CreEntity entity = particle_emitter_system->entities[i];
        Particles2DComponent* particles2DComponent = (Particles2DComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_PARTICLES_2D);
        cre_particle_emitter_ec_system_update_component(particles2DComponent, deltaTime);
    }
}

void particle_emitter_system_render() {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();

    for (size_t i = 0; i < particle_emitter_system->entity_count; i++) {
        const CreEntity entity = particle_emitter_system->entities[i];
        Particles2DComponent* particles2DComponent = (Particles2DComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_PARTICLES_2D);

        Transform2DComponent* particleTransformComp = (Transform2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
        const CRECamera2D* renderCamera = particleTransformComp->ignoreCamera ? defaultCamera : camera2D;

        // Particle types can only be Particle2DComponentType_SQUARE (default) and Particle2DComponentType_TEXTURE for now
        const CreParticleRenderItem renderItem = particles2DComponent->type == Particle2DComponentType_TEXTURE
                ? (CreParticleRenderItem){ .texture = particles2DComponent->typeTexture.texture, .size = particles2DComponent->squareSize }
                : (CreParticleRenderItem){ .texture = particleSquareTexture, .size = particles2DComponent->squareSize };

        SETexture* texture = renderItem.texture;
        const SKARect2 particleDrawSource = (SKARect2){ 0.0f, 0.0f, 1.0f, 1.0f };
        const SKASize2D particleSize = (SKASize2D){ renderItem.size.w * renderCamera->zoom.x, renderItem.size.h * renderCamera->zoom.y };
        SKATransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, particleTransformComp);
        cre_scene_utils_apply_camera_and_origin_translation(globalTransform, &SKA_VECTOR2_ZERO, particleTransformComp->ignoreCamera);
        particleTransformComp->isGlobalTransformDirty = true;

        // Draw individual particles
        SKATransform2D baseParticleTransform;
        ska_transform2d_mat4_to_transform(globalTransform->model, &baseParticleTransform);

        for (int pi = 0; pi < particles2DComponent->amount; pi++) {
            CreParticle2D* particle2D = &particles2DComponent->particles[pi];
            if (particle2D->state != Particle2DState_ACTIVE) {
                continue;
            }
            // TODO: Setting inactive here so we can render
            if (particle2D->timeActive >= particles2DComponent->lifeTime) {
                particle2D->state = Particle2DState_TIMED_WAITING_TO_BE_ACTIVE;
            }
            const SKATransform2D particle2DTransform = {
                .position = { .x = baseParticleTransform.position.x + particle2D->position.x, .y = baseParticleTransform.position.y + particle2D->position.y },
                .scale = baseParticleTransform.scale,
                .rotation = baseParticleTransform.rotation
            };
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
        }
    }
}
