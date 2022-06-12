#pragma once

typedef enum ScriptContextType {
    ScriptContextType_NONE = 0, // INVALID
    ScriptContextType_NATIVE = 1 << 0, // C/C++
    ScriptContextType_PYTHON = 1 << 1
} ScriptContextType;

typedef struct ScriptComponent {
    char* classPath;
    char* className;
    ScriptContextType contextType;
} ScriptComponent;

ScriptComponent* script_component_create();
