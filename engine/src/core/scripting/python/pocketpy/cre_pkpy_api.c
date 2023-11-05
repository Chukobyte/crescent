#include "cre_pkpy_api.h"

#include <string.h>

#include "../seika/src/utils/se_assert.h"

#include "cre_pkpy_util.h"
#include "cre_pkpy_api_source.h"
#include "../../../ecs/entity/entity.h"
#include "../../../ecs/component/node_component.h"
#include "../../../ecs/component/component.h"
#include "../../../scene/scene_utils.h"
#include "../../../ecs/system/ec_system.h"
#include "../../../ecs/component/script_component.h"

int cre_pkpy_api_node_new(pkpy_vm* vm);
int cre_pkpy_api_node_queue_deletion(pkpy_vm* vm);
int cre_pkpy_api_node_is_queued_for_deletion(pkpy_vm* vm);
int cre_pkpy_api_node_add_child(pkpy_vm* vm);
int cre_pkpy_api_node_get_child(pkpy_vm* vm);
int cre_pkpy_api_node_get_children(pkpy_vm* vm);
int cre_pkpy_api_node_get_parent(pkpy_vm* vm);
int cre_pkpy_api_node_get_name(pkpy_vm* vm);
int cre_pkpy_api_node_set_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_node_get_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_node_set_full_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_node_get_full_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_node_create_event(pkpy_vm* vm);
int cre_pkpy_api_node_subscribe_to_event(pkpy_vm* vm);
int cre_pkpy_api_node_broadcast_event(pkpy_vm* vm);

void cre_pkpy_api_load_internal_modules(pkpy_vm* vm) {
    // Load internal first
    cre_pkpy_util_create_module(vm, &(CrePPModule) {
        .name = "crescent_internal",
        .functions = {
            {.signature = "node_new(class_path: str, class_name: str, node_type_flag: int) -> int", .function = cre_pkpy_api_node_new},
            {.signature = "node_add_child(parent_entity_id: int, child_entity_id: int) -> None", .function = cre_pkpy_api_node_add_child},
            {.signature = "node_get_child(parent_entity_id: int, child_entity_name: str) -> int", .function = cre_pkpy_api_node_get_child},
            {.signature = "node_get_children(entity_id: int) -> Tuple[int, ...]", .function = cre_pkpy_api_node_get_children},
            {.signature = "node_queue_deletion(entity_id: int) -> None", .function = cre_pkpy_api_node_queue_deletion},
            {.signature = "node_is_queued_for_deletion(entity_id: int) -> bool", .function = cre_pkpy_api_node_is_queued_for_deletion},
            {.signature = "node_get_name(entity_id: int) -> str", .function = cre_pkpy_api_node_get_name},
            { NULL, NULL },
        }
    });

    // Now load front facing api
    pkpy_push_module(vm, "crescent");
    pkpy_exec_2(vm, CRE_PKPY_CRESCENT_SOURCE, "crescent.py", 0, "crescent");
    SE_ASSERT(!cre_pkpy_util_print_error_message(vm));
    pkpy_pop_top(vm);
    SE_ASSERT(pkpy_stack_size(vm) == 0);
}

int cre_pkpy_api_node_new(pkpy_vm* vm) {
    pkpy_CString pyClassPath;
    pkpy_CString pyClassName;
    int pyNodeTypeFlag;
    pkpy_to_string(vm, 0, &pyClassPath);
    pkpy_to_string(vm, 1, &pyClassName);
    pkpy_to_int(vm, 2, &pyNodeTypeFlag);

    SceneTreeNode* newNode = cre_scene_tree_create_tree_node(cre_ec_system_create_entity_uid(), NULL);
    cre_scene_manager_stage_child_node_to_be_added_later(newNode);

    NodeComponent* nodeComponent = node_component_create();
    strcpy(nodeComponent->name, pyClassName.data);
    nodeComponent->type = (NodeBaseType)pyNodeTypeFlag;
    cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_NODE, nodeComponent);

    cre_component_manager_set_component(newNode->entity, CreComponentDataIndex_SCRIPT, script_component_create(pyClassPath.data, pyClassName.data));

    pkpy_push_int(vm, (int)newNode->entity);
    return 1;
}

int cre_pkpy_api_node_get_name(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    pkpy_push_string(vm, pkpy_string(nodeComponent->name));
    return 1;
}

int cre_pkpy_api_node_add_child(pkpy_vm* vm) {
    int childEntityId;
    int parentEntityId;
    pkpy_to_int(vm, 0, &parentEntityId);
    pkpy_to_int(vm, 1, &childEntityId);

    CreEntity childEntity = (CreEntity)childEntityId;
    CreEntity parentEntity = (CreEntity)parentEntityId;
    cre_scene_manager_add_node_as_child(childEntity, parentEntity);
    return 0;
}

int cre_pkpy_api_node_get_child(pkpy_vm* vm) {
    int parentEntityId;
    pkpy_CString childEntityName;
    pkpy_to_int(vm, 0, &parentEntityId);
    pkpy_to_string(vm, 1, &childEntityName);

    int childEntityId = (int)cre_scene_manager_get_entity_child_by_name((CreEntity)parentEntityId, childEntityName.data);
    pkpy_to_int(vm, 0, &childEntityId);
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

int cre_pkpy_api_node_queue_deletion(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    if (!nodeComponent->queuedForDeletion) {
        if (cre_scene_manager_has_entity_tree_node(entity)) {
            SceneTreeNode* node = cre_scene_manager_get_entity_tree_node(entity);
            cre_queue_destroy_tree_node_entity_all(node);
        } else {
            se_logger_warn("Entity not found in tree, skipping queue deletion.");
        }
    } else {
        se_logger_warn("Node '%s' already queued for deletion!", nodeComponent->name);
    }
    return 0;
}

int cre_pkpy_api_node_is_queued_for_deletion(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    pkpy_push_bool(vm, nodeComponent->queuedForDeletion);
    return 1;
}
