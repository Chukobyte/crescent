#include "fps_display_class.h"

#include "../native_script_class.h"
#include "../../../memory/rbe_mem.h"
#include "../../../utils/logger.h"

typedef struct FpsDisplayClassData {
    int value;
} FpsDisplayClassData;

RBENativeScriptClass* fps_display_create_new_instance();
void fps_display_on_start(void* inst_data);

RBENativeScriptClass* fps_display_native_class_create_new() {
    RBENativeScriptClass* fpsClassInstance = fps_display_create_new_instance();
    fpsClassInstance->create_new_instance_func = fps_display_create_new_instance;
    return fpsClassInstance;
}

RBENativeScriptClass* fps_display_create_new_instance() {
    RBENativeScriptClass* fpsClassInstance = rbe_native_class_create_new("main", "FpsDisplay");
    FpsDisplayClassData* classData = RBE_MEM_ALLOCATE(FpsDisplayClassData);
    classData->value = 10;
    fpsClassInstance->on_start_func = fps_display_on_start;
    fpsClassInstance->instance_data = classData;
    fpsClassInstance->class_instance_size = (sizeof(RBENativeScriptClass*) + sizeof(FpsDisplayClassData*)) * 2;
    return fpsClassInstance;
}

void fps_display_on_start(void* inst_data) {
    FpsDisplayClassData* data = (FpsDisplayClassData*) inst_data;
    rbe_logger_error("NOT AN ERROR - on start value = %d", data->value);
}
