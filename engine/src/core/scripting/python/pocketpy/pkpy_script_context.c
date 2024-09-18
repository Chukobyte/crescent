#include "pkpy_script_context.h"

#include <pocketpy.h>

#include <seika/assert.h>

static CREScriptContext* scriptContext = NULL;

//--- Script Context Interface ---//
static void pkpy_sc_on_script_context_init(CREScriptContext* context);
static void pkpy_sc_on_script_context_finalize(CREScriptContext* context);

CREScriptContextTemplate cre_pkpy_get_script_context_template() {
    return (CREScriptContextTemplate){
        .contextType = CreScriptContextType_PYTHON,
        .on_script_context_init = pkpy_sc_on_script_context_init,
        .on_script_context_finalize = pkpy_sc_on_script_context_finalize,
        .on_create_instance = NULL,
        .on_delete_instance = NULL,
        .on_start = NULL,
        .on_pre_update_all = NULL,
        .on_post_update_all = NULL,
        .on_update_instance = NULL,
        .on_fixed_update_instance = NULL,
        .on_end = NULL,
        .on_network_callback = NULL,
    };
}

void pkpy_sc_on_script_context_init(CREScriptContext* context) {
    py_initialize();
    scriptContext = context;
}

void pkpy_sc_on_script_context_finalize(CREScriptContext* context) {
    scriptContext = NULL;
    py_finalize();
}
