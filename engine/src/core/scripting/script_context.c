#include "script_context.h"

#include "seika/memory/se_mem.h"
#include "seika/utils/se_assert.h"

CREScriptContext* cre_script_context_create() {
    CREScriptContext* newScriptContext = SE_MEM_ALLOCATE(CREScriptContext);
    newScriptContext->on_create_instance = NULL;
    newScriptContext->on_delete_instance = NULL;
    newScriptContext->on_start = NULL;
    newScriptContext->on_pre_update_all = NULL;
    newScriptContext->on_post_update_all = NULL;
    newScriptContext->on_update_instance = NULL;
    newScriptContext->on_fixed_update_instance = NULL;
    newScriptContext->on_end = NULL;
    newScriptContext->on_network_callback = NULL;
    newScriptContext->on_entity_subscribe_to_network_callback = NULL;
    newScriptContext->updateEntityCount = 0;
    newScriptContext->fixedUpdateEntityCount = 0;
    return newScriptContext;
}
