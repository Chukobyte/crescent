#pragma once

#include "../script_context.h"

struct CRENativeScriptClass;

CREScriptContextTemplate cre_native_get_script_context_template();
void cre_native_class_register_new_class(struct CRENativeScriptClass* scriptClass);
