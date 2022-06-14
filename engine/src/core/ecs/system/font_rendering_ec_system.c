#include "font_rendering_ec_system.h"

#include "string.h"

#include "ec_system.h"
#include "../../rendering/renderer.h"
#include "../../utils/rbe_assert.h"


EntitySystem* fontRenderingSystem = NULL;

void font_rendering_system_render();

EntitySystem* font_rendering_ec_system_create() {
    RBE_ASSERT(fontRenderingSystem == NULL);
    fontRenderingSystem = rbe_ec_system_create();
    fontRenderingSystem->name = strdup("Font Rendering");
    fontRenderingSystem->render_func = font_rendering_system_render;
    fontRenderingSystem->component_signature = ComponentType_TRANSFORM_2D | ComponentType_TEXT_LABEL;
    return fontRenderingSystem;
}

void font_rendering_system_render() {}
