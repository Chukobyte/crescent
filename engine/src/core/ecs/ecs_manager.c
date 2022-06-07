#include "ecs_manager.h"

#include "component/component.h"
#include "system/ec_system.h"

void rbe_ecs_manager_initialize() {
    component_manager_initialize();
    rbe_ec_system_initialize();
}

void rbe_ecs_manager_finalize() {
    rbe_ec_system_finalize();
    component_manager_finalize();
}
