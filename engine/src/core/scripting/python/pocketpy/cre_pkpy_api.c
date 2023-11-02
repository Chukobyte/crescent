#include "cre_pkpy_api.h"

#include "../seika/src/utils/se_assert.h"

#include "cre_pkpy_util.h"
#include "cre_pkpy_api_source.h"
#include "../../../ecs/entity/entity.h"
#include "../../../ecs/component/node_component.h"
#include "../../../ecs/component/component.h"
#include "../../../scene/scene_utils.h"

int cre_pkpy_api_node_get_name(pkpy_vm* vm);
int cre_pkpy_api_node_get_children(pkpy_vm* vm);

void cre_pkpy_api_load_internal_modules(pkpy_vm* vm) {
    // Load internal first
    cre_pkpy_util_create_module(vm, &(CrePPModule) {
            .name = "crescent_internal",
            .functionCount = 2,
            .functions = {
                    {.signature = "node_get_name(entity_id: int) -> str", .function = cre_pkpy_api_node_get_name},
                    {.signature = "node_get_children(entity_id: int) -> Tuple[int, ...]", .function = cre_pkpy_api_node_get_children},
            }
    });

    // Now load front facing api
    pkpy_push_module(vm, "crescent");
    pkpy_exec_2(vm, CRE_PKPY_CRESCENT_SOURCE, "crescent.py", 0, "crescent");
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    SE_ASSERT(pkpy_stack_size(vm) == 0);
}

int cre_pkpy_api_node_get_name(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    pkpy_push_string(vm, pkpy_string(nodeComponent->name));
    return 1;
}

int cre_pkpy_api_node_get_children(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    const SceneTreeNode* parentTreeNode = cre_scene_manager_get_entity_tree_node(entity);
    for (size_t i = 0; i < parentTreeNode->childCount; i++) {
        const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
        pkpy_push_int(vm, (int)childTreeNode->entity);
    }

    return (int)parentTreeNode->childCount;
}
