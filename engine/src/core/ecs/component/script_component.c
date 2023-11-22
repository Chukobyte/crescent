#include "script_component.h"

#include <string.h>

#include <seika/memory/se_mem.h>

ScriptComponent* script_component_create(const char* path, const char* name) {
    ScriptComponent* scriptComponent = SE_MEM_ALLOCATE(ScriptComponent);
    strcpy(scriptComponent->classPath, path);
    strcpy(scriptComponent->className, name);
    scriptComponent->contextType = ScriptContextType_NONE;
    return scriptComponent;
}

ScriptComponent* script_component_create_ex(const char* path, const char* name, ScriptContextType contextType) {
    ScriptComponent* scriptComponent = script_component_create(path, name);
    scriptComponent->contextType = contextType;
    return scriptComponent;
}

void script_component_delete(ScriptComponent* scriptComponent) {
    SE_MEM_FREE(scriptComponent);
}

ScriptComponent* script_component_copy(const ScriptComponent* scriptComponent) {
    ScriptComponent* copiedNode = SE_MEM_ALLOCATE(ScriptComponent);
    memcpy(copiedNode, scriptComponent, sizeof(ScriptComponent));
    return copiedNode;
}
