#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct RenderContext {
    FT_Library freeTypeLibrary;
    int windowWidth;
    int windowHeight;
} RenderContext;

void se_render_context_initialize();
void se_render_context_finalize();
RenderContext* se_render_context_get();
