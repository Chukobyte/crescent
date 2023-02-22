#include "render_context.h"

#include <stddef.h>

#include "../memory/se_mem.h"
#include "../utils/se_assert.h"

static SERenderContext* renderContext = NULL;

void se_render_context_initialize() {
    SE_ASSERT_FMT(renderContext == NULL, "Render context is already initialized!");
    renderContext = SE_MEM_ALLOCATE(SERenderContext);
}

void se_render_context_finalize() {
    SE_ASSERT_FMT(renderContext != NULL, "Render context is not initialized!");
    SE_MEM_FREE(renderContext);
}

SERenderContext* se_render_context_get() {
    SE_ASSERT_FMT(renderContext != NULL, "Render context is not initialized!");
    return renderContext;
}
