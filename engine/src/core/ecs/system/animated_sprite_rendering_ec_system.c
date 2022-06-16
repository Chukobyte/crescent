#include "animated_sprite_rendering_ec_system.h"

#include <string.h>

#include "ec_system.h"

void animated_sprite_rendering_system_render();

EntitySystem* animated_sprite_rendering_ec_system_create() {
    EntitySystem* animatedSpriteRenderingSystem = rbe_ec_system_create();
    animatedSpriteRenderingSystem->name = strdup("Animated Sprite Rendering");
    animatedSpriteRenderingSystem->render_func = animated_sprite_rendering_system_render;
    animatedSpriteRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_ANIMATED_SPRITE;
    return animatedSpriteRenderingSystem;
}

void animated_sprite_rendering_system_render() {}
