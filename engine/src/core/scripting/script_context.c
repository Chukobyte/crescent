#include "script_context.h"

#include <seika/memory.h>

CREScriptContext* cre_script_context_create() {
    return SKA_MEM_ALLOCATE(CREScriptContext);
}

CREScriptContext* cre_script_context_create_from_template(const CREScriptContextTemplate* temp) {
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_script_context_init = temp->on_script_context_init;
    scriptContext->on_script_context_finalize = temp->on_script_context_finalize;
    scriptContext->on_create_instance = temp->on_create_instance;
    scriptContext->on_delete_instance = temp->on_delete_instance;
    scriptContext->on_start = temp->on_start;
    scriptContext->on_pre_update_all = temp->on_pre_update_all;
    scriptContext->on_post_update_all = temp->on_post_update_all;
    scriptContext->on_update_instance = temp->on_update_instance;
    scriptContext->on_fixed_update_instance = temp->on_fixed_update_instance;
    scriptContext->on_end = temp->on_end;
    scriptContext->on_network_callback = temp->on_network_callback;
    return scriptContext;
}

void cre_script_context_destroy(CREScriptContext* scriptContext) {
    SKA_MEM_FREE(scriptContext);
}
