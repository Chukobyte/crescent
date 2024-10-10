#include "script_component.h"

#include <string.h>

#include <seika/memory.h>
#include <seika/string.h>

ScriptComponent* script_component_create(const char* path, const char* name) {
    ScriptComponent* scriptComponent = SKA_MEM_ALLOCATE(ScriptComponent);
    ska_strcpy(scriptComponent->classPath, path);
    ska_strcpy(scriptComponent->className, name);
    scriptComponent->contextType = CreScriptContextType_NONE;
    return scriptComponent;
}

ScriptComponent* script_component_create_ex(const char* path, const char* name, CreScriptContextType contextType) {
    ScriptComponent* scriptComponent = script_component_create(path, name);
    scriptComponent->contextType = contextType;
    return scriptComponent;
}

void script_component_delete(ScriptComponent* scriptComponent) {
    SKA_MEM_FREE(scriptComponent);
}

ScriptComponent* script_component_copy(const ScriptComponent* scriptComponent) {
    ScriptComponent* copiedNode = SKA_MEM_ALLOCATE(ScriptComponent);
    memcpy(copiedNode, scriptComponent, sizeof(ScriptComponent));
    return copiedNode;
}
