#include "fps_display_class.h"

#include <string.h>
#include <stdlib.h>

#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/memory.h>
#include <seika/string.h>

#include "../native_script_class.h"
#include "../../../engine_context.h"
#include "../../../ecs/ecs_globals.h"
#include "../../../ecs/components/text_label_component.h"
#include "../../../ecs/component.h"

typedef struct FpsDisplayClassData {
    int value;
} FpsDisplayClassData;

CRENativeScriptClass* fps_display_create_new_instance(SkaEntity entity);
void fps_display_on_start(CRENativeScriptClass* nativeScriptClass);
void fps_display_on_end(CRENativeScriptClass* nativeScriptClass);
void fps_display_update(CRENativeScriptClass* nativeScriptClass, float deltaTime);

CRENativeScriptClass* fps_display_native_class_create_new() {
    CRENativeScriptClass* fpsClassInstance = fps_display_create_new_instance(SKA_NULL_ENTITY);
    fpsClassInstance->create_new_instance_func = fps_display_create_new_instance;
    return fpsClassInstance;
}

CRENativeScriptClass* fps_display_create_new_instance(SkaEntity entity) {
    CRENativeScriptClass* fpsClassInstance = cre_native_class_create_new(entity, "main", "FpsDisplay");
    fpsClassInstance->on_start_func = fps_display_on_start;
    fpsClassInstance->on_end_func = fps_display_on_end;
    fpsClassInstance->update_func = fps_display_update;
    FpsDisplayClassData* classData = SKA_MEM_ALLOCATE(FpsDisplayClassData);
    classData->value = 10;
    fpsClassInstance->instance_data = classData;
    fpsClassInstance->class_instance_size = (sizeof(CRENativeScriptClass*) + sizeof(FpsDisplayClassData*)) * 4;
    return fpsClassInstance;
}

void fps_display_on_start(CRENativeScriptClass* nativeScriptClass) {
    FpsDisplayClassData* data = (FpsDisplayClassData*) nativeScriptClass->instance_data;
//    printf("[C SCRIPT] - entity '%u' on start value = %d\n", nativeScriptClass->entity, data->value);
}

void fps_display_on_end(CRENativeScriptClass* nativeScriptClass) {}

void fps_display_update(CRENativeScriptClass* nativeScriptClass, float deltaTime) {
    // Update Text Label Component's Text
    static char fpsAmountBuffer[6];
    // FIXME: This is windows specific, needs to be replaced for other OS
    gcvt(cre_engine_context_get()->stats.averageFPS, 4, fpsAmountBuffer);
    TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(nativeScriptClass->entity, TEXT_LABEL_COMPONENT_INDEX);
    ska_strcpy(textLabelComponent->text, "FPS: ");
    ska_strcat(textLabelComponent->text, fpsAmountBuffer);
}
