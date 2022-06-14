#include "fps_display_class.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../native_script_class.h"
#include "../../../engine_context.h"
#include "../../../asset_manager.h"
#include "../../../rendering/renderer.h"
#include "../../../memory/rbe_mem.h"

typedef struct FpsDisplayClassData {
    int value;
} FpsDisplayClassData;

RBENativeScriptClass* fps_display_create_new_instance(Entity entity);
void fps_display_on_start(RBENativeScriptClass* nativeScriptClass);
void fps_display_on_end(RBENativeScriptClass* nativeScriptClass);
void fps_display_update(RBENativeScriptClass* nativeScriptClass, float deltaTime);

RBENativeScriptClass* fps_display_native_class_create_new() {
    RBENativeScriptClass* fpsClassInstance = fps_display_create_new_instance(NULL_ENTITY);
    fpsClassInstance->create_new_instance_func = fps_display_create_new_instance;
    return fpsClassInstance;
}

RBENativeScriptClass* fps_display_create_new_instance(Entity entity) {
    RBENativeScriptClass* fpsClassInstance = rbe_native_class_create_new(entity, "main", "FpsDisplay");
    fpsClassInstance->on_start_func = fps_display_on_start;
    fpsClassInstance->on_start_func = fps_display_on_end;
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

void fps_display_on_end(RBENativeScriptClass* nativeScriptClass) {}

void fps_display_update(RBENativeScriptClass* nativeScriptClass, float deltaTime) {
    static const Color fpsTextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    static char fpsDisplayBuffer[16];
    static char fpsAmountBuffer[6];
    gcvt(rbe_engine_context_get()->averageFPS, 4, fpsAmountBuffer);
    strcpy(fpsDisplayBuffer, "FPS: ");
    strcat(fpsDisplayBuffer, fpsAmountBuffer);
    rbe_renderer_queue_font_draw_call(rbe_asset_manager_get_font("verdana-32"), fpsDisplayBuffer, 275, 200, 1.0f, fpsTextColor);
}
