#include "cre_pkpy_api.h"

#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "cre_pkpy.h"
#include "cre_pkpy_util.h"
#include "cre_pkpy_api_source.h"
#include "../../../ecs/entity/entity.h"
#include "../../../ecs/component/component.h"
#include "../../../ecs/component/node_component.h"
#include "../../../ecs/component/sprite_component.h"
#include "../../../ecs/component/animated_sprite_component.h"
#include "../../../ecs/component/text_label_component.h"
#include "../../../ecs/component/collider2d_component.h"
#include "../../../ecs/component/color_rect_component.h"
#include "../../../ecs/component/parallax_component.h"
#include "../../../scene/scene_utils.h"
#include "../../../ecs/component/script_component.h"
#include "cre_pkpy_entity_instance_cache.h"

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
// TODO: These are implemented in python, may want to move later..
//int cre_pkpy_api_node_create_event(pkpy_vm* vm);
//int cre_pkpy_api_node_subscribe_to_event(pkpy_vm* vm);
//int cre_pkpy_api_node_broadcast_event(pkpy_vm* vm);

// Node2D
int cre_pkpy_api_node2d_set_position(pkpy_vm* vm);
int cre_pkpy_api_node2d_add_to_position(pkpy_vm* vm);
int cre_pkpy_api_node2d_get_position(pkpy_vm* vm);
int cre_pkpy_api_node2d_get_global_position(pkpy_vm* vm);
int cre_pkpy_api_node2d_set_scale(pkpy_vm* vm);
int cre_pkpy_api_node2d_add_to_scale(pkpy_vm* vm);
int cre_pkpy_api_node2d_get_scale(pkpy_vm* vm);
int cre_pkpy_api_node2d_set_rotation(pkpy_vm* vm);
int cre_pkpy_api_node2d_add_to_rotation(pkpy_vm* vm);
int cre_pkpy_api_node2d_get_rotation(pkpy_vm* vm);
int cre_pkpy_api_node2d_set_z_index(pkpy_vm* vm);
int cre_pkpy_api_node2d_get_z_index(pkpy_vm* vm);
int cre_pkpy_api_node2d_set_z_index_relative_to_parent(pkpy_vm* vm);
int cre_pkpy_api_node2d_get_z_index_relative_to_parent(pkpy_vm* vm);
int cre_pkpy_api_node2d_set_ignore_camera(pkpy_vm* vm);
int cre_pkpy_api_node2d_get_ignore_camera(pkpy_vm* vm);

// Scene Tree
int cre_pkpy_api_scene_tree_change_scene(pkpy_vm* vm);
int cre_pkpy_api_scene_tree_get_root(pkpy_vm* vm);

// Scene Manager
int cre_pkpy_api_scene_manager_process_queued_creation_entities(pkpy_vm* vm);
int cre_pkpy_api_scene_manager_process_queued_scene_change(pkpy_vm* vm);

void cre_pkpy_api_load_internal_modules(pkpy_vm* vm) {
    // Load internal first
    cre_pkpy_util_create_module(vm, &(CrePPModule) {
        .name = "crescent_internal",
        .functions = {
                // Node
            {.signature = "node_new(class_path: str, class_name: str, node_type_flag: int) -> \"Node\"", .function = cre_pkpy_api_node_new},
            {.signature = "node_get_name(entity_id: int) -> str", .function = cre_pkpy_api_node_get_name},
            {.signature = "node_add_child(parent_entity_id: int, child_entity_id: int) -> None", .function = cre_pkpy_api_node_add_child},
            {.signature = "node_get_child(parent_entity_id: int, child_entity_name: str) -> Optional[\"Node\"]", .function = cre_pkpy_api_node_get_child},
            {.signature = "node_get_children(entity_id: int) -> Tuple[\"Node\", ...]", .function = cre_pkpy_api_node_get_children},
            {.signature = "node_get_parent(child_entity_id: int) -> Optional[\"Node\"]", .function = cre_pkpy_api_node_get_parent},
            {.signature = "node_queue_deletion(entity_id: int) -> None", .function = cre_pkpy_api_node_queue_deletion},
            {.signature = "node_is_queued_for_deletion(entity_id: int) -> bool", .function = cre_pkpy_api_node_is_queued_for_deletion},
            {.signature = "node_set_time_dilation(entity_id: int, dilation: float) -> None", .function = cre_pkpy_api_node_set_time_dilation},
            {.signature = "node_get_time_dilation(entity_id: int) -> float", .function = cre_pkpy_api_node_get_time_dilation},
            {.signature = "node_get_total_time_dilation(entity_id: int) -> float", .function = cre_pkpy_api_node_get_total_time_dilation},
            // Scene Tree
            {.signature = "scene_tree_change_scene(path: str) -> None", .function = cre_pkpy_api_scene_tree_change_scene},
            {.signature = "scene_tree_get_root()", .function = cre_pkpy_api_scene_tree_get_root},
            // Scene Manager
            {.signature = "_scene_manager_process_queued_creation_entities() -> None", .function = cre_pkpy_api_scene_manager_process_queued_creation_entities},
            {.signature = "_scene_manager_process_queued_scene_change() -> None", .function = cre_pkpy_api_scene_manager_process_queued_scene_change},

            { NULL, NULL },
        }
    });

    // Now load front facing api
    cre_pkpy_util_create_from_string(vm, CRE_PKPY_MODULE_NAME_CRESCENT, CRE_PKPY_CRESCENT_SOURCE);
}

//--- NODE ---//

void set_node_component_from_type(CreEntity entity, const char* classPath, const char* className, NodeBaseType baseType) {

    // Set components that should be set for a base node (that has invoked .new() from scripting)
    cre_component_manager_set_component(entity, CreComponentDataIndex_NODE, node_component_create_ex(className, baseType));
    cre_component_manager_set_component(entity, CreComponentDataIndex_SCRIPT, script_component_create_ex(classPath, className, ScriptContextType_PYTHON));

    const NodeBaseInheritanceType inheritanceType = node_get_type_inheritance(baseType);

    if ((NodeBaseInheritanceType_NODE2D & inheritanceType) == NodeBaseInheritanceType_NODE2D) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_TRANSFORM_2D, transform2d_component_create());
    }
    if ((NodeBaseInheritanceType_SPRITE & inheritanceType) == NodeBaseInheritanceType_SPRITE) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_SPRITE, sprite_component_create());
    }
    if ((NodeBaseInheritanceType_ANIMATED_SPRITE & inheritanceType) == NodeBaseInheritanceType_ANIMATED_SPRITE) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_ANIMATED_SPRITE, animated_sprite_component_create());
    }
    if ((NodeBaseInheritanceType_TEXT_LABEL & inheritanceType) == NodeBaseInheritanceType_TEXT_LABEL) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_TEXT_LABEL, text_label_component_create());
    }
    if ((NodeBaseInheritanceType_COLLIDER2D & inheritanceType) == NodeBaseInheritanceType_COLLIDER2D) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_COLLIDER_2D, collider2d_component_create());
    }
    if ((NodeBaseInheritanceType_COLOR_RECT & inheritanceType) == NodeBaseInheritanceType_COLOR_RECT) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_COLOR_RECT, color_rect_component_create());
    }
    if ((NodeBaseInheritanceType_PARALLAX & inheritanceType) == NodeBaseInheritanceType_PARALLAX) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_PARALLAX, parallax_component_create());
    }
}

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
    const CreEntity entity = cre_pkpy_entity_instance_cache_create_new_entity(vm, classPath, className, CRE_NULL_ENTITY);
    SceneTreeNode* newNode = cre_scene_tree_create_tree_node(entity, NULL);
    cre_scene_manager_stage_child_node_to_be_added_later(newNode);

    set_node_component_from_type(entity, classPath, className, (NodeBaseType)pyNodeTypeFlag);

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

    const CreEntity childEntity = cre_scene_manager_get_entity_child_by_name((CreEntity)parentEntityId, childEntityName.data);
    if (cre_pkpy_entity_instance_cache_has_entity(vm, childEntity)) {
        cre_pkpy_entity_instance_cache_push_or_add_default_entity_instance(vm, childEntity);
        return 1;
    }
    return 0;
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
        cre_pkpy_entity_instance_cache_push_or_add_default_entity_instance(vm, childTreeNode->entity);
    }

    return (int)parentTreeNode->childCount;
}

int cre_pkpy_api_node_get_parent(pkpy_vm* vm) {
    int childEntityId;
    pkpy_to_int(vm, 0, &childEntityId);

    const CreEntity childEntity = (CreEntity)childEntityId;
    if (cre_scene_manager_has_entity_tree_node(childEntity)) {
        const SceneTreeNode* treeNode = cre_scene_manager_get_entity_tree_node(childEntity);
        cre_pkpy_entity_instance_cache_push_or_add_default_entity_instance(vm, treeNode->entity);
        return 1;
    }
    return 0;
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

//--- NODE2D ---//

void cre_pkpy_update_entity_local_position(CreEntity entity, SEVector2* position) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,CreComponentDataIndex_TRANSFORM_2D);
    const SEVector2 prevPosition = transformComp->localTransform.position;
    transformComp->localTransform.position.x = position->x;
    transformComp->localTransform.position.y = position->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.position.x != prevPosition.x || transformComp->localTransform.position.y != prevPosition.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void cre_pkpy_update_entity_local_scale(CreEntity entity, SEVector2 * scale) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,CreComponentDataIndex_TRANSFORM_2D);
    const SEVector2 prevScale = transformComp->localTransform.scale;
    transformComp->localTransform.scale.x = scale->x;
    transformComp->localTransform.scale.y = scale->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.scale.x != prevScale.x || transformComp->localTransform.scale.y != prevScale.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void cre_pkpy_update_entity_local_rotation(CreEntity entity, float rotation) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    const float prevRotation = transformComp->localTransform.rotation;
    transformComp->localTransform.rotation = rotation;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.rotation != prevRotation) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

int cre_pkpy_api_node2d_set_position(pkpy_vm* vm) {
    int entityId;
    double positionX;
    double positionY;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_float(vm, 1, &positionX);
    pkpy_to_float(vm, 2, &positionY);

    const CreEntity entity = (CreEntity)entityId;
    cre_pkpy_update_entity_local_position(entity, &(SEVector2){ (float)positionX, (float)positionY });
    return 0;
}

int cre_pkpy_api_node2d_add_to_position(pkpy_vm* vm) {
    int entityId;
    double positionX;
    double positionY;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_float(vm, 1, &positionX);
    pkpy_to_float(vm, 2, &positionY);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    cre_pkpy_update_entity_local_position(entity, &(SEVector2){
            transformComp->localTransform.position.x + (float)positionX,
            transformComp->localTransform.position.y + (float)positionY
    });
    return 0;
}

int cre_pkpy_api_node2d_get_position(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_float(vm, (double)transformComp->localTransform.position.x);
    pkpy_push_float(vm, (double)transformComp->localTransform.position.y);
    return 2;
}

int cre_pkpy_api_node2d_get_global_position(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    SETransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transformComp);
    pkpy_push_float(vm, (double)globalTransform->position.x);
    pkpy_push_float(vm, (double)globalTransform->position.y);
    return 2;
}

int cre_pkpy_api_node2d_set_scale(pkpy_vm* vm) {
    int entityId;
    double scaleX;
    double scaleY;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_float(vm, 1, &scaleX);
    pkpy_to_float(vm, 2, &scaleY);

    const CreEntity entity = (CreEntity)entityId;
    cre_pkpy_update_entity_local_scale(entity, &(SEVector2){(float)scaleX, (float)scaleY });
    return 0;
}

int cre_pkpy_api_node2d_add_to_scale(pkpy_vm* vm) {
    int entityId;
    double scaleX;
    double scaleY;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_float(vm, 1, &scaleX);
    pkpy_to_float(vm, 2, &scaleY);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    cre_pkpy_update_entity_local_scale(entity, &(SEVector2){
        transformComp->localTransform.scale.x + (float)scaleX,
        transformComp->localTransform.scale.y + (float)scaleY
    });
    return 0;
}

int cre_pkpy_api_node2d_get_scale(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_float(vm, (double)transformComp->localTransform.scale.x);
    pkpy_push_float(vm, (double)transformComp->localTransform.scale.y);
    return 2;
}

int cre_pkpy_api_node2d_set_rotation(pkpy_vm* vm) {
    int entityId;
    double rotation;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_float(vm, 1, &rotation);

    const CreEntity entity = (CreEntity)entityId;
    cre_pkpy_update_entity_local_rotation(entity, (float)rotation);
    return 0;
}

int cre_pkpy_api_node2d_add_to_rotation(pkpy_vm* vm) {
    int entityId;
    double rotation;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_float(vm, 1, &rotation);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    cre_pkpy_update_entity_local_rotation(entity, transformComp->localTransform.rotation + (float)rotation);
    return 0;
}

int cre_pkpy_api_node2d_get_rotation(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_float(vm, (double)transformComp->localTransform.rotation);
    return 1;
}

int cre_pkpy_api_node2d_set_z_index(pkpy_vm* vm) {
    int entityId;
    int zIndex;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_int(vm, 1, &zIndex);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    transformComp->zIndex = zIndex;
    return 0;
}

int cre_pkpy_api_node2d_get_z_index(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_int(vm, transformComp->zIndex);
    return 1;
}

int cre_pkpy_api_node2d_set_z_index_relative_to_parent(pkpy_vm* vm) {
    int entityId;
    bool zIndexRelativeToParent;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_bool(vm, 1, &zIndexRelativeToParent);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    transformComp->isZIndexRelativeToParent = zIndexRelativeToParent;
    return 0;
}

int cre_pkpy_api_node2d_get_z_index_relative_to_parent(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_bool(vm, transformComp->isZIndexRelativeToParent);
    return 1;
}

int cre_pkpy_api_node2d_set_ignore_camera(pkpy_vm* vm) {
    int entityId;
    bool ignoreCamera;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_to_bool(vm, 1, &ignoreCamera);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    transformComp->ignoreCamera = ignoreCamera;
    return 0;
}

int cre_pkpy_api_node2d_get_ignore_camera(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);

    const CreEntity entity = (CreEntity)entityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_bool(vm, transformComp->ignoreCamera);
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
    cre_pkpy_entity_instance_cache_push_or_add_default_entity_instance(vm, rootNode->entity);
    return 1;
}

//--- SCENE MANAGER ---//
int cre_pkpy_api_scene_manager_process_queued_creation_entities(pkpy_vm* vm) {
    cre_scene_manager_process_queued_creation_entities();
    return 0;
}

int cre_pkpy_api_scene_manager_process_queued_scene_change(pkpy_vm* vm) {
    cre_scene_manager_process_queued_scene_change();
    return 0;
}
