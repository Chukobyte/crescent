#include "render_context.h"

#include <stddef.h>

#include "../memory/rbe_mem.h"
#include "../utils/rbe_assert.h"

static RenderContext* renderContext = NULL;

void rbe_render_context_initialize() {
    RBE_ASSERT_FMT(renderContext == NULL, "Render context is already initialized!");
    renderContext = RBE_MEM_ALLOCATE(RenderContext);
}

void rbe_render_context_finalize() {
    RBE_ASSERT_FMT(renderContext != NULL, "Render context is not initialized!");
    RBE_MEM_FREE(renderContext);
}

RenderContext* rbe_render_context_get() {
    RBE_ASSERT_FMT(renderContext != NULL, "Render context is not initialized!");
    return renderContext;
}
