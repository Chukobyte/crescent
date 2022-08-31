#include "script_component.h"

#include <stddef.h>
#include <string.h>

#include "../../memory/rbe_mem.h"

ScriptComponent* script_component_create() {
    ScriptComponent* scriptComponent = RBE_MEM_ALLOCATE(ScriptComponent);
    scriptComponent->classPath = NULL;
    scriptComponent->className = NULL;
    scriptComponent->contextType = ScriptContextType_NONE;
    return scriptComponent;
}

void script_component_delete(ScriptComponent* scriptComponent) {
    RBE_MEM_FREE(scriptComponent);
}

ScriptComponent* script_component_copy(const ScriptComponent* scriptComponent) {
    ScriptComponent* copiedNode = RBE_MEM_ALLOCATE(ScriptComponent);
    memcpy(copiedNode, scriptComponent, sizeof(ScriptComponent));
    return copiedNode;
}
