#include "scene_manager.h"

#include <string.h>

#include <seika/rendering/shader/shader_cache.h>
#include <seika/rendering/renderer.h>
#include <seika/ecs/ecs.h>
#include <seika/asset/asset_manager.h>
#include <seika/utils/logger.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_assert.h>
#include <seika/data_structures/se_hash_map.h>
#include <seika/data_structures/se_static_array.h>

#include "scene_utils.h"
#include "scene_template_cache.h"
#include "../world.h"
#include "../game_properties.h"
#include "../ecs/ecs_globals.h"
//#include "../ecs/system/ec_system.h"
#include "../ecs/component/sprite_component.h"
#include "../ecs/component/animated_sprite_component.h"
#include "../ecs/component/text_label_component.h"
#include "../ecs/component/script_component.h"
#include "../ecs/component/collider2d_component.h"
#include "../ecs/component/color_rect_component.h"
#include "../ecs/component/parallax_component.h"
#include "../ecs/component/particles2d_component.h"
#include "../camera/camera_manager.h"
#include "../camera/camera.h"

// --- Scene Tree --- //
// Executes function on passed in tree node and all child tree nodes
void cre_scene_execute_on_all_tree_nodes(SceneTreeNode* treeNode, ExecuteOnAllTreeNodesFunc func) {
    for (size_t i = 0; i < treeNode->childCount; i++) {
        cre_scene_execute_on_all_tree_nodes(treeNode->children[i], func);
    }
    func(treeNode);
}

typedef struct SceneTree {
    SceneTreeNode* root;
} SceneTree;

SceneTreeNode* cre_scene_tree_create_tree_node(CreEntity entity, SceneTreeNode* parent) {
    SceneTreeNode* treeNode = SE_MEM_ALLOCATE(SceneTreeNode);
    treeNode->entity = entity;
    treeNode->parent = parent;
    treeNode->childCount = 0;
    return treeNode;
}

// --- Scene --- //
typedef struct Scene {
    const char* scenePath;
    SceneTree* sceneTree;
} Scene;

Scene* cre_scene_create_scene(const char* scenePath) {
    Scene* scene = SE_MEM_ALLOCATE(Scene);
    scene->scenePath = scenePath;
    scene->sceneTree = SE_MEM_ALLOCATE(SceneTree);
    scene->sceneTree->root = NULL;
    return scene;
}

// --- Scene Manager --- //
CreEntity entitiesQueuedForCreation[CRE_MAX_ENTITIES];
size_t entitiesQueuedForCreationSize = 0;
CreEntity entitiesQueuedForDeletion[CRE_MAX_ENTITIES];
size_t entitiesQueuedForDeletionSize = 0;

SE_STATIC_ARRAY_CREATE(CreEntity, CRE_MAX_ENTITIES, entitiesToUnlinkParent);

#ifdef CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA
// Will need a different mechanism for 3D (maybe just storing a vector3, but this is fine for now
SE_STATIC_ARRAY_CREATE(SKATransform2D , CRE_MAX_ENTITIES, entityPrevGlobalTransforms);
#endif // CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA

Scene* activeScene = NULL;
Scene* queuedSceneToChangeTo = NULL;

static SEHashMap* entityToTreeNodeMap = NULL;
static SEHashMap* entityToStagedTreeNodeMap = NULL;

SceneTreeNode* cre_scene_manager_pop_staged_entity_tree_node(CreEntity entity);
void cre_scene_manager_add_staged_node_children_to_scene(SceneTreeNode* treeNode);
void cre_scene_manager_setup_scene_nodes_from_json(JsonSceneNode* jsonSceneNode);

void cre_scene_manager_initialize() {
    SE_ASSERT(entityToTreeNodeMap == NULL);
    SE_ASSERT(entityToStagedTreeNodeMap == NULL);
    entityToTreeNodeMap = se_hash_map_create(sizeof(CreEntity), sizeof(SceneTreeNode**), SE_HASH_MAP_MIN_CAPACITY);
    entityToStagedTreeNodeMap = se_hash_map_create(sizeof(CreEntity), sizeof(SceneTreeNode**), SE_HASH_MAP_MIN_CAPACITY);
    cre_scene_template_cache_initialize();
}

void cre_scene_manager_finalize() {
    SE_ASSERT(entityToTreeNodeMap != NULL);
    SE_ASSERT(entityToStagedTreeNodeMap != NULL);
    se_hash_map_destroy(entityToTreeNodeMap);
    se_hash_map_destroy(entityToStagedTreeNodeMap);
    cre_scene_template_cache_finalize();
}

void cre_scene_manager_queue_node_for_creation(SceneTreeNode* treeNode) {
    entitiesQueuedForCreation[entitiesQueuedForCreationSize++] = treeNode->entity;
    SE_ASSERT_FMT(!se_hash_map_has(entityToTreeNodeMap, &treeNode->entity), "Entity '%d' already in entity to tree map!", treeNode->entity);
    se_hash_map_add(entityToTreeNodeMap, &treeNode->entity, &treeNode);
}

void cre_scene_manager_stage_child_node_to_be_added_later(SceneTreeNode* treeNode) {
    SE_ASSERT_FMT(!se_hash_map_has(entityToTreeNodeMap, &treeNode->entity), "Entity '%d' already staged to be added!", treeNode->entity);
    se_hash_map_add(entityToStagedTreeNodeMap, &treeNode->entity, &treeNode);
}

void cre_scene_manager_process_queued_creation_entities() {
    for (size_t i = 0; i < entitiesQueuedForCreationSize; i++) {
        const CreEntity queuedEntity = entitiesQueuedForCreation[i];
        ska_ecs_system_update_entity_signature_with_systems(queuedEntity);
        ska_ecs_system_event_entity_start(queuedEntity);
        ska_ecs_system_event_entity_entered_scene(queuedEntity);

#ifdef CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA
        Transform2DComponent* transformComponent = (Transform2DComponent*)ska_ecs_component_manager_get_component(queuedEntity, TRANSFORM2D_COMPONENT_INDEX);
        if (transformComponent) {
            entityPrevGlobalTransforms[queuedEntity] = transformComponent->localTransform; // TODO: globalTransform isn't filled out yet, change that...
        }
#endif
    }
    entitiesQueuedForCreationSize = 0;
}

void cre_scene_manager_queue_entity_for_deletion(CreEntity entity) {
    // Check if entity is already queued exit out early if so
    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        if (entitiesQueuedForDeletion[i] == entity) {
            se_logger_warn("Entity '%d' already queued for deletion!", entity);
            return;
        }
    }
    // Insert queued entity
    entitiesQueuedForDeletion[entitiesQueuedForDeletionSize++] = entity;
    // Clean up
    ska_ecs_system_event_entity_end(entity);
}

void cre_scene_manager_process_queued_deletion_entities() {
    for (size_t i = 0; i < entitiesToUnlinkParent_count; i++) {
        SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) se_hash_map_get(entityToTreeNodeMap,&entitiesToUnlinkParent[i]);
        SceneTreeNode* parentNode = treeNode->parent;
        SE_ARRAY_REMOVE_AND_CONDENSE(parentNode->children, parentNode->childCount, treeNode, NULL);
    }
    entitiesToUnlinkParent_count = 0;

    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        // Remove entity from entity to tree node map
        CreEntity entityToDelete = entitiesQueuedForDeletion[i];
        SE_ASSERT_FMT(se_hash_map_has(entityToTreeNodeMap, &entityToDelete), "Entity '%d' not in tree node map!?", entityToDelete);
        SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) se_hash_map_get(entityToTreeNodeMap,&entityToDelete);
        SE_MEM_FREE(treeNode);
        se_hash_map_erase(entityToTreeNodeMap, &entityToDelete);
        // Remove entity from systems
        ska_ecs_system_remove_entity_from_all_systems(entityToDelete);
        // Remove shader instances if applicable
        SpriteComponent* spriteComponent = (SpriteComponent*)ska_ecs_component_manager_get_component(entityToDelete, SPRITE_COMPONENT_INDEX);
        if (spriteComponent != NULL && spriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID) {
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(spriteComponent->shaderInstanceId);
            if (shaderInstance) {
                se_shader_cache_remove_instance(spriteComponent->shaderInstanceId);
                se_shader_instance_destroy(shaderInstance);
            }
        }
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent*)ska_ecs_component_manager_get_component(entityToDelete, ANIMATED_SPRITE_COMPONENT_INDEX);
        if (animatedSpriteComponent != NULL && animatedSpriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID) {
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(animatedSpriteComponent->shaderInstanceId);
            if (shaderInstance) {
                se_shader_cache_remove_instance(animatedSpriteComponent->shaderInstanceId);
                se_shader_instance_destroy(shaderInstance);
            }
        }
        // Remove all components
        ska_ecs_component_manager_remove_all_components(entityToDelete);
        // Return entity id to pool
        ska_ecs_entity_return(entityToDelete);

#ifdef CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA
        // Reset entity previous position
        entityPrevGlobalTransforms[entityToDelete] = SKA_TRANSFORM_IDENTITY;
#endif

    }
    entitiesQueuedForDeletionSize = 0;
}

void cre_scene_manager_queue_scene_change(const char* scenePath) {
    if (queuedSceneToChangeTo == NULL) {
        queuedSceneToChangeTo = cre_scene_create_scene(scenePath);
    } else {
        se_logger_warn("Scene already queued, not loading '%s'", scenePath);
    }
}

void cre_queue_destroy_tree_node_entity(SceneTreeNode* treeNode) {
    cre_scene_manager_queue_entity_for_deletion(treeNode->entity);
}

void cre_queue_destroy_tree_node_entity_all(SceneTreeNode* treeNode) {
    NodeComponent* nodeComp = (NodeComponent*)ska_ecs_component_manager_get_component(treeNode->entity, NODE_COMPONENT_INDEX);
    if (nodeComp) {
        if (nodeComp->queuedForDeletion) {
            se_logger_warn("Entity '%s' already queued for deletion, skipping queue deletion!", nodeComp->name);
            return;
        }
        nodeComp->queuedForDeletion = true;
    }
    cre_scene_execute_on_all_tree_nodes(treeNode, cre_queue_destroy_tree_node_entity);
    if (treeNode->parent != NULL) {
        SE_STATIC_ARRAY_ADD(entitiesToUnlinkParent, treeNode->entity);
    }
}

void cre_scene_manager_process_queued_scene_change() {
    if (queuedSceneToChangeTo != NULL) {
        // Destroy old scene
        if (activeScene != NULL) {
            cre_queue_destroy_tree_node_entity_all(activeScene->sceneTree->root);
            SE_MEM_FREE(activeScene);
        }

        // Reset Camera
        cre_camera_manager_reset_current_camera();

        // Setup new scene
        activeScene = queuedSceneToChangeTo;
        queuedSceneToChangeTo = NULL;
        SE_ASSERT(activeScene->scenePath != NULL);
        // Load scene file
        JsonSceneNode* rootJsonSceneNode = cre_json_load_scene_file(activeScene->scenePath);
        SE_ASSERT_FMT(rootJsonSceneNode != NULL, "Root scene file at path '%s' is NULL!", activeScene->scenePath);
        cre_scene_manager_setup_scene_nodes_from_json(rootJsonSceneNode);
        cre_json_delete_json_scene_node(rootJsonSceneNode);
    }
}

void cre_scene_manager_set_active_scene_root(SceneTreeNode* root) {
    SE_ASSERT_FMT(activeScene != NULL, "There is no active scene!");
    SE_ASSERT_FMT(activeScene->sceneTree->root == NULL, "Trying to overwrite an already existing scene root!");
    activeScene->sceneTree->root = root;
}

SceneTreeNode* cre_scene_manager_get_active_scene_root() {
    if (activeScene != NULL && activeScene->sceneTree != NULL) {
        return activeScene->sceneTree->root;
    }
    return NULL;
}

// TODO: Make function to update flags for all children
SKATransformModel2D* cre_scene_manager_get_scene_node_global_transform(CreEntity entity, Transform2DComponent* transform2DComponent) {
    SE_ASSERT_FMT(transform2DComponent != NULL, "Transform Model is NULL for entity '%d'", entity);
    if (transform2DComponent->isGlobalTransformDirty) {
        // Walk up scene to root node and calculate global transform
        cre_scene_utils_update_global_transform_model(entity, &transform2DComponent->globalTransform);
        // Flag is no longer dirty since the global transform is up to date
        transform2DComponent->isGlobalTransformDirty = false;
    }
    return &transform2DComponent->globalTransform;
}

SceneNodeRenderResource cre_scene_manager_get_scene_node_global_render_resource(CreEntity entity, Transform2DComponent* transform2DComponent, const SKAVector2* origin) {
    SKATransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transform2DComponent);

    cre_scene_utils_apply_camera_and_origin_translation(globalTransform, origin, transform2DComponent->ignoreCamera);
    transform2DComponent->isGlobalTransformDirty = true; // TODO: Make global transform const
    const SKATransform2D transform2D = ska_transform2d_model_convert_to_transform(globalTransform);
#ifdef CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA
    const SKATransform2D prevTransform2D = entityPrevGlobalTransforms[entity];
    // Favor keeping most of the previous value as that seems to look best especially when changes are done in '_fixed_process'
    const SKATransform2D lerpedTransform2D = ska_transform2d_lerp(&prevTransform2D, &transform2D, CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA);
    entityPrevGlobalTransforms[entity] = lerpedTransform2D; // Store prev
    return (SceneNodeRenderResource){
        .transform2D = lerpedTransform2D,
        .globalZIndex = globalTransform->zIndex
    };
#endif // CRE_SCENE_MANAGER_RENDER_INTERPOLATE_TRANSFORM2D_ALPHA
    return (SceneNodeRenderResource){
        .transform2D = transform2D,
        .globalZIndex = globalTransform->zIndex
    };
}


float cre_scene_manager_get_node_full_time_dilation(CreEntity entity) {
    NodeComponent* nodeComp = (NodeComponent*)ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    // The only thing in the scene without nodes current are native script entities
    if (nodeComp == NULL) {
        return cre_world_get_time_dilation();
    }
    // Early out for cached value if the value is still valid
    if (!nodeComp->timeDilation.cacheInvalid) {
        return nodeComp->timeDilation.cachedFullValue;
    }
    // Set starting value
    nodeComp->timeDilation.cachedFullValue = cre_world_get_time_dilation();
    const EntityArray entityArray = cre_scene_manager_get_self_and_parent_nodes(entity);
    for (size_t i = 0; (int) i < entityArray.entityCount; i++) {
        NodeComponent* entityNodeComp = (NodeComponent*)ska_ecs_component_manager_get_component(entityArray.entities[i],NODE_COMPONENT_INDEX);
        SE_ASSERT_FMT(entityNodeComp != NULL, "node comp is NULL!");
        nodeComp->timeDilation.cachedFullValue *= entityNodeComp->timeDilation.value;
    }
    nodeComp->timeDilation.cacheInvalid = false;
    return nodeComp->timeDilation.cachedFullValue;
}

SceneTreeNode* cre_scene_manager_get_entity_tree_node(CreEntity entity) {
    SE_ASSERT_FMT(se_hash_map_has(entityToTreeNodeMap, &entity), "Doesn't have entity '%d' in scene tree!", entity);
    SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) se_hash_map_get(entityToTreeNodeMap, &entity);
    SE_ASSERT_FMT(treeNode != NULL, "Failed to get tree node for entity '%d'", entity);
    return treeNode;
}

SceneTreeNode* cre_scene_manager_pop_staged_entity_tree_node(CreEntity entity) {
    SE_ASSERT_FMT(se_hash_map_has(entityToStagedTreeNodeMap, &entity), "Doesn't have entity '%d' in scene tree!", entity);
    SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) se_hash_map_get(entityToStagedTreeNodeMap, &entity);
    SE_ASSERT_FMT(treeNode != NULL, "Failed to get tree node for entity '%d'", entity);
    // Now that we have the staged tree node, remove the reference from the staged hash map
    se_hash_map_erase(entityToStagedTreeNodeMap, &entity);
    return treeNode;
}

CreEntity cre_scene_manager_get_entity_child_by_name(CreEntity parent, const char* childName) {
    SceneTreeNode* parentNode = cre_scene_manager_get_entity_tree_node(parent);
    for (size_t childIndex = 0; childIndex < parentNode->childCount; childIndex++) {
        const CreEntity childEntity = parentNode->children[childIndex]->entity;
        if (ska_ecs_component_manager_has_component(childEntity, NODE_COMPONENT_INDEX)) {
            NodeComponent* childNodeComponent = (NodeComponent*) ska_ecs_component_manager_get_component(childEntity,NODE_COMPONENT_INDEX);
            if (strcmp(childNodeComponent->name, childName) == 0) {
                return childEntity;
            }
        }
    }
    return CRE_NULL_ENTITY;
}

bool cre_scene_manager_has_entity_tree_node(CreEntity entity) {
    return se_hash_map_has(entityToTreeNodeMap, &entity);
}

void cre_scene_manager_add_node_as_child(CreEntity childEntity, CreEntity parentEntity) {
    SceneTreeNode* parentNode = cre_scene_manager_get_entity_tree_node(parentEntity);
    SceneTreeNode* node = cre_scene_manager_pop_staged_entity_tree_node(childEntity);
    node->parent = parentNode;
    SE_ASSERT(parentNode->childCount + 1 < SCENE_TREE_NODE_MAX_CHILDREN);
    parentNode->children[parentNode->childCount++] = node;
    cre_scene_manager_queue_node_for_creation(node);
    // If there are child nodes, they are already parented to the current child entity
    for (size_t i = 0; i < node->childCount; i++) {
        cre_scene_manager_add_staged_node_children_to_scene(node->children[i]);
    }
}

// A recursive functions to add already setup child nodes to the scene
void cre_scene_manager_add_staged_node_children_to_scene(SceneTreeNode* treeNode) {
    cre_scene_manager_queue_node_for_creation(treeNode);
    for (size_t i = 0; i < treeNode->childCount; i++) {
        cre_scene_manager_add_staged_node_children_to_scene(treeNode->children[i]);
    }
}

EntityArray cre_scene_manager_get_self_and_parent_nodes(CreEntity entity) {
    EntityArray combineModelResult = { .entityCount = 0 };
    combineModelResult.entities[combineModelResult.entityCount++] = entity;

    SceneTreeNode* sceneTreeNode = cre_scene_manager_get_entity_tree_node(entity);
    SceneTreeNode* parentTreeNode = sceneTreeNode->parent;
    while (parentTreeNode != NULL) {
        combineModelResult.entities[combineModelResult.entityCount++] = parentTreeNode->entity;
        parentTreeNode = parentTreeNode->parent;
    }
    return combineModelResult;
}

void cre_scene_manager_invalidate_time_dilation_nodes_with_children(CreEntity entity) {
    NodeComponent* nodeComponent = ska_ecs_component_manager_get_component(entity, NODE_COMPONENT_INDEX);
    nodeComponent->timeDilation.cacheInvalid = true;
    if (cre_scene_manager_has_entity_tree_node(entity)) {
        SceneTreeNode* sceneTreeNode = cre_scene_manager_get_entity_tree_node(entity);
        for (size_t i = 0; i < sceneTreeNode->childCount; i++) {
            cre_scene_manager_invalidate_time_dilation_nodes_with_children(sceneTreeNode->children[i]->entity);
        }
    }
}

void cre_scene_manager_notify_all_on_transform_events(CreEntity entity, Transform2DComponent* transformComp) {
    se_event_notify_observers(&transformComp->onTransformChanged, &(SESubjectNotifyPayload) {
            .data = &(CreComponentEntityUpdatePayload) {.entity = entity, .component = transformComp, .componentType = TRANSFORM2D_COMPONENT_TYPE}
    });
    // Notify children by recursion
    if (cre_scene_manager_has_entity_tree_node(entity)) {
        SceneTreeNode* sceneTreeNode = cre_scene_manager_get_entity_tree_node(entity);
        for (size_t i = 0; i < sceneTreeNode->childCount; i++) {
            const CreEntity childEntity = sceneTreeNode->children[i]->entity;
            Transform2DComponent* childTransformComp = (Transform2DComponent*) ska_ecs_component_manager_get_component(childEntity, TRANSFORM2D_COMPONENT_INDEX);
            if (childTransformComp != NULL) {
                cre_scene_manager_notify_all_on_transform_events(childEntity, childTransformComp);
            }
        }
    }
}

void cre_scene_manager_execute_on_root_and_child_nodes(ExecuteOnAllTreeNodesFunc func) {
    SceneTreeNode* node = cre_scene_manager_get_active_scene_root();
    if (node != NULL) {
        cre_scene_execute_on_all_tree_nodes(node, func);
    }
}

// Scene node setup
// Setup nodes and components from passed in json scene node.  Will return the root tree node
SceneTreeNode* cre_scene_manager_setup_json_scene_node(JsonSceneNode* jsonSceneNode, SceneTreeNode* parent, bool isStagedNodes);

void cre_scene_manager_setup_scene_nodes_from_json(JsonSceneNode* jsonSceneNode) {
    cre_scene_manager_setup_json_scene_node(jsonSceneNode, NULL, false);
}

SceneTreeNode* cre_scene_manager_stage_scene_nodes_from_json(JsonSceneNode* jsonSceneNode) {
    return cre_scene_manager_setup_json_scene_node(jsonSceneNode, NULL, true);
}

// Recursive
SceneTreeNode* cre_scene_manager_setup_json_scene_node(JsonSceneNode* jsonSceneNode, SceneTreeNode* parent, bool isStagedNodes) {
    SceneTreeNode* node = cre_scene_tree_create_tree_node(ska_ecs_entity_create(), parent);

    const bool isRoot = parent == NULL;
    if (isRoot && !isStagedNodes) {
        cre_scene_manager_set_active_scene_root(node);
    }  else if (parent) {
        SE_ASSERT(parent->childCount + 1 < SCENE_TREE_NODE_MAX_CHILDREN);
        parent->children[parent->childCount++] = node;
    }

    // Components
    NodeComponent* nodeComponent = node_component_create();
    se_strcpy(nodeComponent->name, jsonSceneNode->name);
    nodeComponent->type = jsonSceneNode->type;
    SE_ASSERT_FMT(nodeComponent->type != NodeBaseType_INVALID, "Node '%s' has an invalid node type '%d'", nodeComponent->name, nodeComponent->type);
    ska_ecs_component_manager_set_component(node->entity, NODE_COMPONENT_INDEX, nodeComponent);
    se_logger_info("Creating entity - name: '%s', entity_id = '%d', type: '%s'", nodeComponent->name, node->entity, node_get_base_type_string(nodeComponent->type));

    if (jsonSceneNode->components[TRANSFORM2D_COMPONENT_INDEX] != NULL) {
        Transform2DComponent* transform2DComponent = transform2d_component_copy((Transform2DComponent*) jsonSceneNode->components[TRANSFORM2D_COMPONENT_INDEX]);
        ska_ecs_component_manager_set_component(node->entity, TRANSFORM2D_COMPONENT_INDEX, transform2DComponent);
    }
    if (jsonSceneNode->components[SPRITE_COMPONENT_INDEX] != NULL) {
        SpriteComponent* spriteComponent = sprite_component_copy((SpriteComponent*) jsonSceneNode->components[SPRITE_COMPONENT_INDEX]);
        spriteComponent->texture = se_asset_manager_get_texture(jsonSceneNode->spriteTexturePath);
        if (jsonSceneNode->shaderInstanceShaderPath) {
            spriteComponent->shaderInstanceId = se_shader_cache_create_instance_and_add(jsonSceneNode->shaderInstanceShaderPath);
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(spriteComponent->shaderInstanceId);
            se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
        } else if (jsonSceneNode->shaderInstanceVertexPath && jsonSceneNode->shaderInstanceFragmentPath) {
            spriteComponent->shaderInstanceId = se_shader_cache_create_instance_and_add_from_raw(
                    jsonSceneNode->shaderInstanceVertexPath, jsonSceneNode->shaderInstanceFragmentPath);
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(spriteComponent->shaderInstanceId);
            se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
        } else {
            spriteComponent->shaderInstanceId = SE_SHADER_INSTANCE_INVALID_ID;
        }
        ska_ecs_component_manager_set_component(node->entity, SPRITE_COMPONENT_INDEX, spriteComponent);
    }
    if (jsonSceneNode->components[ANIMATED_SPRITE_COMPONENT_INDEX] != NULL) {
        AnimatedSpriteComponent* animatedSpriteComponent = animated_sprite_component_data_copy_to_animated_sprite((AnimatedSpriteComponentData*) jsonSceneNode->components[CreComponentDataIndex_ANIMATED_SPRITE]);
        if (jsonSceneNode->shaderInstanceShaderPath) {
            animatedSpriteComponent->shaderInstanceId = se_shader_cache_create_instance_and_add(jsonSceneNode->shaderInstanceShaderPath);
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(animatedSpriteComponent->shaderInstanceId);
            se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
        } else if (jsonSceneNode->shaderInstanceVertexPath && jsonSceneNode->shaderInstanceFragmentPath) {
            animatedSpriteComponent->shaderInstanceId = se_shader_cache_create_instance_and_add_from_raw(
                    jsonSceneNode->shaderInstanceVertexPath, jsonSceneNode->shaderInstanceFragmentPath);
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(animatedSpriteComponent->shaderInstanceId);
            se_renderer_set_sprite_shader_default_params(shaderInstance->shader);
        } else {
            animatedSpriteComponent->shaderInstanceId = SE_SHADER_INSTANCE_INVALID_ID;
        }
        ska_ecs_component_manager_set_component(node->entity, ANIMATED_SPRITE_COMPONENT_INDEX, animatedSpriteComponent);
    }
    if (jsonSceneNode->components[TEXT_LABEL_COMPONENT_INDEX] != NULL) {
        TextLabelComponent* textLabelComponent = text_label_component_copy((TextLabelComponent*) jsonSceneNode->components[TEXT_LABEL_COMPONENT_INDEX]);
        if (jsonSceneNode->fontUID != NULL) {
            textLabelComponent->font = se_asset_manager_get_font(jsonSceneNode->fontUID);
        } else {
            textLabelComponent->font = se_asset_manager_get_font(CRE_DEFAULT_FONT_ASSET.uid);
        }
        ska_ecs_component_manager_set_component(node->entity, TEXT_LABEL_COMPONENT_INDEX, textLabelComponent);
    }
    if (jsonSceneNode->components[SCRIPT_COMPONENT_INDEX] != NULL) {
        ScriptComponent* scriptComponent = script_component_copy((ScriptComponent*)jsonSceneNode->components[SCRIPT_COMPONENT_INDEX]);
        ska_ecs_component_manager_set_component(node->entity, SCRIPT_COMPONENT_INDEX, scriptComponent);
    }
    if (jsonSceneNode->components[COLLIDER2D_COMPONENT_INDEX] != NULL) {
        Collider2DComponent* collider2DComponent = collider2d_component_copy((Collider2DComponent*)jsonSceneNode->components[COLLIDER2D_COMPONENT_INDEX]);
        ska_ecs_component_manager_set_component(node->entity, COLLIDER2D_COMPONENT_INDEX, collider2DComponent);
    }
    if (jsonSceneNode->components[COLOR_RECT_COMPONENT_INDEX] != NULL) {
        ColorRectComponent* colorSquareComponent = color_rect_component_copy((ColorRectComponent*)jsonSceneNode->components[COLOR_RECT_COMPONENT_INDEX]);
        ska_ecs_component_manager_set_component(node->entity, COLOR_RECT_COMPONENT_INDEX, colorSquareComponent);
    }
    if (jsonSceneNode->components[PARTICLES2D_COMPONENT_INDEX] != NULL) {
        ParallaxComponent* parallaxComponent = parallax_component_copy((ParallaxComponent*)jsonSceneNode->components[PARTICLES2D_COMPONENT_INDEX]);
        ska_ecs_component_manager_set_component(node->entity, PARTICLES2D_COMPONENT_INDEX, parallaxComponent);
    }
    if (jsonSceneNode->components[PARTICLES2D_COMPONENT_INDEX] != NULL) {
        Particles2DComponent* particles2DComponent = particles2d_component_copy((Particles2DComponent*)jsonSceneNode->components[PARTICLES2D_COMPONENT_INDEX]);
        ska_ecs_component_manager_set_component(node->entity, PARTICLES2D_COMPONENT_INDEX, particles2DComponent);
    }

    if (!isStagedNodes) {
        cre_scene_manager_queue_node_for_creation(node);
    } else if (isRoot) {
        // Staged nodes only need to add root as the children are added within a recursive function
        cre_scene_manager_stage_child_node_to_be_added_later(node);
    }

    // Children
    for (size_t i = 0; i < jsonSceneNode->childrenCount; i++) {
        cre_scene_manager_setup_json_scene_node(jsonSceneNode->children[i], node, isStagedNodes);
    }

    return node;
}
