#pragma once

typedef struct ScriptComponent {
    char* classPath;
    char* className;
} ScriptComponent;

ScriptComponent* script_component_create();
