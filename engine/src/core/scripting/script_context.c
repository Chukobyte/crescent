#include "script_context.h"

#include "../memory/rbe_mem.h"
#include "../utils/rbe_assert.h"

RBEScriptContext* rbe_script_context_create() {
    // Only have one script context for now...
    RBEScriptContext* newScriptContext                        = RBE_MEM_ALLOCATE(RBEScriptContext);
    newScriptContext->on_create_instance                      = NULL;
    newScriptContext->on_delete_instance                      = NULL;
    newScriptContext->on_start                                = NULL;
    newScriptContext->on_update_all_instances                 = NULL;
    newScriptContext->on_end                                  = NULL;
    newScriptContext->on_network_callback                     = NULL;
    newScriptContext->on_entity_subscribe_to_network_callback = NULL;
    return newScriptContext;
}
