#include "render_context.h"

#include <stddef.h>

#include "../memory/se_mem.h"
#include "../utils/se_assert.h"

static RenderContext* renderContext = NULL;

void se_render_context_initialize() {
    SE_ASSERT_FMT(renderContext == NULL, "Render context is already initialized!");
    renderContext = SE_MEM_ALLOCATE(RenderContext);
}

void se_render_context_finalize() {
    SE_ASSERT_FMT(renderContext != NULL, "Render context is not initialized!");
    SE_MEM_FREE(renderContext);
}

RenderContext* se_render_context_get() {
    SE_ASSERT_FMT(renderContext != NULL, "Render context is not initialized!");
    return renderContext;
}
