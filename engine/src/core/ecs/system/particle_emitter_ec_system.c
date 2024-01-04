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

CreEntitySystem* particle_emitter_system = NULL;
SETexture* particleSquareTexture = NULL;

static void particle_emitter_system_on_ec_system_destroy();
static void particle_emitter_system_on_entity_registered(CreEntity entity);
static void particle_emitter_system_on_entity_unregistered(CreEntity entity);
static void particle_emitter_system_on_entity_entered_scene(CreEntity entity);
static void particle_emitter_system_fixed_update(float deltaTime);
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
    particle_emitter_system->fixed_update_func = particle_emitter_system_fixed_update;
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

void particle_emitter_system_fixed_update(float deltaTime) {
    for (size_t i = 0; i < particle_emitter_system->entity_count; i++) {
        const CreEntity entity = particle_emitter_system->entities[i];
        Particles2DComponent* particles2DComponent = (Particles2DComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_PARTICLES_2D);
        for (int pi = 0; pi < particles2DComponent->amount; pi++) {
            cre_particle_system_integrate(&particles2DComponent->particles[pi], deltaTime);
        }
    }
}

void particle_emitter_system_render() {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    const CRECamera2D* defaultCamera = cre_camera_manager_get_default_camera();

    for (size_t i = 0; i < particle_emitter_system->entity_count; i++) {
        const CreEntity entity = particle_emitter_system->entities[i];
        const Particles2DComponent* particles2DComponent = (Particles2DComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_PARTICLES_2D);

        Transform2DComponent* particleTransformComp = (Transform2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
        const CRECamera2D* renderCamera = particleTransformComp->ignoreCamera ? defaultCamera : camera2D;

        // Particle types can only be Particle2DComponentType_SQUARE (default) and Particle2DComponentType_TEXTURE for now
        SETexture* texture = particles2DComponent->type == Particle2DComponentType_TEXTURE ? particles2DComponent->texture : particleSquareTexture;
        const SKARect2 particleDrawSource = (SKARect2){ 0.0f, 0.0f, 1.0f, 1.0f };
        static const SKAVector2 tempParticleSize = { 32.0f, 32.0f };
        const SKASize2D particleSize = (SKASize2D){ tempParticleSize.x * renderCamera->zoom.x, tempParticleSize.y * renderCamera->zoom.y };
        SKATransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, particleTransformComp);
        cre_scene_utils_apply_camera_and_origin_translation(globalTransform, &SKA_VECTOR2_ZERO, particleTransformComp->ignoreCamera);
        particleTransformComp->isGlobalTransformDirty = true;

        // Draw individual particles
        SKATransform2D baseParticleTransform;
        ska_transform2d_mat4_to_transform(globalTransform->model, &baseParticleTransform);

        for (int pi = 0; pi < particles2DComponent->amount; pi++) {
            const CreParticle2D* particle2D = &particles2DComponent->particles[pi];
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
