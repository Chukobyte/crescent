#include "cre_pkpy_api.h"

#include <string.h>

#include "../seika/utils/se_string_util.h"
#include "../seika/utils/se_assert.h"

#include "cre_pkpy_util.h"
#include "cre_pkpy_api_source.h"
#include "../../../ecs/entity/entity.h"
#include "../../../ecs/component/node_component.h"
#include "../../../ecs/component/component.h"
#include "../../../scene/scene_utils.h"
#include "../../../ecs/system/ec_system.h"
#include "../../../ecs/component/script_component.h"
#include "cre_pkpy_entity_instance_cache.h"

#define CRE_PKPY_NULL_ENTITY (-1)

// Node
int cre_pkpy_api_node_new(pkpy_vm* vm);
int cre_pkpy_api_node_get_name(pkpy_vm* vm);
int cre_pkpy_api_node_add_child(pkpy_vm* vm);
int cre_pkpy_api_node_get_child(pkpy_vm* vm);
int cre_pkpy_api_node_get_children(pkpy_vm* vm);
int cre_pkpy_api_node_get_parent(pkpy_vm* vm);
int cre_pkpy_api_node_queue_deletion(pkpy_vm* vm);
int cre_pkpy_api_node_is_queued_for_deletion(pkpy_vm* vm);
int cre_pkpy_api_node_set_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_node_get_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_node_get_total_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_node_create_event(pkpy_vm* vm);
int cre_pkpy_api_node_subscribe_to_event(pkpy_vm* vm);
int cre_pkpy_api_node_broadcast_event(pkpy_vm* vm);

// Scene Tree
int cre_pkpy_api_scene_tree_change_scene(pkpy_vm* vm);
int cre_pkpy_api_scene_tree_get_root(pkpy_vm* vm);

void cre_pkpy_api_load_internal_modules(pkpy_vm* vm) {
    // Load internal first
    cre_pkpy_util_create_module(vm, &(CrePPModule) {
        .name = "crescent_internal",
        .functions = {
                // Node
            {.signature = "node_new(class_path: str, class_name: str, node_type_flag: int) -> \"Node\"", .function = cre_pkpy_api_node_new},
            {.signature = "node_get_name(entity_id: int) -> str", .function = cre_pkpy_api_node_get_name},
            {.signature = "node_add_child(parent_entity_id: int, child_entity_id: int) -> None", .function = cre_pkpy_api_node_add_child},
            {.signature = "node_get_child(parent_entity_id: int, child_entity_name: str) -> int", .function = cre_pkpy_api_node_get_child},
            {.signature = "node_get_children(entity_id: int) -> Tuple[int, ...]", .function = cre_pkpy_api_node_get_children},
            {.signature = "node_get_parent(child_entity_id: int) -> int", .function = cre_pkpy_api_node_get_parent},
            {.signature = "node_queue_deletion(entity_id: int) -> None", .function = cre_pkpy_api_node_queue_deletion},
            {.signature = "node_is_queued_for_deletion(entity_id: int) -> bool", .function = cre_pkpy_api_node_is_queued_for_deletion},
            {.signature = "node_set_time_dilation(entity_id: int, dilation: float) -> None", .function = cre_pkpy_api_node_set_time_dilation},
            {.signature = "node_get_time_dilation(entity_id: int) -> float", .function = cre_pkpy_api_node_get_time_dilation},
            {.signature = "node_get_total_time_dilation(entity_id: int) -> float", .function = cre_pkpy_api_node_get_total_time_dilation},
            // Scene Tree
            {.signature = "scene_tree_change_scene(path: str) -> None", .function = cre_pkpy_api_scene_tree_change_scene},
            {.signature = "scene_tree_get_root()", .function = cre_pkpy_api_scene_tree_get_root},

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

//--- NODE ---//

int cre_pkpy_api_node_new(pkpy_vm* vm) {
    pkpy_CString pyClassPath;
    pkpy_CString pyClassName;
    int pyNodeTypeFlag;
    pkpy_to_string(vm, 0, &pyClassPath);
    pkpy_to_string(vm, 1, &pyClassName);
    pkpy_to_int(vm, 2, &pyNodeTypeFlag);

    char classPath[64];
    char className[32];
    se_str_trim_by_size(pyClassPath.data, classPath, pyClassPath.size);
    se_str_trim_by_size(pyClassName.data, className, pyClassName.size);
    const CreEntity entity = cre_pkpy_entity_instance_cache_create_new_entity(vm, classPath, className);
    SceneTreeNode* newNode = cre_scene_tree_create_tree_node(entity, NULL);
    cre_scene_manager_stage_child_node_to_be_added_later(newNode);

    NodeComponent* nodeComponent = node_component_create();
    strcpy(nodeComponent->name, className);
    nodeComponent->type = (NodeBaseType)pyNodeTypeFlag;
    cre_component_manager_set_component(entity, CreComponentDataIndex_NODE, nodeComponent);

    cre_component_manager_set_component(entity, CreComponentDataIndex_SCRIPT, script_component_create(classPath, className));

    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
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

    int childEntityId = CRE_PKPY_NULL_ENTITY;
    const CreEntity childEntity = cre_scene_manager_get_entity_child_by_name((CreEntity)parentEntityId, childEntityName.data);
    if (childEntity != CRE_NULL_ENTITY) {
        childEntityId = (int)childEntity;
    }
    pkpy_push_int(vm, childEntityId);
    return 1;
}

int cre_pkpy_api_node_get_children(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    if (!cre_scene_manager_has_entity_tree_node(entity)) {
        return 0;
    }

    const SceneTreeNode* parentTreeNode = cre_scene_manager_get_entity_tree_node(entity);
    for (size_t i = 0; i < parentTreeNode->childCount; i++) {
        const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
        pkpy_push_int(vm, (int)childTreeNode->entity);
    }

    return (int)parentTreeNode->childCount;
}

int cre_pkpy_api_node_get_parent(pkpy_vm* vm) {
    int childEntityId;
    pkpy_to_int(vm, 0, &childEntityId);

    const CreEntity childEntity = (CreEntity)childEntityId;
    int parentEntityId = CRE_PKPY_NULL_ENTITY;
    if (cre_scene_manager_has_entity_tree_node(childEntity)) {
        const SceneTreeNode* treeNode = cre_scene_manager_get_entity_tree_node(childEntity);
        parentEntityId = (int)treeNode->parent->entity;
    }
    pkpy_push_int(vm, parentEntityId);
    return 1;
}

int cre_pkpy_api_node_queue_deletion(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    const NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
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

int cre_pkpy_api_node_set_time_dilation(pkpy_vm* vm) {
    int entityId;
    double timeDilation;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_float(vm, 1, &timeDilation);

    const CreEntity entity = (CreEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    nodeComponent->timeDilation.value = (float)timeDilation;
    cre_scene_manager_invalidate_time_dilation_nodes_with_children(entity);

    return 0;
}

int cre_pkpy_api_node_get_time_dilation(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    const double timeDilation = (double)nodeComponent->timeDilation.value;
    pkpy_push_float(vm, timeDilation);
    return 1;
}

int cre_pkpy_api_node_get_total_time_dilation(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    const double totalTimeDilation = (double)cre_scene_manager_get_node_full_time_dilation(entity);
    pkpy_push_float(vm, totalTimeDilation);
    return 1;
}

//--- SCENE TREE ---//

int cre_pkpy_api_scene_tree_change_scene(pkpy_vm* vm) {
    pkpy_CString scenePath;
    pkpy_to_string(vm, 0, &scenePath);

    cre_scene_manager_queue_scene_change(scenePath.data);
    return 0;
}

int cre_pkpy_api_scene_tree_get_root(pkpy_vm* vm) {
    SceneTreeNode* rootNode = cre_scene_manager_get_active_scene_root();
    SE_ASSERT(rootNode != NULL);
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, rootNode->entity);
    return 0;
}
