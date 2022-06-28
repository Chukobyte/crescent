#pragma once

#include "native_script_class.h"

struct RBEScriptContext* rbe_native_create_script_context();

void rbe_native_class_register_new_class(RBENativeScriptClass* scriptClass);
