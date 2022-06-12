#include "script_context.h"

#include "../memory/rbe_mem.h"
#include "../utils/rbe_assert.h"

static RBEScriptContext* scriptContext = NULL;

RBEScriptContext* rbe_script_context_create() {
    // Only have one script context for now...
    RBEScriptContext* newScriptContext = RBE_MEM_ALLOCATE(RBEScriptContext);
    newScriptContext->on_create_instance = NULL;
    newScriptContext->on_delete_instance = NULL;
    newScriptContext->on_start = NULL;
    newScriptContext->on_update_all_instances = NULL;
    newScriptContext->on_end = NULL;
    if (scriptContext == NULL) {
        scriptContext = newScriptContext;
        return scriptContext;
    } else {
        rbe_logger_warn("Adding additional script context, won't be recognized by scene manager!");
    }
    return newScriptContext;
}

RBEScriptContext* rbe_script_context_get_current() {
    return scriptContext;
}
