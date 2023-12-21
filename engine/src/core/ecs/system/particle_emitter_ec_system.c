#include "particle_emitter_ec_system.h"

#include <seika/memory/se_mem.h>
#include <seika/utils/se_assert.h>

#include "ec_system.h"

CreEntitySystem* particle_emitter_system = NULL;

static void particle_emitter_system_on_ec_system_destroy();
static void particle_emitter_system_on_entity_registered(CreEntity entity);
static void particle_emitter_system_render();

CreEntitySystem* cre_particle_emitter_ec_system_create() {
    SE_ASSERT(!particle_emitter_system);
    particle_emitter_system = cre_ec_system_create();
    particle_emitter_system->on_entity_registered_func = particle_emitter_system_on_entity_registered;
    particle_emitter_system->on_ec_system_destroy = particle_emitter_system_on_ec_system_destroy;
    particle_emitter_system->render_func = particle_emitter_system_render;
    return particle_emitter_system;
}

void particle_emitter_system_on_ec_system_destroy() {
    SE_ASSERT(particle_emitter_system);
    particle_emitter_system = NULL;
}

void particle_emitter_system_on_entity_registered(CreEntity entity) {

}

void particle_emitter_system_render() {

}
