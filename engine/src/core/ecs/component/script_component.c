#include "script_component.h"

#include <stddef.h>

#include "../../memory/rbe_mem.h"

ScriptComponent* script_component_create() {
    ScriptComponent* scriptComponent = RBE_MEM_ALLOCATE(ScriptComponent);
    scriptComponent->classPath       = NULL;
    scriptComponent->className       = NULL;
    scriptComponent->contextType     = ScriptContextType_NONE;
    return scriptComponent;
}
