#include "scene_utils.h"

#include "scene_tree.h"
#include "../ecs/component/transform2d_component.h"

// TODO: Clean up temp stuff for CombineModelResult
typedef struct EntityArray {
    int entityCount;
    Entity entities[10];
} EntityArray;

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

void cre_scene_utils_update_global_transform_model(Entity entity, TransformModel2D* globalTransform) {
    glm_mat4_identity(globalTransform->model);
    EntityArray combineModelResult = cre_scene_utils_get_self_and_parent_nodes(entity);
    Vector2 scaleTotal = { 1.0f, 1.0f };
    for (int i = combineModelResult.entityCount - 1; i >= 0; i--) {
        Entity currentEntity = combineModelResult.entities[i];
//        Transform2DComponent* transform2DComponent = component_manager_get_component_unsafe(currentEntity, ComponentDataIndex_TRANSFORM_2D);
        Transform2DComponent* transform2DComponent = NULL;
        if (transform2DComponent == NULL) {
            continue;
        }
        scaleTotal.x *= transform2DComponent->localTransform.scale.x;
        scaleTotal.y *= transform2DComponent->localTransform.scale.y;
        mat4 newModel;
        transform2d_component_get_local_model_matrix(newModel, transform2DComponent);
        glm_mat4_mul(globalTransform->model, newModel, globalTransform->model);
    }
    globalTransform->scaleSign = rbe_math_signvec2(&scaleTotal);
}
