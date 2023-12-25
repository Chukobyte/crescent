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

CreEntitySystem* particle_emitter_system = NULL;
SETexture* particleSquareTexture = NULL;

static void particle_emitter_system_on_ec_system_destroy();
static void particle_emitter_system_on_entity_registered(CreEntity entity);
static void particle_emitter_system_on_entity_unregistered(CreEntity entity);
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
    particle_emitter_system->on_ec_system_destroy = particle_emitter_system_on_ec_system_destroy;
    particle_emitter_system->fixed_update_func = particle_emitter_system_fixed_update;
    particle_emitter_system->render_func = particle_emitter_system_render;
    particle_emitter_system->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_PARTICLES_2D;

    SE_ASSERT(!particleSquareTexture);
    particleSquareTexture = !squareTextureOverride ? se_texture_create_solid_colored_texture(1, 1, 255) : squareTextureOverride;
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

void particle_emitter_system_fixed_update(float deltaTime) {
    for (size_t i = 0; i < particle_emitter_system->entity_count; i++) {
        const CreEntity entity = particle_emitter_system->entities[i];
        const Particles2DComponent* particles2DComponent = (Particles2DComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_PARTICLES_2D);
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
        const SERect2 particleDrawSource = (SERect2){ 0.0f, 0.0f, 0.0f, 0.0f };
        const SESize2D particleSize = (SESize2D){ 0.0f * renderCamera->zoom.x, 0.0f * renderCamera->zoom.y };
        // TODO: Use real global transform
        SETransformModel2D globalTransform = {0};
        se_renderer_queue_sprite_draw_call(
                texture,
                particleDrawSource,
                particleSize,
                particles2DComponent->color,
                false,
                false,
                &globalTransform,
                particleTransformComp->zIndex,
                NULL
        );
    }
}
