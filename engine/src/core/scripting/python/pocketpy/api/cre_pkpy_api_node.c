#include "cre_pkpy_api_node.h"

#include <pocketpy_c.h>

#include <seika/flag_utils.h>
#include <seika/logger.h>
#include <seika/string.h>
#include <seika/assert.h>
#include <seika/math/math.h>
#include <seika/rendering/renderer.h>
#include <seika/rendering/shader/shader_cache.h>
#include <seika/asset/asset_manager.h>

#include "../cre_pkpy_entity_instance_cache.h"
#include "../cre_pkpy_script_context.h"
#include "../../../../scene/scene_manager.h"
#include "../../../../ecs/ecs_globals.h"
#include "../../../../ecs/components/sprite_component.h"
#include "../../../../ecs/components/animated_sprite_component.h"
#include "../../../../ecs/components/text_label_component.h"
#include "../../../../ecs/components/collider2d_component.h"
#include "../../../../ecs/components/color_rect_component.h"
#include "../../../../ecs/components/parallax_component.h"
#include "../../../../ecs/components/script_component.h"
#include "../../../../ecs/components/particles2d_component.h"
#include "../../../../ecs/components/tilemap_component.h"

typedef struct CreColor {
    int32 r;
    int32 g;
    int32 b;
    int32 a;
} CreColor;

static CreColor convert_ska_color_to_cre(const SkaColor* color) {
    return (CreColor){
        .r = (int32)(color->r * 255.0f),
        .g = (int32)(color->g * 255.0f),
        .b = (int32)(color->b * 255.0f),
        .a = (int32)(color->a * 255.0f)
    };
}

static void set_node_component_from_type(SkaEntity entity, const char* classPath, const char* className, NodeBaseType baseType) {

    // Set components that should be set for a base node (that has invoked .new() from scripting)
    ska_ecs_component_manager_set_component(entity, NODE_COMPONENT_INDEX, node_component_create_ex(className, baseType));
    ska_ecs_component_manager_set_component(entity, SCRIPT_COMPONENT_INDEX, script_component_create_ex(classPath, className, ScriptContextType_PYTHON));

    const NodeBaseInheritanceType inheritanceType = node_get_type_inheritance(baseType);

    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_NODE2D)) {
        ska_ecs_component_manager_set_component(entity, TRANSFORM2D_COMPONENT_INDEX, transform2d_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_SPRITE)) {
        ska_ecs_component_manager_set_component(entity, SPRITE_COMPONENT_INDEX, sprite_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_ANIMATED_SPRITE)) {
        ska_ecs_component_manager_set_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX, animated_sprite_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_TEXT_LABEL)) {
        ska_ecs_component_manager_set_component(entity, TEXT_LABEL_COMPONENT_INDEX, text_label_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_COLLIDER2D)) {
        ska_ecs_component_manager_set_component(entity, COLLIDER2D_COMPONENT_INDEX, collider2d_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_COLOR_RECT)) {
        ska_ecs_component_manager_set_component(entity, COLOR_RECT_COMPONENT_INDEX, color_rect_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_PARALLAX)) {
        ska_ecs_component_manager_set_component(entity, PARALLAX_COMPONENT_INDEX, parallax_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_PARTICLES2D)) {
        ska_ecs_component_manager_set_component(entity, PARTICLES2D_COMPONENT_INDEX, particles2d_component_create());
    }
    if (SKA_FLAG_CONTAINS(inheritanceType, NodeBaseInheritanceType_TILEMAP)) {
        ska_ecs_component_manager_set_component(entity, TILEMAP_COMPONENT_INDEX, tilemap_component_create());
    }
}

//--- NODE ---//

int32 cre_pkpy_api_node_new(pkpy_vm* vm) {
    pkpy_CString pyClassPath;
    pkpy_CString pyClassName;
    int32 pyNodeTypeFlag;
    pkpy_to_string(vm, 0, &pyClassPath);
    pkpy_to_string(vm, 1, &pyClassName);
    pkpy_to_int(vm, 2, &pyNodeTypeFlag);

    char classPath[64];
    char className[32];
    ska_strcpy(classPath, pyClassPath);
    ska_strcpy(className, pyClassName);
    const SkaEntity entity = cre_pkpy_entity_instance_cache_create_new_entity(vm, classPath, className, ska_ecs_entity_create());
    SceneTreeNode* newNode = cre_scene_tree_create_tree_node(entity, NULL);
    cre_scene_manager_stage_child_node_to_be_added_later(newNode);

    set_node_component_from_type(entity, classPath, className, (NodeBaseType)pyNodeTypeFlag);

    cre_pkpy_script_context_create_instance(entity, classPath, className);
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, entity);
    return 1;
}

int32 cre_pkpy_api_node_get_name(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    pkpy_push_string(vm, pkpy_string(nodeComponent->name));
    return 1;
}

int32 cre_pkpy_api_node_add_child(pkpy_vm* vm) {
    int32 childEntityId;
    int32 parentEntityId;
    pkpy_to_int(vm, 0, &parentEntityId);
    pkpy_to_int(vm, 1, &childEntityId);

    SkaEntity childEntity = (SkaEntity)childEntityId;
    SkaEntity parentEntity = (SkaEntity)parentEntityId;
    cre_scene_manager_add_node_as_child(childEntity, parentEntity);
    return 0;
}

int32 cre_pkpy_api_node_get_child(pkpy_vm* vm) {
    int32 parentEntityId;
    pkpy_CString childEntityName;
    pkpy_to_int(vm, 0, &parentEntityId);
    pkpy_to_string(vm, 1, &childEntityName);

    const char* childName = childEntityName;
    const SkaEntity childEntity = cre_scene_manager_get_entity_child_by_name((SkaEntity)parentEntityId, childName);
    if (childEntity != SKA_NULL_ENTITY) {
        cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(childEntity);
        return 1;
    }
    return 0;
}

int32 cre_pkpy_api_node_get_children(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    if (!cre_scene_manager_has_entity_tree_node(entity)) {
        return 0;
    }

    const SceneTreeNode* parentTreeNode = cre_scene_manager_get_entity_tree_node(entity);
    for (size_t i = 0; i < parentTreeNode->childCount; i++) {
        const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
        cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(childTreeNode->entity);
    }

    return (int32)parentTreeNode->childCount;
}

int32 cre_pkpy_api_node_get_parent(pkpy_vm* vm) {
    int32 childEntityId;
    pkpy_to_int(vm, 0, &childEntityId);

    const SkaEntity childEntity = (SkaEntity)childEntityId;
    if (cre_scene_manager_has_entity_tree_node(childEntity)) {
        const SceneTreeNode* treeNode = cre_scene_manager_get_entity_tree_node(childEntity);
        if (treeNode->parent) {
            cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(treeNode->parent->entity);
        } else {
            pkpy_push_none(vm);
        }
        return 1;
    }
    return 0;
}

int32 cre_pkpy_api_node_queue_deletion(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    if (!nodeComponent->queuedForDeletion) {
        if (cre_scene_manager_has_entity_tree_node(entity)) {
            SceneTreeNode* node = cre_scene_manager_get_entity_tree_node(entity);
            cre_queue_destroy_tree_node_entity_all(node);
        } else {
            ska_logger_warn("Entity not found in tree, skipping queue deletion.");
        }
    } else {
        ska_logger_warn("Node '%s' already queued for deletion!", nodeComponent->name);
    }
    return 0;
}

int32 cre_pkpy_api_node_is_queued_for_deletion(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    pkpy_push_bool(vm, nodeComponent->queuedForDeletion);
    return 1;
}

int32 cre_pkpy_api_node_set_time_dilation(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 timeDilation;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &timeDilation);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    nodeComponent->timeDilation.value = (f32)timeDilation;
    cre_scene_manager_invalidate_time_dilation_nodes_with_children(entity);

    return 0;
}

int32 cre_pkpy_api_node_get_time_dilation(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    NodeComponent* nodeComponent = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    const f64 timeDilation = (f64)nodeComponent->timeDilation.value;
    pkpy_push_float(vm, timeDilation);
    return 1;
}

int32 cre_pkpy_api_node_get_total_time_dilation(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const f64 totalTimeDilation = (f64)cre_scene_manager_get_node_full_time_dilation(entity);
    pkpy_push_float(vm, totalTimeDilation);
    return 1;
}

//--- NODE2D ---//

void cre_pkpy_update_entity_local_position(SkaEntity entity, SkaVector2* position) {
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity,TRANSFORM2D_COMPONENT_INDEX);
    const SkaVector2 prevPosition = transformComp->localTransform.position;
    transformComp->localTransform.position.x = position->x;
    transformComp->localTransform.position.y = position->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.position.x != prevPosition.x || transformComp->localTransform.position.y != prevPosition.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void cre_pkpy_update_entity_local_scale(SkaEntity entity, SkaVector2 * scale) {
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity,TRANSFORM2D_COMPONENT_INDEX);
    const SkaVector2 prevScale = transformComp->localTransform.scale;
    transformComp->localTransform.scale.x = scale->x;
    transformComp->localTransform.scale.y = scale->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.scale.x != prevScale.x || transformComp->localTransform.scale.y != prevScale.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void cre_pkpy_update_entity_local_rotation(SkaEntity entity, f32 rotation) {
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    const f32 prevRotation = transformComp->localTransform.rotation;
    transformComp->localTransform.rotation = rotation;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.rotation != prevRotation) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

int32 cre_pkpy_api_node2d_set_position(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 positionX;
    f64 positionY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &positionX);
    pkpy_to_float(vm, 2, &positionY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    cre_pkpy_update_entity_local_position(entity, &(SkaVector2){ (f32)positionX, (f32)positionY });
    return 0;
}

int32 cre_pkpy_api_node2d_add_to_position(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 positionX;
    f64 positionY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &positionX);
    pkpy_to_float(vm, 2, &positionY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    cre_pkpy_update_entity_local_position(entity, &(SkaVector2){
            transformComp->localTransform.position.x + (f32)positionX,
            transformComp->localTransform.position.y + (f32)positionY
    });
    return 0;
}

int32 cre_pkpy_api_node2d_get_position(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)transformComp->localTransform.position.x);
    pkpy_push_float(vm, (f64)transformComp->localTransform.position.y);
    return 2;
}

int32 cre_pkpy_api_node2d_get_global_position(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    SkaTransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transformComp);
    pkpy_push_float(vm, (f64)globalTransform->position.x);
    pkpy_push_float(vm, (f64)globalTransform->position.y);
    return 2;
}

int32 cre_pkpy_api_node2d_set_scale(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 scaleX;
    f64 scaleY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &scaleX);
    pkpy_to_float(vm, 2, &scaleY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    cre_pkpy_update_entity_local_scale(entity, &(SkaVector2){(f32)scaleX, (f32)scaleY });
    return 0;
}

int32 cre_pkpy_api_node2d_add_to_scale(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 scaleX;
    f64 scaleY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &scaleX);
    pkpy_to_float(vm, 2, &scaleY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    cre_pkpy_update_entity_local_scale(entity, &(SkaVector2){
            transformComp->localTransform.scale.x + (f32)scaleX,
            transformComp->localTransform.scale.y + (f32)scaleY
    });
    return 0;
}

int32 cre_pkpy_api_node2d_get_scale(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)transformComp->localTransform.scale.x);
    pkpy_push_float(vm, (f64)transformComp->localTransform.scale.y);
    return 2;
}

int32 cre_pkpy_api_node2d_set_rotation(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 rotation;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &rotation);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    cre_pkpy_update_entity_local_rotation(entity, (f32)rotation);
    return 0;
}

int32 cre_pkpy_api_node2d_add_to_rotation(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 rotation;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &rotation);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    cre_pkpy_update_entity_local_rotation(entity, transformComp->localTransform.rotation + (f32)rotation);
    return 0;
}

int32 cre_pkpy_api_node2d_get_rotation(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)transformComp->localTransform.rotation);
    return 1;
}

int32 cre_pkpy_api_node2d_set_z_index(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 zIndex;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &zIndex);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    transformComp->zIndex = zIndex;
    return 0;
}

int32 cre_pkpy_api_node2d_get_z_index(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    pkpy_push_int(vm, transformComp->zIndex);
    return 1;
}

int32 cre_pkpy_api_node2d_set_z_index_relative_to_parent(pkpy_vm* vm) {
    int32 pyEntityId;
    bool zIndexRelativeToParent;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &zIndexRelativeToParent);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    transformComp->isZIndexRelativeToParent = zIndexRelativeToParent;
    return 0;
}

int32 cre_pkpy_api_node2d_get_z_index_relative_to_parent(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    pkpy_push_bool(vm, transformComp->isZIndexRelativeToParent);
    return 1;
}

int32 cre_pkpy_api_node2d_set_ignore_camera(pkpy_vm* vm) {
    int32 pyEntityId;
    bool ignoreCamera;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &ignoreCamera);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    transformComp->ignoreCamera = ignoreCamera;
    return 0;
}

int32 cre_pkpy_api_node2d_get_ignore_camera(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    pkpy_push_bool(vm, transformComp->ignoreCamera);
    return 1;
}

//--- SPRITE ---//

int32 cre_pkpy_api_sprite_get_texture(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    SkaTexture* texture = spriteComponent->texture; // TODO: Make const once applied to se_texture_get_wrap function params
    pkpy_push_string(vm, pkpy_string(texture->fileName));
    pkpy_push_string(vm, pkpy_string(ska_texture_get_wrap_s_string(texture)));
    pkpy_push_string(vm, pkpy_string(ska_texture_get_wrap_t_string(texture)));
    pkpy_push_bool(vm, texture->applyNearestNeighbor);
    return 4;
}

int32 cre_pkpy_api_sprite_set_texture(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_CString pyTexturePath;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyTexturePath);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const char* texturePath = pyTexturePath;
    SKA_ASSERT_FMT(ska_asset_manager_has_texture(texturePath), "Doesn't have texture with file path '%s'", texturePath);
    SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    spriteComponent->texture = ska_asset_manager_get_texture(texturePath);
    return 0;
}

int32 cre_pkpy_api_sprite_get_draw_source(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)spriteComponent->drawSource.x);
    pkpy_push_float(vm, (f64)spriteComponent->drawSource.y);
    pkpy_push_float(vm, (f64)spriteComponent->drawSource.w);
    pkpy_push_float(vm, (f64)spriteComponent->drawSource.h);
    return 4;
}

int32 cre_pkpy_api_sprite_set_draw_source(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyDrawSourceX;
    f64 pyDrawSourceY;
    f64 pyDrawSourceW;
    f64 pyDrawSourceH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyDrawSourceX);
    pkpy_to_float(vm, 2, &pyDrawSourceY);
    pkpy_to_float(vm, 3, &pyDrawSourceW);
    pkpy_to_float(vm, 4, &pyDrawSourceH);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    spriteComponent->drawSource = (SkaRect2){ (f32)pyDrawSourceX, (f32)pyDrawSourceY, (f32)pyDrawSourceW, (f32)pyDrawSourceH };
    return 0;
}

int32 cre_pkpy_api_sprite_get_flip_h(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    pkpy_push_bool(vm, spriteComponent->flipH);
    return 1;
}

int32 cre_pkpy_api_sprite_set_flip_h(pkpy_vm* vm) {
    int32 pyEntityId;
    bool pySetFlipH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipH);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    spriteComponent->flipH = pySetFlipH;
    return 0;
}

int32 cre_pkpy_api_sprite_get_flip_v(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    pkpy_push_bool(vm, spriteComponent->flipV);
    return 1;
}

int32 cre_pkpy_api_sprite_set_flip_v(pkpy_vm* vm) {
    int32 pyEntityId;
    bool pySetFlipV;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipV);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    spriteComponent->flipV = pySetFlipV;
    return 0;
}

int32 cre_pkpy_api_sprite_get_modulate(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    const CreColor color = convert_ska_color_to_cre(&spriteComponent->modulate);
    pkpy_push_int(vm, color.r);
    pkpy_push_int(vm, color.g);
    pkpy_push_int(vm, color.b);
    pkpy_push_int(vm, color.a);
    return 4;
}

int32 cre_pkpy_api_sprite_set_modulate(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyModulateR, pyModulateG, pyModulateB, pyModulateA;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyModulateR);
    pkpy_to_int(vm, 2, &pyModulateG);
    pkpy_to_int(vm, 3, &pyModulateB);
    pkpy_to_int(vm, 4, &pyModulateA);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    spriteComponent->modulate = ska_color_get_normalized_color(pyModulateR, pyModulateB, pyModulateG, pyModulateA);
    return 0;
}

int32 cre_pkpy_api_sprite_get_origin(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)spriteComponent->origin.x);
    pkpy_push_float(vm, (f64)spriteComponent->origin.y);
    return 2;
}

int32 cre_pkpy_api_sprite_set_origin(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyOriginX;
    f64 pyOriginY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyOriginX);
    pkpy_to_float(vm, 2, &pyOriginY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    spriteComponent->origin = (SkaVector2){ (f32)pyOriginX, (f32)pyOriginY };
    return 0;
}

int32 cre_pkpy_api_sprite_get_shader_instance(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    const int32 pyShaderInstanceId = spriteComponent->shaderInstanceId != SKA_SHADER_INSTANCE_INVALID_ID ? (int32)spriteComponent->shaderInstanceId : -1;
    pkpy_push_int(vm, pyShaderInstanceId);
    return 1;
}

int32 cre_pkpy_api_sprite_set_shader_instance(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyShaderInstanceId;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyShaderInstanceId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entity, SPRITE_COMPONENT_INDEX);
    spriteComponent->shaderInstanceId = (SkaShaderInstanceId)pyShaderInstanceId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(spriteComponent->shaderInstanceId);
    ska_renderer_set_sprite_shader_default_params(shaderInstance->shader);
    return 0;
}

//--- ANIMATED SPRITE ---//

int32 cre_pkpy_api_animated_sprite_play(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_CString pyAnimationName;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyAnimationName);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const char* animationName = pyAnimationName;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    const bool hasSuccessfullyPlayedAnimation = animated_sprite_component_play_animation(animatedSpriteComponent, animationName);
    pkpy_push_bool(vm, hasSuccessfullyPlayedAnimation);
    return 1;
}

int32 cre_pkpy_api_animated_sprite_stop(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->isPlaying = false;
    return 0;
}

int32 cre_pkpy_api_animated_sprite_set_current_animation_frame(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyFrame;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyFrame);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->currentAnimation.currentFrame = ska_math_clamp_int(pyFrame, 0, animatedSpriteComponent->currentAnimation.frameCount - 1);
    return 0;
}

int32 cre_pkpy_api_animated_sprite_add_animation(pkpy_vm* vm) {
#define CRE_PKPY_API_ANIM_FRAME_STRIDE 6
    int32 pyEntityId;
    pkpy_CString pyAnimationName;
    int32 pyAnimationSpeed;
    bool pyAnimationDoesLoop;
    int32 pyFrameCount;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyAnimationName);
    pkpy_to_int(vm, 2, &pyAnimationSpeed);
    pkpy_to_bool(vm, 3, &pyAnimationDoesLoop);
    pkpy_to_int(vm, 4, &pyFrameCount);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const char* animationName = pyAnimationName;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    CreAnimation newAnim = { .frameCount = 0, .currentFrame = 0, .speed = pyAnimationSpeed, .name = {'\0'}, .doesLoop = pyAnimationDoesLoop, .isValid = true };
    ska_strcpy(newAnim.name, animationName);
    // Unpack *args filled with frame data
    pkpy_unpack_sequence(vm, pyFrameCount * CRE_PKPY_API_ANIM_FRAME_STRIDE);
    for (int32 i = 0; i < pyFrameCount; i++) {
        int32 pyFrame;
        pkpy_CString pyTexturePath;
        f64 pyDrawSourceX;
        f64 pyDrawSourceY;
        f64 pyDrawSourceW;
        f64 pyDrawSourceH;
        const int32 dataIndex = i * CRE_PKPY_API_ANIM_FRAME_STRIDE;
        pkpy_to_int(vm, dataIndex, &pyFrame);
        pkpy_to_string(vm, dataIndex + 1, &pyTexturePath);
        pkpy_to_float(vm, dataIndex + 2, &pyDrawSourceX);
        pkpy_to_float(vm, dataIndex + 3, &pyDrawSourceY);
        pkpy_to_float(vm, dataIndex + 4, &pyDrawSourceW);
        pkpy_to_float(vm, dataIndex + 5, &pyDrawSourceH);

        const char* texturePath = pyTexturePath;
        CreAnimationFrame animationFrame = {
                .frame = pyFrame,
                .texture = ska_asset_manager_get_texture(texturePath),
                .drawSource = { (f32)pyDrawSourceX, (f32)pyDrawSourceY, (f32)pyDrawSourceW, (f32)pyDrawSourceH }
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

int32 cre_pkpy_api_animated_sprite_get_stagger_animation_start_times(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    pkpy_push_bool(vm, animatedSpriteComponent->staggerStartAnimationTimes);
    return 1;
}

int32 cre_pkpy_api_animated_sprite_set_stagger_animation_start_times(pkpy_vm* vm) {
    int32 pyEntityId;
    bool pyStaggerStartAnimationTimes;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pyStaggerStartAnimationTimes);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->staggerStartAnimationTimes = pyStaggerStartAnimationTimes;
    return 0;
}

int32 cre_pkpy_api_animated_sprite_get_flip_h(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    pkpy_push_bool(vm, animatedSpriteComponent->flipH);
    return 1;
}

int32 cre_pkpy_api_animated_sprite_set_flip_h(pkpy_vm* vm) {
    int32 pyEntityId;
    bool pySetFlipH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipH);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->flipH = pySetFlipH;
    return 0;
}

int32 cre_pkpy_api_animated_sprite_get_flip_v(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    pkpy_push_bool(vm, animatedSpriteComponent->flipV);
    return 1;
}

int32 cre_pkpy_api_animated_sprite_set_flip_v(pkpy_vm* vm) {
    int32 pyEntityId;
    bool pySetFlipV;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_bool(vm, 1, &pySetFlipV);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->flipV = pySetFlipV;
    return 0;
}

int32 cre_pkpy_api_animated_sprite_get_modulate(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    const CreColor color = convert_ska_color_to_cre(&animatedSpriteComponent->modulate);
    pkpy_push_int(vm, color.r);
    pkpy_push_int(vm, color.g);
    pkpy_push_int(vm, color.b);
    pkpy_push_int(vm, color.a);
    return 4;
}

int32 cre_pkpy_api_animated_sprite_set_modulate(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyModulateR, pyModulateG, pyModulateB, pyModulateA;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyModulateR);
    pkpy_to_int(vm, 2, &pyModulateG);
    pkpy_to_int(vm, 3, &pyModulateB);
    pkpy_to_int(vm, 4, &pyModulateA);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->modulate = ska_color_get_normalized_color(pyModulateR, pyModulateB, pyModulateG, pyModulateA);
    return 0;
}

int32 cre_pkpy_api_animated_sprite_get_origin(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)animatedSpriteComponent->origin.x);
    pkpy_push_float(vm, (f64)animatedSpriteComponent->origin.y);
    return 2;
}

int32 cre_pkpy_api_animated_sprite_set_origin(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyOriginX;
    f64 pyOriginY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyOriginX);
    pkpy_to_float(vm, 2, &pyOriginY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->origin = (SkaVector2){ (f32)pyOriginX, (f32)pyOriginY };
    return 0;
}

int32 cre_pkpy_api_animated_sprite_get_shader_instance(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    const int32 pyShaderInstanceId = animatedSpriteComponent->shaderInstanceId != SKA_SHADER_INSTANCE_INVALID_ID ? (int32)animatedSpriteComponent->shaderInstanceId : -1;
    pkpy_push_int(vm, pyShaderInstanceId);
    return 1;
}

int32 cre_pkpy_api_animated_sprite_set_shader_instance(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyShaderInstanceId;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyShaderInstanceId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entity, ANIMATED_SPRITE_COMPONENT_INDEX);
    animatedSpriteComponent->shaderInstanceId = (SkaShaderInstanceId)pyShaderInstanceId;
    SkaShaderInstance* shaderInstance = ska_shader_cache_get_instance(animatedSpriteComponent->shaderInstanceId);
    ska_renderer_set_sprite_shader_default_params(shaderInstance->shader);
    return 0;
}

//--- TEXT LABEL ---//

int32 cre_pkpy_api_text_label_get_text(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(entity, TEXT_LABEL_COMPONENT_INDEX);
    pkpy_push_string(vm, pkpy_string(textLabelComponent->text));
    return 1;
}

int32 cre_pkpy_api_text_label_set_text(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_CString pyText;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyText);

    static char textBuffer[2048];
    ska_strcpy(textBuffer, pyText);
    const SkaEntity entity = (SkaEntity)pyEntityId;
    TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(entity, TEXT_LABEL_COMPONENT_INDEX);
    ska_strcpy(textLabelComponent->text, textBuffer);
    return 0;
}

int32 cre_pkpy_api_text_label_get_color(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(entity, TEXT_LABEL_COMPONENT_INDEX);
    const CreColor color = convert_ska_color_to_cre(&textLabelComponent->color);
    pkpy_push_int(vm, color.r);
    pkpy_push_int(vm, color.g);
    pkpy_push_int(vm, color.b);
    pkpy_push_int(vm, color.a);
    return 4;
}

int32 cre_pkpy_api_text_label_set_color(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyRed);
    pkpy_to_int(vm, 2, &pyGreen);
    pkpy_to_int(vm, 3, &pyBlue);
    pkpy_to_int(vm, 4, &pyAlpha);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(entity, TEXT_LABEL_COMPONENT_INDEX);
    textLabelComponent->color = ska_color_get_normalized_color(pyRed, pyGreen, pyBlue, pyAlpha);
    return 0;
}

int32 cre_pkpy_api_text_label_set_font_uid(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_CString pyFontUID;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_string(vm, 1, &pyFontUID);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const char* fontUID = pyFontUID;
    if (ska_asset_manager_has_font(fontUID)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*)ska_ecs_component_manager_get_component(entity, TEXT_LABEL_COMPONENT_INDEX);
        textLabelComponent->font = ska_asset_manager_get_font(fontUID);
    } else {
        ska_logger_warn("Failed to set font to '%s' as it doesn't exist in the asset manager!", fontUID);
    }
    return 0;
}

// --- COLLIDER2D ---//

int32 cre_pkpy_api_collider2d_get_extents(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Collider2DComponent* collider2DComponent = (Collider2DComponent*)ska_ecs_component_manager_get_component(entity, COLLIDER2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)collider2DComponent->extents.w);
    pkpy_push_float(vm, (f64)collider2DComponent->extents.h);
    return 2;
}

int32 cre_pkpy_api_collider2d_set_extents(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyExtentsX, pyExtentsY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyExtentsX);
    pkpy_to_float(vm, 2, &pyExtentsY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Collider2DComponent* collider2DComponent = (Collider2DComponent*)ska_ecs_component_manager_get_component(entity, COLLIDER2D_COMPONENT_INDEX);
    collider2DComponent->extents = (SkaSize2D){ (f32)pyExtentsX, (f32)pyExtentsY };
    return 0;
}

int32 cre_pkpy_api_collider2d_get_color(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Collider2DComponent* collider2DComponent = (Collider2DComponent*)ska_ecs_component_manager_get_component(entity, COLLIDER2D_COMPONENT_INDEX);
    const CreColor color = convert_ska_color_to_cre(&collider2DComponent->color);
    pkpy_push_int(vm, color.r);
    pkpy_push_int(vm, color.g);
    pkpy_push_int(vm, color.b);
    pkpy_push_int(vm, color.a);
    return 4;
}

int32 cre_pkpy_api_collider2d_set_color(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyRed);
    pkpy_to_int(vm, 2, &pyGreen);
    pkpy_to_int(vm, 3, &pyBlue);
    pkpy_to_int(vm, 4, &pyAlpha);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Collider2DComponent* collider2DComponent = (Collider2DComponent*)ska_ecs_component_manager_get_component(entity, COLLIDER2D_COMPONENT_INDEX);
    collider2DComponent->color = ska_color_get_normalized_color(pyRed, pyGreen, pyBlue, pyAlpha);
    return 0;
}

//--- COLOR RECT ---//

int32 cre_pkpy_api_color_rect_get_size(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const ColorRectComponent* colorRectComponent = (ColorRectComponent*)ska_ecs_component_manager_get_component(entity, COLOR_RECT_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)colorRectComponent->size.w);
    pkpy_push_float(vm, (f64)colorRectComponent->size.h);
    return 2;
}

int32 cre_pkpy_api_color_rect_set_size(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pySizeW, pySizeH;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pySizeW);
    pkpy_to_float(vm, 2, &pySizeH);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    ColorRectComponent* colorRectComponent = (ColorRectComponent*)ska_ecs_component_manager_get_component(entity, COLOR_RECT_COMPONENT_INDEX);
    colorRectComponent->size = (SkaSize2D){ (f32)pySizeW, (f32)pySizeH };
    return 0;
}

int32 cre_pkpy_api_color_rect_get_color(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const ColorRectComponent* colorRectComponent = (ColorRectComponent*)ska_ecs_component_manager_get_component(entity, COLOR_RECT_COMPONENT_INDEX);
    const CreColor color = convert_ska_color_to_cre(&colorRectComponent->color);
    pkpy_push_int(vm, color.r);
    pkpy_push_int(vm, color.g);
    pkpy_push_int(vm, color.b);
    pkpy_push_int(vm, color.a);
    return 4;
}

int32 cre_pkpy_api_color_rect_set_color(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyRed);
    pkpy_to_int(vm, 2, &pyGreen);
    pkpy_to_int(vm, 3, &pyBlue);
    pkpy_to_int(vm, 4, &pyAlpha);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    ColorRectComponent* colorRectComponent = (ColorRectComponent*)ska_ecs_component_manager_get_component(entity, COLOR_RECT_COMPONENT_INDEX);
    colorRectComponent->color = ska_color_get_normalized_color(pyRed, pyGreen, pyBlue, pyAlpha);
    return 0;
}

//--- PARALLAX ---//

int32 cre_pkpy_api_parallax_get_scroll_speed(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const ParallaxComponent* parallaxComponent = (ParallaxComponent*)ska_ecs_component_manager_get_component(entity, PARALLAX_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)parallaxComponent->scrollSpeed.x);
    pkpy_push_float(vm, (f64)parallaxComponent->scrollSpeed.y);
    return 2;
}

int32 cre_pkpy_api_parallax_set_scroll_speed(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pySpeedX, pySpeedY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pySpeedX);
    pkpy_to_float(vm, 2, &pySpeedY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    ParallaxComponent* parallaxComponent = (ParallaxComponent*)ska_ecs_component_manager_get_component(entity, PARALLAX_COMPONENT_INDEX);
    parallaxComponent->scrollSpeed = (SkaVector2){ (f32)pySpeedX, (f32)pySpeedY };
    return 0;
}

//--- PARTICLES2D ---//

int32 cre_pkpy_api_particles2d_get_amount(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    pkpy_push_int(vm, particles2dComponent->amount);
    return 1;
}

int32 cre_pkpy_api_particles2d_set_amount(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyAmount;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyAmount);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2dComponent->amount = pyAmount;
    // TODO: Update particle stuff...
    return 0;
}

int32 cre_pkpy_api_particles2d_get_life_time(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)particles2dComponent->lifeTime);
    return 1;
}

int32 cre_pkpy_api_particles2d_set_life_time(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyLifeTime;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyLifeTime);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2dComponent->lifeTime = (f32)pyLifeTime;
    return 0;
}

int32 cre_pkpy_api_particles2d_get_damping(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)particles2dComponent->damping);
    return 1;
}

int32 cre_pkpy_api_particles2d_set_damping(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyDamping;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyDamping);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2dComponent->damping = (f32)pyDamping;
    return 0;
}

int32 cre_pkpy_api_particles2d_get_explosiveness(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)particles2dComponent->explosiveness);
    return 1;
}

int32 cre_pkpy_api_particles2d_set_explosiveness(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyExplosiveness;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyExplosiveness);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2dComponent->explosiveness = ska_math_clamp_float((f32)pyExplosiveness, 0.0f, 1.0f);
    return 0;
}

int32 cre_pkpy_api_particles2d_get_color(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    const CreColor color = convert_ska_color_to_cre(&particles2dComponent->color);
    pkpy_push_int(vm, color.r);
    pkpy_push_int(vm, color.g);
    pkpy_push_int(vm, color.b);
    pkpy_push_int(vm, color.a);
    return 4;
}

int32 cre_pkpy_api_particles2d_set_color(pkpy_vm* vm) {
    int32 pyEntityId;
    int32 pyRed, pyGreen, pyBlue, pyAlpha;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_int(vm, 1, &pyRed);
    pkpy_to_int(vm, 2, &pyGreen);
    pkpy_to_int(vm, 3, &pyBlue);
    pkpy_to_int(vm, 4, &pyAlpha);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2dComponent->color = ska_color_get_normalized_color(pyRed, pyGreen, pyBlue, pyAlpha);
    return 0;
}

int32 cre_pkpy_api_particles2d_get_initial_velocity(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)particles2dComponent->initialVelocity.min.x);
    pkpy_push_float(vm, (f64)particles2dComponent->initialVelocity.min.y);
    pkpy_push_float(vm, (f64)particles2dComponent->initialVelocity.max.x);
    pkpy_push_float(vm, (f64)particles2dComponent->initialVelocity.max.y);
    return 4;
}

int32 cre_pkpy_api_particles2d_set_initial_velocity(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pyMinX, pyMinY;
    f64 pyMaxX, pyMaxY;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pyMinX);
    pkpy_to_float(vm, 2, &pyMinY);
    pkpy_to_float(vm, 3, &pyMaxX);
    pkpy_to_float(vm, 4, &pyMaxY);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2dComponent->initialVelocity = (SkaMinMaxVec2){
        .min = { .x = (f32)pyMinX, .y = (f32)pyMinY },
        .max = { .x = (f32)pyMaxX, .y = (f32)pyMaxY }
    };
    return 0;
}

int32 cre_pkpy_api_particles2d_get_spread(pkpy_vm* vm) {
    int32 pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    const Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    pkpy_push_float(vm, (f64)particles2dComponent->spread);
    return 1;
}

int32 cre_pkpy_api_particles2d_set_spread(pkpy_vm* vm) {
    int32 pyEntityId;
    f64 pySpread;
    pkpy_to_int(vm, 0, &pyEntityId);
    pkpy_to_float(vm, 1, &pySpread);

    const SkaEntity entity = (SkaEntity)pyEntityId;
    Particles2DComponent* particles2dComponent = (Particles2DComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    particles2dComponent->spread = (f32)pySpread;
    return 0;
}
