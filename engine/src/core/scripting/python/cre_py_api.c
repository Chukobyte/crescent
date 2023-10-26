#include "cre_py_api.h"

#include "../seika/src/utils/se_assert.h"

#include "../../ecs/entity/entity.h"
#include "../../ecs/component/node_component.h"
#include "../../ecs/component/component.h"

int cre_py_api_node_get_name(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);
    const CreEntity entity = (CreEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    pkpy_push_string(vm, pkpy_string(nodeComponent->name));
    return 1;
}
