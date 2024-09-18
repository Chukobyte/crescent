#include "script_context.h"

#include <seika/memory.h>

CREScriptContext* cre_script_context_create() {
    return SKA_MEM_ALLOCATE(CREScriptContext);
}

CREScriptContext* cre_script_context_create_from_template(const CREScriptContextTemplate* template) {
    CREScriptContext* scriptContext = cre_script_context_create();
    scriptContext->on_script_context_init = template->on_script_context_init;
    scriptContext->on_script_context_finalize = template->on_script_context_finalize;
    scriptContext->on_create_instance = template->on_create_instance;
    scriptContext->on_delete_instance = template->on_delete_instance;
    scriptContext->on_start = template->on_start;
    scriptContext->on_pre_update_all = template->on_pre_update_all;
    scriptContext->on_post_update_all = template->on_post_update_all;
    scriptContext->on_update_instance = template->on_update_instance;
    scriptContext->on_fixed_update_instance = template->on_fixed_update_instance;
    scriptContext->on_end = template->on_end;
    scriptContext->on_network_callback = template->on_network_callback;
    return scriptContext;
}

void cre_script_context_destroy(CREScriptContext* scriptContext) {
    SKA_MEM_FREE(scriptContext);
}
