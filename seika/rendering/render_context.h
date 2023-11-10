#pragma once

#include "ft2build.h"
#include FT_FREETYPE_H"freetype/freetype.h"

typedef struct SERenderContext {
    FT_Library freeTypeLibrary;
    int windowWidth;
    int windowHeight;
} SERenderContext;

void se_render_context_initialize();
void se_render_context_finalize();
SERenderContext* se_render_context_get();
