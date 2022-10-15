#include "scene_utils.h"

#include "scene_tree.h"
#include "../ecs/component/transform2d_component.h"
#include "../ecs/component/component.h"

// TODO: Clean up temp stuff for CombineModelResult
typedef struct EntityArray {
    int entityCount;
    Entity entities[10];
} EntityArray;

typedef EntityArray (*on_get_self_and_parent_entities) (Entity);
typedef Transform2D (*on_get_local_transform) (Entity, bool*);

EntityArray cre_scene_utils_get_self_and_parent_nodes(Entity entity);
Transform2D cre_scene_utils_get_local_transform(Entity entity, bool* success);

on_get_self_and_parent_entities onGetSelfAndParentEntitiesFunc = &cre_scene_utils_get_self_and_parent_nodes;
on_get_local_transform onGetLocalTransformFunc = &cre_scene_utils_get_local_transform;

// First index is the child
EntityArray cre_scene_utils_get_self_and_parent_nodes(Entity entity) {
    EntityArray combineModelResult = { .entityCount = 0 };
    combineModelResult.entities[combineModelResult.entityCount++] = entity;

//    SceneTreeNode* sceneTreeNode = rbe_scene_manager_get_entity_tree_node(entity);
    SceneTreeNode* sceneTreeNode = NULL;
    SceneTreeNode* parentTreeNode = sceneTreeNode->parent;
    while (parentTreeNode != NULL) {
        combineModelResult.entities[combineModelResult.entityCount++] = parentTreeNode->entity;
        parentTreeNode = parentTreeNode->parent;
    }
    return combineModelResult;
}

Transform2D cre_scene_utils_get_local_transform(Entity entity, bool* success) {
    Transform2DComponent* transform2DComponent = component_manager_get_component_unsafe(entity, ComponentDataIndex_TRANSFORM_2D);
    if (transform2DComponent != NULL) {
        *success = true;
        return transform2DComponent->localTransform;
    }
    *success = false;
    return (Transform2D){};
}

void cre_scene_utils_update_global_transform_model(Entity entity, TransformModel2D* globalTransform) {
    glm_mat4_identity(globalTransform->model);
    EntityArray combineModelResult = onGetSelfAndParentEntitiesFunc(entity);
    Vector2 scaleTotal = { 1.0f, 1.0f };
    for (int i = combineModelResult.entityCount - 1; i >= 0; i--) {
        Entity currentEntity = combineModelResult.entities[i];
        bool hasLocalTransform = false;
        const Transform2D localTransform = onGetLocalTransformFunc(currentEntity, &hasLocalTransform);
        if (!hasLocalTransform) {
            continue;
        }
        scaleTotal.x *= localTransform.scale.x;
        scaleTotal.y *= localTransform.scale.y;
        mat4 newModel;
//        transform2d_component_get_local_model_matrix(newModel, transform2DComponent);
        glm_mat4_mul(globalTransform->model, newModel, globalTransform->model);
    }
    globalTransform->scaleSign = rbe_math_signvec2(&scaleTotal);
}
