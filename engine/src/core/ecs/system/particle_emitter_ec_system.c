#include "particle_emitter_ec_system.h"

#include <seika/memory/se_mem.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "ec_system.h"
#include "../component/particles2d_component.h"

CreEntitySystem* particle_emitter_system = NULL;

static void particle_emitter_system_on_ec_system_destroy();
static void particle_emitter_system_on_entity_registered(CreEntity entity);
static void particle_emitter_system_fixed_update(float deltaTime);
static void particle_emitter_system_render();

CreEntitySystem* cre_particle_emitter_ec_system_create() {
    SE_ASSERT(!particle_emitter_system);
    particle_emitter_system = cre_ec_system_create();
    particle_emitter_system->name = se_strdup("Particle Emitter");
    particle_emitter_system->on_entity_registered_func = particle_emitter_system_on_entity_registered;
    particle_emitter_system->on_ec_system_destroy = particle_emitter_system_on_ec_system_destroy;
    particle_emitter_system->fixed_update_func = particle_emitter_system_fixed_update;
    particle_emitter_system->render_func = particle_emitter_system_render;
    particle_emitter_system->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_PARTICLES_2D;
    return particle_emitter_system;
}

void particle_emitter_system_on_ec_system_destroy() {
    SE_ASSERT(particle_emitter_system);
    particle_emitter_system = NULL;
}

void particle_emitter_system_on_entity_registered(CreEntity entity) {

}

void particle_emitter_system_fixed_update(float deltaTime) {
    for (size_t i = 0; i < particle_emitter_system->entity_count; i++) {
        const CreEntity entity = particle_emitter_system->entities[i];
        const Particles2DComponent* particles2DComponent = (Particles2DComponent*)cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_PARTICLES_2D);
    }
}

void particle_emitter_system_render() {

}
