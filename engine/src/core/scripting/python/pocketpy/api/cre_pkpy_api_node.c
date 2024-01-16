#include "cre_pkpy_api_node.h"

#include <pocketpy_c.h>

#include <seika/rendering/renderer.h>
#include <seika/rendering/shader/shader_cache.h>
#include <seika/asset/asset_manager.h>
#include <seika/utils/logger.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>

#include "../cre_pkpy_entity_instance_cache.h"
#include "../cre_pkpy_script_context.h"
#include "../../../../scene/scene_manager.h"
#include "../../../../ecs/component/sprite_component.h"
#include "../../../../ecs/component/animated_sprite_component.h"
#include "../../../../ecs/component/text_label_component.h"
#include "../../../../ecs/component/collider2d_component.h"
#include "../../../../ecs/component/color_rect_component.h"
#include "../../../../ecs/component/parallax_component.h"
#include "../../../../ecs/component/script_component.h"
#include "../../../../ecs/system/ec_system.h"
#include "../../../../ecs/component/particles2d_component.h"

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
    if ((NodeBaseInheritanceType_PARTICLES2D & inheritanceType) == NodeBaseInheritanceType_PARTICLES2D) {
        cre_component_manager_set_component(entity, CreComponentDataIndex_PARTICLES_2D, particles2d_component_create());
    }
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
    const CreEntity entity = cre_pkpy_entity_instance_cache_create_new_entity(vm, classPath, className, cre_ec_system_create_entity_uid());
    SceneTreeNode* newNode = cre_scene_tree_create_tree_node(entity, NULL);
    cre_scene_manager_stage_child_node_to_be_added_later(newNode);

    set_node_component_from_type(entity, classPath, className, (NodeBaseType)pyNodeTypeFlag);

    cre_pkpy_script_context_create_instance(entity, classPath, className);
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
    return 1;
}

int cre_pkpy_api_node_get_name(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
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
    if (childEntity != CRE_NULL_ENTITY) {
        cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(childEntity);
        return 1;
    }
    return 0;
}

int cre_pkpy_api_node_get_children(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    if (!cre_scene_manager_has_entity_tree_node(entity)) {
        return 0;
    }

    const SceneTreeNode* parentTreeNode = cre_scene_manager_get_entity_tree_node(entity);
    for (size_t i = 0; i < parentTreeNode->childCount; i++) {
        const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
        cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(childTreeNode->entity);
    }

    return (int)parentTreeNode->childCount;
}

int cre_pkpy_api_node_get_parent(pkpy_vm* vm) {
    int childEntityId;
    pkpy_to_int(vm, 0, &childEntityId);

    const CreEntity childEntity = (CreEntity)childEntityId;
    if (cre_scene_manager_has_entity_tree_node(childEntity)) {
        const SceneTreeNode* treeNode = cre_scene_manager_get_entity_tree_node(childEntity);
        cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(treeNode->entity);
        return 1;
    }
    return 0;
}

int cre_pkpy_api_node_queue_deletion(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
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
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    pkpy_push_bool(vm, nodeComponent->queuedForDeletion);
    return 1;
}

int cre_pkpy_api_node_set_time_dilation(pkpy_vm* vm) {
    int pyEntityId;
    double timeDilation;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &timeDilation);

    const CreEntity entity = (CreEntity)pyEntityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    nodeComponent->timeDilation.value = (float)timeDilation;
    cre_scene_manager_invalidate_time_dilation_nodes_with_children(entity);

    return 0;
}

int cre_pkpy_api_node_get_time_dilation(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
    const double timeDilation = (double)nodeComponent->timeDilation.value;
    pkpy_push_float(vm, timeDilation);
    return 1;
}

int cre_pkpy_api_node_get_total_time_dilation(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const double totalTimeDilation = (double)cre_scene_manager_get_node_full_time_dilation(entity);
    pkpy_push_float(vm, totalTimeDilation);
    return 1;
}

//--- NODE2D ---//

void cre_pkpy_update_entity_local_position(CreEntity entity, SKAVector2* position) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,CreComponentDataIndex_TRANSFORM_2D);
    const SKAVector2 prevPosition = transformComp->localTransform.position;
    transformComp->localTransform.position.x = position->x;
    transformComp->localTransform.position.y = position->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.position.x != prevPosition.x || transformComp->localTransform.position.y != prevPosition.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void cre_pkpy_update_entity_local_scale(CreEntity entity, SKAVector2 * scale) {
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,CreComponentDataIndex_TRANSFORM_2D);
    const SKAVector2 prevScale = transformComp->localTransform.scale;
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
    int pyEntityId;
    double positionX;
    double positionY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &positionX);
    pkpy_to_float(vm, 2, &positionY);

    const CreEntity entity = (CreEntity)pyEntityId;
    cre_pkpy_update_entity_local_position(entity, &(SKAVector2){ (float)positionX, (float)positionY });
    return 0;
}

int cre_pkpy_api_node2d_add_to_position(pkpy_vm* vm) {
    int pyEntityId;
    double positionX;
    double positionY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &positionX);
    pkpy_to_float(vm, 2, &positionY);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    cre_pkpy_update_entity_local_position(entity, &(SKAVector2){
            transformComp->localTransform.position.x + (float)positionX,
            transformComp->localTransform.position.y + (float)positionY
    });
    return 0;
}

int cre_pkpy_api_node2d_get_position(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_float(vm, (double)transformComp->localTransform.position.x);
    pkpy_push_float(vm, (double)transformComp->localTransform.position.y);
    return 2;
}

int cre_pkpy_api_node2d_get_global_position(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    SKATransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transformComp);
    pkpy_push_float(vm, (double)globalTransform->position.x);
    pkpy_push_float(vm, (double)globalTransform->position.y);
    return 2;
}

int cre_pkpy_api_node2d_set_scale(pkpy_vm* vm) {
    int pyEntityId;
    double scaleX;
    double scaleY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &scaleX);
    pkpy_to_float(vm, 2, &scaleY);

    const CreEntity entity = (CreEntity)pyEntityId;
    cre_pkpy_update_entity_local_scale(entity, &(SKAVector2){(float)scaleX, (float)scaleY });
    return 0;
}

int cre_pkpy_api_node2d_add_to_scale(pkpy_vm* vm) {
    int pyEntityId;
    double scaleX;
    double scaleY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &scaleX);
    pkpy_to_float(vm, 2, &scaleY);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    cre_pkpy_update_entity_local_scale(entity, &(SKAVector2){
            transformComp->localTransform.scale.x + (float)scaleX,
            transformComp->localTransform.scale.y + (float)scaleY
    });
    return 0;
}

int cre_pkpy_api_node2d_get_scale(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_float(vm, (double)transformComp->localTransform.scale.x);
    pkpy_push_float(vm, (double)transformComp->localTransform.scale.y);
    return 2;
}

int cre_pkpy_api_node2d_set_rotation(pkpy_vm* vm) {
    int pyEntityId;
    double rotation;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &rotation);

    const CreEntity entity = (CreEntity)pyEntityId;
    cre_pkpy_update_entity_local_rotation(entity, (float)rotation);
    return 0;
}

int cre_pkpy_api_node2d_add_to_rotation(pkpy_vm* vm) {
    int pyEntityId;
    double rotation;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &rotation);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    cre_pkpy_update_entity_local_rotation(entity, transformComp->localTransform.rotation + (float)rotation);
    return 0;
}

int cre_pkpy_api_node2d_get_rotation(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_float(vm, (double)transformComp->localTransform.rotation);
    return 1;
}

int cre_pkpy_api_node2d_set_z_index(pkpy_vm* vm) {
    int pyEntityId;
    int zIndex;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &zIndex);

    const CreEntity entity = (CreEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    transformComp->zIndex = zIndex;
    return 0;
}

int cre_pkpy_api_node2d_get_z_index(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_int(vm, transformComp->zIndex);
    return 1;
}

int cre_pkpy_api_node2d_set_z_index_relative_to_parent(pkpy_vm* vm) {
    int pyEntityId;
    bool zIndexRelativeToParent;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &zIndexRelativeToParent);

    const CreEntity entity = (CreEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    transformComp->isZIndexRelativeToParent = zIndexRelativeToParent;
    return 0;
}

int cre_pkpy_api_node2d_get_z_index_relative_to_parent(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_bool(vm, transformComp->isZIndexRelativeToParent);
    return 1;
}

int cre_pkpy_api_node2d_set_ignore_camera(pkpy_vm* vm) {
    int pyEntityId;
    bool ignoreCamera;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &ignoreCamera);

    const CreEntity entity = (CreEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    transformComp->ignoreCamera = ignoreCamera;
    return 0;
}

int cre_pkpy_api_node2d_get_ignore_camera(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TRANSFORM_2D);
    pkpy_push_bool(vm, transformComp->ignoreCamera);
    return 1;
}

//--- SPRITE ---//

int cre_pkpy_api_sprite_get_texture(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    SETexture* texture = spriteComponent->texture; // TODO: Make const once applied to se_texture_get_wrap function params
    pkpy_push_string(vm, pkpy_string(texture->fileName));
    pkpy_push_string(vm, pkpy_string(se_texture_get_wrap_s_string(texture)));
    pkpy_push_string(vm, pkpy_string(se_texture_get_wrap_t_string(texture)));
    pkpy_push_bool(vm, texture->applyNearestNeighbor);
    return 4;
}

int cre_pkpy_api_sprite_set_texture(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_CString pyTexturePath;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyTexturePath);

    const CreEntity entity = (CreEntity)pyEntityId;
    const char* texturePath = pyTexturePath.data;
    SE_ASSERT_FMT(se_asset_manager_has_texture(texturePath), "Doesn't have texture with file path '%s'", texturePath);
    SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    spriteComponent->texture = se_asset_manager_get_texture(texturePath);
    return 0;
}

int cre_pkpy_api_sprite_get_draw_source(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    pkpy_push_float(vm, (double)spriteComponent->drawSource.x);
    pkpy_push_float(vm, (double)spriteComponent->drawSource.y);
    pkpy_push_float(vm, (double)spriteComponent->drawSource.w);
    pkpy_push_float(vm, (double)spriteComponent->drawSource.h);
    return 4;
}

int cre_pkpy_api_sprite_set_draw_source(pkpy_vm* vm) {
    int pyEntityId;
    double pyDrawSourceX;
    double pyDrawSourceY;
    double pyDrawSourceW;
    double pyDrawSourceH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyDrawSourceX);
    pkpy_to_float(vm, 2, &pyDrawSourceY);
    pkpy_to_float(vm, 3, &pyDrawSourceW);
    pkpy_to_float(vm, 4, &pyDrawSourceH);

    const CreEntity entity = (CreEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    spriteComponent->drawSource = (SKARect2){ (float)pyDrawSourceX, (float)pyDrawSourceY, (float)pyDrawSourceW, (float)pyDrawSourceH };
    return 0;
}

int cre_pkpy_api_sprite_get_flip_h(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    pkpy_push_bool(vm, spriteComponent->flipH);
    return 1;
}

int cre_pkpy_api_sprite_set_flip_h(pkpy_vm* vm) {
    int pyEntityId;
    bool pySetFlipH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipH);

    const CreEntity entity = (CreEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    spriteComponent->flipH = pySetFlipH;
    return 0;
}

int cre_pkpy_api_sprite_get_flip_v(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    pkpy_push_bool(vm, spriteComponent->flipV);
    return 1;
}

int cre_pkpy_api_sprite_set_flip_v(pkpy_vm* vm) {
    int pyEntityId;
    bool pySetFlipV;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipV);

    const CreEntity entity = (CreEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    spriteComponent->flipV = pySetFlipV;
    return 0;
}

int cre_pkpy_api_sprite_get_modulate(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    pkpy_push_float(vm, (double)spriteComponent->modulate.r);
    pkpy_push_float(vm, (double)spriteComponent->modulate.g);
    pkpy_push_float(vm, (double)spriteComponent->modulate.b);
    pkpy_push_float(vm, (double)spriteComponent->modulate.a);
    return 4;
}

int cre_pkpy_api_sprite_set_modulate(pkpy_vm* vm) {
    int pyEntityId;
    double pyModulateR;
    double pyModulateG;
    double pyModulateB;
    double pyModulateA;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyModulateR);
    pkpy_to_float(vm, 2, &pyModulateG);
    pkpy_to_float(vm, 3, &pyModulateB);
    pkpy_to_float(vm, 4, &pyModulateA);

    const CreEntity entity = (CreEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    spriteComponent->modulate = (SKAColor){(float)pyModulateR, (float)pyModulateG, (float)pyModulateB, (float)pyModulateA };
    return 0;
}

int cre_pkpy_api_sprite_get_origin(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    pkpy_push_float(vm, (double)spriteComponent->origin.x);
    pkpy_push_float(vm, (double)spriteComponent->origin.y);
    return 2;
}

int cre_pkpy_api_sprite_set_origin(pkpy_vm* vm) {
    int pyEntityId;
    double pyOriginX;
    double pyOriginY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyOriginX);
    pkpy_to_float(vm, 2, &pyOriginY);

    const CreEntity entity = (CreEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    spriteComponent->origin = (SKAVector2){ (float)pyOriginX, (float)pyOriginY };
    return 0;
}

int cre_pkpy_api_sprite_get_shader_instance(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    const int pyShaderInstanceId = spriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID ? (int)spriteComponent->shaderInstanceId : -1;
    pkpy_push_int(vm, pyShaderInstanceId);
    return 1;
}

int cre_pkpy_api_sprite_set_shader_instance(pkpy_vm* vm) {
    int pyEntityId;
    int pyShaderInstanceId;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyShaderInstanceId);

    const CreEntity entity = (CreEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_SPRITE);
    spriteComponent->shaderInstanceId = (SEShaderInstanceId)pyShaderInstanceId;
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(spriteComponent->shaderInstanceId);
    se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
    return 0;
}

//--- ANIMATED SPRITE ---//

int cre_pkpy_api_animated_sprite_play(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_CString pyAnimationName;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyAnimationName);

    const CreEntity entity = (CreEntity)pyEntityId;
    const char* animationName = pyAnimationName.data;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    const bool hasSuccessfullyPlayedAnimation = animated_sprite_component_play_animation(animatedSpriteComponent, animationName);
    pkpy_push_bool(vm, hasSuccessfullyPlayedAnimation);
    return 1;
}

int cre_pkpy_api_animated_sprite_stop(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->isPlaying = false;
    return 0;
}

int cre_pkpy_api_animated_sprite_set_current_animation_frame(pkpy_vm* vm) {
    int pyEntityId;
    int pyFrame;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyFrame);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->currentAnimation.currentFrame = ska_math_clamp_int(pyFrame, 0, animatedSpriteComponent->currentAnimation.frameCount - 1);
    return 0;
}

int cre_pkpy_api_animated_sprite_add_animation(pkpy_vm* vm) {
#define CRE_PKPY_API_ANIM_FRAME_STRIDE 6
    int pyEntityId;
    pkpy_CString pyAnimationName;
    int pyAnimationSpeed;
    bool pyAnimationDoesLoop;
    int pyFrameCount;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyAnimationName);
    pkpy_to_int(vm, 2, &pyAnimationSpeed);
    pkpy_to_bool(vm, 3, &pyAnimationDoesLoop);
    pkpy_to_int(vm, 4, &pyFrameCount);

    const CreEntity entity = (CreEntity)pyEntityId;
    const char* animationName = pyAnimationName.data;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    CreAnimation newAnim = { .frameCount = 0, .currentFrame = 0, .speed = pyAnimationSpeed, .name = {'\0'}, .doesLoop = pyAnimationDoesLoop, .isValid = true };
    se_strcpy(newAnim.name, animationName);
    // Unpack *args filled with frame data
    pkpy_unpack_sequence(vm, pyFrameCount * CRE_PKPY_API_ANIM_FRAME_STRIDE);
    for (int i = 0; i < pyFrameCount; i++) {
        int pyFrame;
        pkpy_CString pyTexturePath;
        double pyDrawSourceX;
        double pyDrawSourceY;
        double pyDrawSourceW;
        double pyDrawSourceH;
        const int dataIndex = i * CRE_PKPY_API_ANIM_FRAME_STRIDE;
        pkpy_to_int(vm, dataIndex, &pyFrame);
        pkpy_to_string(vm, dataIndex + 1, &pyTexturePath);
        pkpy_to_float(vm, dataIndex + 2, &pyDrawSourceX);
        pkpy_to_float(vm, dataIndex + 3, &pyDrawSourceY);
        pkpy_to_float(vm, dataIndex + 4, &pyDrawSourceW);
        pkpy_to_float(vm, dataIndex + 5, &pyDrawSourceH);

        const char* texturePath = pyTexturePath.data;
        CreAnimationFrame animationFrame = {
                .frame = pyFrame,
                .texture = se_asset_manager_get_texture(texturePath),
                .drawSource = { (float)pyDrawSourceX, (float)pyDrawSourceY, (float)pyDrawSourceW, (float)pyDrawSourceH }
        };
        newAnim.animationFrames[newAnim.frameCount++] = animationFrame;
    }
    // Finally add the animation
    animated_sprite_component_add_animation(animatedSpriteComponent, newAnim);\
    // If the only animation then set the current to that
    if (animatedSpriteComponent->animationCount == 1) {
        animated_sprite_component_set_animation(animatedSpriteComponent, newAnim.name);
    }
    return 0;
#undef CRE_PKPY_API_ANIM_FRAME_STRIDE
}

int cre_pkpy_api_animated_sprite_get_stagger_animation_start_times(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    pkpy_push_bool(vm, animatedSpriteComponent->staggerStartAnimationTimes);
    return 1;
}

int cre_pkpy_api_animated_sprite_set_stagger_animation_start_times(pkpy_vm* vm) {
    int pyEntityId;
    bool pyStaggerStartAnimationTimes;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pyStaggerStartAnimationTimes);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->staggerStartAnimationTimes = pyStaggerStartAnimationTimes;
    return 0;
}

int cre_pkpy_api_animated_sprite_get_flip_h(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    pkpy_push_bool(vm, animatedSpriteComponent->flipH);
    return 1;
}

int cre_pkpy_api_animated_sprite_set_flip_h(pkpy_vm* vm) {
    int pyEntityId;
    bool pySetFlipH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipH);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->flipH = pySetFlipH;
    return 0;
}

int cre_pkpy_api_animated_sprite_get_flip_v(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    pkpy_push_bool(vm, animatedSpriteComponent->flipV);
    return 1;
}

int cre_pkpy_api_animated_sprite_set_flip_v(pkpy_vm* vm) {
    int pyEntityId;
    bool pySetFlipV;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipV);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->flipV = pySetFlipV;
    return 0;
}

int cre_pkpy_api_animated_sprite_get_modulate(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    pkpy_push_float(vm, (double)animatedSpriteComponent->modulate.r);
    pkpy_push_float(vm, (double)animatedSpriteComponent->modulate.g);
    pkpy_push_float(vm, (double)animatedSpriteComponent->modulate.b);
    pkpy_push_float(vm, (double)animatedSpriteComponent->modulate.a);
    return 4;
}

int cre_pkpy_api_animated_sprite_set_modulate(pkpy_vm* vm) {
    int pyEntityId;
    double pyModulateR;
    double pyModulateG;
    double pyModulateB;
    double pyModulateA;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyModulateR);
    pkpy_to_float(vm, 2, &pyModulateG);
    pkpy_to_float(vm, 3, &pyModulateB);
    pkpy_to_float(vm, 4, &pyModulateA);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->modulate = (SKAColor){(float)pyModulateR, (float)pyModulateG, (float)pyModulateB, (float)pyModulateA };
    return 0;
}

int cre_pkpy_api_animated_sprite_get_origin(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    pkpy_push_float(vm, (double)animatedSpriteComponent->origin.x);
    pkpy_push_float(vm, (double)animatedSpriteComponent->origin.y);
    return 2;
}

int cre_pkpy_api_animated_sprite_set_origin(pkpy_vm* vm) {
    int pyEntityId;
    double pyOriginX;
    double pyOriginY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyOriginX);
    pkpy_to_float(vm, 2, &pyOriginY);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->origin = (SKAVector2){ (float)pyOriginX, (float)pyOriginY };
    return 0;
}

int cre_pkpy_api_animated_sprite_get_shader_instance(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    const int pyShaderInstanceId = animatedSpriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID ? (int)animatedSpriteComponent->shaderInstanceId : -1;
    pkpy_push_int(vm, pyShaderInstanceId);
    return 1;
}

int cre_pkpy_api_animated_sprite_set_shader_instance(pkpy_vm* vm) {
    int pyEntityId;
    int pyShaderInstanceId;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyShaderInstanceId);

    const CreEntity entity = (CreEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_ANIMATED_SPRITE);
    animatedSpriteComponent->shaderInstanceId = (SEShaderInstanceId)pyShaderInstanceId;
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(animatedSpriteComponent->shaderInstanceId);
    se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
    return 0;
}

//--- TEXT LABEL ---//

int cre_pkpy_api_text_label_get_text(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TEXT_LABEL);
    pkpy_push_string(vm, pkpy_string(textLabelComponent->text));
    return 1;
}

int cre_pkpy_api_text_label_set_text(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_CString pyText;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyText);

    static char textBuffer[2048];
    se_str_trim_by_size(pyText.data, textBuffer, pyText.size);
    const CreEntity entity = (CreEntity)pyEntityId;
    TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TEXT_LABEL);
    se_strcpy(textLabelComponent->text, textBuffer);
    return 0;
}

int cre_pkpy_api_text_label_get_color(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TEXT_LABEL);
    const int red = (int) (textLabelComponent->color.r * 255.0f);
    const int green = (int) (textLabelComponent->color.g * 255.0f);
    const int blue = (int) (textLabelComponent->color.b * 255.0f);
    const int alpha = (int) (textLabelComponent->color.a * 255.0f);
    pkpy_push_int(vm, red);
    pkpy_push_int(vm, green);
    pkpy_push_int(vm, blue);
    pkpy_push_int(vm, alpha);
    return 4;
}

int cre_pkpy_api_text_label_set_color(pkpy_vm* vm) {
    int pyEntityId;
    int pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyRed);
    pkpy_to_int(vm, 2, &pyGreen);
    pkpy_to_int(vm, 3, &pyBlue);
    pkpy_to_int(vm, 4, &pyAlpha);

    const CreEntity entity = (CreEntity)pyEntityId;
    TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TEXT_LABEL);
    textLabelComponent->color = ska_color_get_normalized_color(pyRed, pyGreen, pyBlue, pyAlpha);
    return 0;
}

int cre_pkpy_api_text_label_set_font_uid(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_CString pyFontUID;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyFontUID);

    const CreEntity entity = (CreEntity)pyEntityId;
    const char* fontUID = pyFontUID.data;
    if (se_asset_manager_has_font(fontUID)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_TEXT_LABEL);
        textLabelComponent->font = se_asset_manager_get_font(fontUID);
    } else {
        se_logger_warn("Failed to set font to '%s' as it doesn't exist in the asset manager!", fontUID);
    }
    return 0;
}

// --- COLLIDER2D ---//

int cre_pkpy_api_collider2d_get_extents(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Collider2DComponent* collider2DComponent = (Collider2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLLIDER_2D);
    pkpy_push_float(vm, (double)collider2DComponent->extents.w);
    pkpy_push_float(vm, (double)collider2DComponent->extents.h);
    return 2;
}

int cre_pkpy_api_collider2d_set_extents(pkpy_vm* vm) {
    int pyEntityId;
    double pyExtentsX, pyExtentsY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyExtentsX);
    pkpy_to_float(vm, 2, &pyExtentsY);

    const CreEntity entity = (CreEntity)pyEntityId;
    Collider2DComponent* collider2DComponent = (Collider2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLLIDER_2D);
    collider2DComponent->extents = (SKASize2D){ (float)pyExtentsX, (float)pyExtentsY };
    return 0;
}

int cre_pkpy_api_collider2d_get_color(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Collider2DComponent* collider2DComponent = (Collider2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLLIDER_2D);
    pkpy_push_float(vm, (double)collider2DComponent->color.r);
    pkpy_push_float(vm, (double)collider2DComponent->color.g);
    pkpy_push_float(vm, (double)collider2DComponent->color.b);
    pkpy_push_float(vm, (double)collider2DComponent->color.a);
    return 4;
}

int cre_pkpy_api_collider2d_set_color(pkpy_vm* vm) {
    int pyEntityId;
    double pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyRed);
    pkpy_to_float(vm, 2, &pyGreen);
    pkpy_to_float(vm, 3, &pyBlue);
    pkpy_to_float(vm, 4, &pyAlpha);

    const CreEntity entity = (CreEntity)pyEntityId;
    Collider2DComponent* collider2DComponent = (Collider2DComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLLIDER_2D);
    collider2DComponent->color = (SKAColor){ (float)pyRed, (float)pyGreen, (float)pyBlue, (float)pyAlpha };
    return 0;
}

//--- COLOR RECT ---//

int cre_pkpy_api_color_rect_get_size(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const ColorRectComponent* colorRectComponent = (ColorRectComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLOR_RECT);
    pkpy_push_float(vm, (double)colorRectComponent->size.w);
    pkpy_push_float(vm, (double)colorRectComponent->size.h);
    return 2;
}

int cre_pkpy_api_color_rect_set_size(pkpy_vm* vm) {
    int pyEntityId;
    double pySizeW, pySizeH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pySizeW);
    pkpy_to_float(vm, 2, &pySizeH);

    const CreEntity entity = (CreEntity)pyEntityId;
    ColorRectComponent* colorRectComponent = (ColorRectComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLOR_RECT);
    colorRectComponent->size = (SKASize2D){ (float)pySizeW, (float)pySizeH };
    return 0;
}

int cre_pkpy_api_color_rect_get_color(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const ColorRectComponent* colorRectComponent = (ColorRectComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLOR_RECT);
    pkpy_push_float(vm, (double)colorRectComponent->color.r);
    pkpy_push_float(vm, (double)colorRectComponent->color.g);
    pkpy_push_float(vm, (double)colorRectComponent->color.b);
    pkpy_push_float(vm, (double)colorRectComponent->color.a);
    return 4;
}

int cre_pkpy_api_color_rect_set_color(pkpy_vm* vm) {
    int pyEntityId;
    double pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyRed);
    pkpy_to_float(vm, 2, &pyGreen);
    pkpy_to_float(vm, 3, &pyBlue);
    pkpy_to_float(vm, 4, &pyAlpha);

    const CreEntity entity = (CreEntity)pyEntityId;
    ColorRectComponent* colorRectComponent = (ColorRectComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_COLOR_RECT);
    colorRectComponent->color = (SKAColor){ (float)pyRed, (float)pyGreen, (float)pyBlue, (float)pyAlpha };
    return 0;
}

//--- PARALLAX ---//

int cre_pkpy_api_parallax_get_scroll_speed(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const ParallaxComponent* parallaxComponent = (ParallaxComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_PARALLAX);
    pkpy_push_float(vm, (double)parallaxComponent->scrollSpeed.x);
    pkpy_push_float(vm, (double)parallaxComponent->scrollSpeed.y);
    return 2;
}

int cre_pkpy_api_parallax_set_scroll_speed(pkpy_vm* vm) {
    int pyEntityId;
    double pySpeedX, pySpeedY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pySpeedX);
    pkpy_to_float(vm, 2, &pySpeedY);

    const CreEntity entity = (CreEntity)pyEntityId;
    ParallaxComponent* parallaxComponent = (ParallaxComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_PARALLAX);
    parallaxComponent->scrollSpeed = (SKAVector2){ (float)pySpeedX, (float)pySpeedY };
    return 0;
}

//--- PARTICLES2D ---//

int cre_pkpy_api_particles2d_get_amount(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    pkpy_push_int(vm, particles2dComponent->amount);
    return 1;
}

int cre_pkpy_api_particles2d_set_amount(pkpy_vm* vm) {
    int pyEntityId;
    int pyAmount;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyAmount);

    const CreEntity entity = (CreEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2dComponent->amount = pyAmount;
    // TODO: Update particle stuff...
    return 0;
}

int cre_pkpy_api_particles2d_get_life_time(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    pkpy_push_float(vm, (double)particles2dComponent->lifeTime);
    return 1;
}

int cre_pkpy_api_particles2d_set_life_time(pkpy_vm* vm) {
    int pyEntityId;
    double pyLifeTime;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyLifeTime);

    const CreEntity entity = (CreEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2dComponent->lifeTime = (float)pyLifeTime;
    return 0;
}

int cre_pkpy_api_particles2d_get_damping(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    pkpy_push_float(vm, (double)particles2dComponent->damping);
    return 1;
}

int cre_pkpy_api_particles2d_set_damping(pkpy_vm* vm) {
    int pyEntityId;
    double pyDamping;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyDamping);

    const CreEntity entity = (CreEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2dComponent->damping = (float)pyDamping;
    return 0;
}

int cre_pkpy_api_particles2d_get_explosiveness(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    pkpy_push_float(vm, (double)particles2dComponent->explosiveness);
    return 1;
}

int cre_pkpy_api_particles2d_set_explosiveness(pkpy_vm* vm) {
    int pyEntityId;
    double pyExplosiveness;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyExplosiveness);

    const CreEntity entity = (CreEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2dComponent->explosiveness = ska_math_clamp_float((float)pyExplosiveness, 0.0f, 1.0f);
    return 0;
}

int cre_pkpy_api_particles2d_get_color(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    const int red = (int) (particles2dComponent->color.r * 255.0f);
    const int green = (int) (particles2dComponent->color.g * 255.0f);
    const int blue = (int) (particles2dComponent->color.b * 255.0f);
    const int alpha = (int) (particles2dComponent->color.a * 255.0f);
    pkpy_push_int(vm, red);
    pkpy_push_int(vm, green);
    pkpy_push_int(vm, blue);
    pkpy_push_int(vm, alpha);
    return 4;
}

int cre_pkpy_api_particles2d_set_color(pkpy_vm* vm) {
    int pyEntityId;
    int pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyRed);
    pkpy_to_int(vm, 2, &pyGreen);
    pkpy_to_int(vm, 3, &pyBlue);
    pkpy_to_int(vm, 4, &pyAlpha);

    const CreEntity entity = (CreEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2dComponent->color = ska_color_get_normalized_color(pyRed, pyGreen, pyBlue, pyAlpha);
    return 0;
}

int cre_pkpy_api_particles2d_get_initial_velocity(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    pkpy_push_float(vm, (double)particles2dComponent->initialVelocity.x);
    pkpy_push_float(vm, (double)particles2dComponent->initialVelocity.y);
    return 2;
}

int cre_pkpy_api_particles2d_set_initial_velocity(pkpy_vm* vm) {
    int pyEntityId;
    double pyX, pyY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyX);
    pkpy_to_float(vm, 2, &pyY);

    const CreEntity entity = (CreEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2dComponent->initialVelocity = (SKAVector2){ (float)pyX, (float)pyY };
    return 0;
}

int cre_pkpy_api_particles2d_get_spread(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    pkpy_push_float(vm, (double)particles2dComponent->spread);
    return 1;
}

int cre_pkpy_api_particles2d_set_spread(pkpy_vm* vm) {
    int pyEntityId;
    double pySpread;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pySpread);

    const CreEntity entity = (CreEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)cre_component_manager_get_component(entity, CreComponentDataIndex_PARTICLES_2D);
    particles2dComponent->spread = (float)pySpread;
    return 0;
}
