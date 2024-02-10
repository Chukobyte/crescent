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

// TODO: Give a way for components to clean up themselves (e.g. call '[component_name]_component_delete' when deleted)
// TODO: (Part2) This way we can have less memory overhead by having flexibility for objects to clean themselves up
typedef struct ScriptComponent {
    char classPath[96];
    char className[32];
    ScriptContextType contextType;
} ScriptComponent;

ScriptComponent* script_component_create(const char* path, const char* name);
ScriptComponent* script_component_create_ex(const char* path, const char* name, ScriptContextType contextType);
void script_component_delete(ScriptComponent* scriptComponent);
ScriptComponent* script_component_copy(const ScriptComponent* scriptComponent);

#ifdef __cplusplus
}
#endif
