#include "sprite_rendering_ec_system.h"

#include "ec_system.h"

void sprite_rendering_system_render();

EntitySystem* sprite_rendering_system_create() {
    EntitySystem* system = rbe_ec_system_create();
    system->render_func = sprite_rendering_system_render;
    system->component_signature = ComponentType_SPRITE;
    return system;
}

void sprite_rendering_system_render() {}
