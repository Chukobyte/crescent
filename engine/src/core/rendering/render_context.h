#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct RenderContext {
    FT_Library freeTypeLibrary;
} RenderContext;

void rbe_render_context_initialize();

void rbe_render_context_finalize();

RenderContext* rbe_render_context_get();
