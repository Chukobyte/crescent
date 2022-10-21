#include "script_context.h"

#include "../seika/src/memory/se_mem.h"
#include "../seika/src/utils/se_assert.h"

CREScriptContext* cre_script_context_create() {
    // Only have one script context for now...
    CREScriptContext* newScriptContext = SE_MEM_ALLOCATE(CREScriptContext);
    newScriptContext->on_create_instance = NULL;
    newScriptContext->on_delete_instance = NULL;
    newScriptContext->on_start = NULL;
    newScriptContext->on_update_all_instances = NULL;
    newScriptContext->on_end = NULL;
    newScriptContext->on_network_callback = NULL;
    newScriptContext->on_entity_subscribe_to_network_callback = NULL;
    return newScriptContext;
}
