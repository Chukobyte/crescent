#pragma once

struct CRENativeScriptClass;

struct CREScriptContext* cre_native_create_script_context();
void cre_native_class_register_new_class(struct CRENativeScriptClass* scriptClass);
