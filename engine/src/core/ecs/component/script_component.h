#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ScriptContextType {
    ScriptContextType_NONE = -1, // INVALID
    ScriptContextType_PYTHON = 0,
    ScriptContextType_NATIVE = 1, // C/C++

    ScriptContextType_TOTAL_TYPES = 2
} ScriptContextType;

typedef struct ScriptComponent {
    const char* classPath;
    const char* className;
    ScriptContextType contextType;
} ScriptComponent;

ScriptComponent* script_component_create();
void script_component_delete(ScriptComponent* scriptComponent);
ScriptComponent* script_component_copy(const ScriptComponent* scriptComponent);

#ifdef __cplusplus
}
#endif
