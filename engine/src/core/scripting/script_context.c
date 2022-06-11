#include "script_context.h"

#include "../memory/rbe_mem.h"
#include "../utils/rbe_assert.h"

static RBEScriptContext* scriptContext = NULL;

RBEScriptContext* rbe_script_context_create() {
    // Only have one script context for now...
    RBE_ASSERT(scriptContext == NULL);
    scriptContext = RBE_MEM_ALLOCATE(RBEScriptContext);
    scriptContext->on_create_instance = NULL;
    scriptContext->on_delete_instance = NULL;
    scriptContext->on_start = NULL;
    scriptContext->on_update_all_instances = NULL;
    scriptContext->on_end = NULL;
    return scriptContext;
}

RBEScriptContext* rbe_script_context_get_current() {
    return scriptContext;
}
