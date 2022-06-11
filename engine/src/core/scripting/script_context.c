#include "script_context.h"

#include "../memory/rbe_mem.h"

RBEScriptContext* rbe_script_context_create() {
    RBEScriptContext* scriptContext = RBE_MEM_ALLOCATE(RBEScriptContext);
    scriptContext->on_create_instance = NULL;
    scriptContext->on_delete_instance = NULL;
    scriptContext->on_start = NULL;
    scriptContext->on_update = NULL;
    scriptContext->on_end = NULL;
    return scriptContext;
}
