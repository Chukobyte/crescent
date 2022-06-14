#include <stdio.h>
#include "fps_display_class.h"

#include "../native_script_class.h"
#include "../../../engine_context.h"
#include "../../../memory/rbe_mem.h"

typedef struct FpsDisplayClassData {
    int value;
} FpsDisplayClassData;

RBENativeScriptClass* fps_display_create_new_instance(Entity entity);
//void fps_display_on_start(Entity entity, void* inst_data);
//void fps_display_update(Entity entity, void* inst_data, float deltaTime);
void fps_display_on_start(RBENativeScriptClass* nativeScriptClass);
void fps_display_update(RBENativeScriptClass* nativeScriptClass, float deltaTime);

RBENativeScriptClass* fps_display_native_class_create_new() {
    RBENativeScriptClass* fpsClassInstance = fps_display_create_new_instance(NULL_ENTITY);
    fpsClassInstance->create_new_instance_func = fps_display_create_new_instance;
    return fpsClassInstance;
}

RBENativeScriptClass* fps_display_create_new_instance(Entity entity) {
    RBENativeScriptClass* fpsClassInstance = rbe_native_class_create_new(entity, "main", "FpsDisplay");
    fpsClassInstance->on_start_func = fps_display_on_start;
    fpsClassInstance->update_func = fps_display_update;
    FpsDisplayClassData* classData = RBE_MEM_ALLOCATE(FpsDisplayClassData);
    classData->value = 10;
    fpsClassInstance->instance_data = classData;
    fpsClassInstance->class_instance_size = (sizeof(RBENativeScriptClass*) + sizeof(FpsDisplayClassData*)) * 4;
    printf("[C SCRIPT] New instance!\n");
    return fpsClassInstance;
}

void fps_display_on_start(RBENativeScriptClass* nativeScriptClass) {
    FpsDisplayClassData* data = (FpsDisplayClassData*) nativeScriptClass->instance_data;
    printf("[C SCRIPT] - entity '%d' on start value = %d\n", nativeScriptClass->entity, data->value);
}

void fps_display_update(RBENativeScriptClass* nativeScriptClass, float deltaTime) {
//    printf("[C SCRIPT] - fps '%f'\n", rbe_engine_context_get()->averageFPS);
}
