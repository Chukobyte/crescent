#include "fps_display_class.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../native_script_class.h"
#include "../../../engine_context.h"
#include "../../../asset_manager.h"
#include "../../../rendering/renderer.h"
#include "../../../memory/rbe_mem.h"
#include "../../../ecs/component/text_label_component.h"
#include "../../../ecs/component/component.h"

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
    return fpsClassInstance;
}

void fps_display_on_start(RBENativeScriptClass* nativeScriptClass) {
    FpsDisplayClassData* data = (FpsDisplayClassData*) nativeScriptClass->instance_data;
    printf("[C SCRIPT] - entity '%d' on start value = %d\n", nativeScriptClass->entity, data->value);
}

void fps_display_on_end(RBENativeScriptClass* nativeScriptClass) {}

void fps_display_update(RBENativeScriptClass* nativeScriptClass, float deltaTime) {
    // Update Text Label Component's Text
    static char fpsAmountBuffer[6];
    // FIXME: this is windows specific, needs to be replaced for other OS
    gcvt(rbe_engine_context_get()->averageFPS, 4, fpsAmountBuffer);
    TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(nativeScriptClass->entity, ComponentDataIndex_TEXT_LABEL);
    // NOTE(PetrFlajsingr): strcpy is deprecated
    strcpy_s(textLabelComponent->text, TEXT_LABEL_BUFFER_SIZE, "FPS: ");
    // NOTE(PetrFlajsingr): strcat is deprecated
    strcat_s(textLabelComponent->text, TEXT_LABEL_BUFFER_SIZE, fpsAmountBuffer);
}
