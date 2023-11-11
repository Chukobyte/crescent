#include "scene_manager.h"

#include <string.h>

#include "../seika/rendering/shader/shader_cache.h"
#include "../seika/rendering/renderer.h"
#include "../seika/asset/asset_manager.h"
#include "../seika/utils/logger.h"
#include "../seika/utils/se_assert.h"
#include "../seika/data_structures/se_hash_map.h"
#include "../seika/data_structures/se_static_array.h"

#include "scene_utils.h"
#include "../world.h"
#include "../game_properties.h"
#include "../ecs/system/ec_system.h"
#include "../ecs/component/sprite_component.h"
#include "../ecs/component/animated_sprite_component.h"
#include "../ecs/component/text_label_component.h"
#include "../ecs/component/script_component.h"
#include "../ecs/component/collider2d_component.h"
#include "../ecs/component/color_rect_component.h"
#include "../camera/camera_manager.h"
#include "../ecs/component/parallax_component.h"
#include "scene_template_cache.h"

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
        cre_ec_system_update_entity_signature_with_systems(entitiesQueuedForCreation[i]);
        cre_ec_system_entity_start(entitiesQueuedForCreation[i]);
        cre_ec_system_entity_entered_scene(entitiesQueuedForCreation[i]);
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
    cre_ec_system_entity_end(entity);
}

void cre_scene_manager_process_queued_deletion_entities() {
    for (size_t i = 0; i < entitiesToUnlinkParent_count; i++) {
        SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) se_hash_map_get(entityToTreeNodeMap,
                                  &entitiesToUnlinkParent[i]);
        SceneTreeNode* parentNode = treeNode->parent;
        SE_ARRAY_REMOVE_AND_CONDENSE(parentNode->children, parentNode->childCount, treeNode, NULL);
    }
    entitiesToUnlinkParent_count = 0;

    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        // Remove entity from entity to tree node map
        CreEntity entityToDelete = entitiesQueuedForDeletion[i];
        SE_ASSERT_FMT(se_hash_map_has(entityToTreeNodeMap, &entityToDelete), "Entity '%d' not in tree node map!?", entityToDelete);
        SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) se_hash_map_get(entityToTreeNodeMap,
                                  &entityToDelete);
        SE_MEM_FREE(treeNode);
        se_hash_map_erase(entityToTreeNodeMap, &entityToDelete);
        // Remove entity from systems
        cre_ec_system_remove_entity_from_all_systems(entityToDelete);
        // Remove shader instance if applicable
        SpriteComponent* spriteComponent = cre_component_manager_get_component_unchecked(entityToDelete, CreComponentDataIndex_SPRITE);
        if (spriteComponent != NULL && spriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID) {
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(spriteComponent->shaderInstanceId);
            if (shaderInstance) {
                se_shader_cache_remove_instance(spriteComponent->shaderInstanceId);
                se_shader_instance_destroy(shaderInstance);
            }
        }
        AnimatedSpriteComponent* animatedSpriteComponent = cre_component_manager_get_component_unchecked(entityToDelete, CreComponentDataIndex_ANIMATED_SPRITE);
        if (animatedSpriteComponent != NULL && animatedSpriteComponent->shaderInstanceId != SE_SHADER_INSTANCE_INVALID_ID) {
            SEShaderInstance* shaderInstance = se_shader_cache_get_instance(animatedSpriteComponent->shaderInstanceId);
            if (shaderInstance) {
                se_shader_cache_remove_instance(animatedSpriteComponent->shaderInstanceId);
                se_shader_instance_destroy(shaderInstance);
            }
        }
        // Remove all components
        cre_component_manager_remove_all_components(entityToDelete);
        // Return entity id to pool
        cre_ec_system_return_entity_uid(entityToDelete);
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
    NodeComponent* nodeComp = cre_component_manager_get_component_unchecked(treeNode->entity, CreComponentDataIndex_NODE);
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
SETransformModel2D* cre_scene_manager_get_scene_node_global_transform(CreEntity entity, Transform2DComponent* transform2DComponent) {
    SE_ASSERT_FMT(transform2DComponent != NULL, "Transform Model is NULL for entity '%d'", entity);
    if (transform2DComponent->isGlobalTransformDirty) {
        // Walk up scene to root node and calculate global transform
        cre_scene_utils_update_global_transform_model(entity, &transform2DComponent->globalTransform);
        // Flag is no longer dirty since the global transform is up to date
        transform2DComponent->isGlobalTransformDirty = false;
    }
    return &transform2DComponent->globalTransform;
}

float cre_scene_manager_get_node_full_time_dilation(CreEntity entity) {
    NodeComponent* nodeComp = cre_component_manager_get_component_unchecked(entity, CreComponentDataIndex_NODE);
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
        NodeComponent* entityNodeComp = cre_component_manager_get_component_unchecked(entityArray.entities[i],
                                        CreComponentDataIndex_NODE);
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
        if (cre_component_manager_has_component(childEntity, CreComponentDataIndex_NODE)) {
            NodeComponent* childNodeComponent = cre_component_manager_get_component(childEntity,
                                                CreComponentDataIndex_NODE);
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
    NodeComponent* nodeComponent = cre_component_manager_get_component(entity, CreComponentDataIndex_NODE);
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
        .data = &(CreComponentEntityUpdatePayload) {.entity = entity, .component = transformComp, .componentType = CreComponentType_TRANSFORM_2D},
        .type = 0
    });
    // Notify children by recursion
    if (cre_scene_manager_has_entity_tree_node(entity)) {
        SceneTreeNode* sceneTreeNode = cre_scene_manager_get_entity_tree_node(entity);
        for (size_t i = 0; i < sceneTreeNode->childCount; i++) {
            const CreEntity childEntity = sceneTreeNode->children[i]->entity;
            Transform2DComponent* childTransformComp = (Transform2DComponent*) cre_component_manager_get_component_unchecked(
                        childEntity, CreComponentDataIndex_TRANSFORM_2D);
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
    SceneTreeNode* node = cre_scene_tree_create_tree_node(cre_ec_system_create_entity_uid(), parent);

    const bool isRoot = parent == NULL;
    if (isRoot && !isStagedNodes) {
        cre_scene_manager_set_active_scene_root(node);
    }  else if (parent) {
        SE_ASSERT(parent->childCount + 1 < SCENE_TREE_NODE_MAX_CHILDREN);
        parent->children[parent->childCount++] = node;
    }

    // Components
    NodeComponent* nodeComponent = node_component_create();
    strcpy(nodeComponent->name, jsonSceneNode->name);
    nodeComponent->type = jsonSceneNode->type;
    SE_ASSERT_FMT(nodeComponent->type != NodeBaseType_INVALID, "Node '%s' has an invalid node type '%d'", nodeComponent->name, nodeComponent->type);
    cre_component_manager_set_component(node->entity, CreComponentDataIndex_NODE, nodeComponent);
    se_logger_info("Creating entity - name: '%s', entity_id = '%d', type: '%s'", nodeComponent->name, node->entity,
                   node_get_base_type_string(nodeComponent->type));

    if (jsonSceneNode->components[CreComponentDataIndex_TRANSFORM_2D] != NULL) {
        Transform2DComponent* transform2DComponent = transform2d_component_copy((Transform2DComponent*) jsonSceneNode->components[CreComponentDataIndex_TRANSFORM_2D]);
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_TRANSFORM_2D, transform2DComponent);
    }
    if (jsonSceneNode->components[CreComponentDataIndex_SPRITE] != NULL) {
        SpriteComponent* spriteComponent = sprite_component_copy((SpriteComponent*) jsonSceneNode->components[CreComponentDataIndex_SPRITE]);
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
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_SPRITE, spriteComponent);
    }
    if (jsonSceneNode->components[CreComponentDataIndex_ANIMATED_SPRITE] != NULL) {
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
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_ANIMATED_SPRITE, animatedSpriteComponent);
    }
    if (jsonSceneNode->components[CreComponentDataIndex_TEXT_LABEL] != NULL) {
        TextLabelComponent* textLabelComponent = text_label_component_copy((TextLabelComponent*) jsonSceneNode->components[CreComponentDataIndex_TEXT_LABEL]);
        if (jsonSceneNode->fontUID != NULL) {
            textLabelComponent->font = se_asset_manager_get_font(jsonSceneNode->fontUID);
        } else {
            textLabelComponent->font = se_asset_manager_get_font(CRE_DEFAULT_FONT_ASSET.uid);
        }
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_TEXT_LABEL, textLabelComponent);
    }
    if (jsonSceneNode->components[CreComponentDataIndex_SCRIPT] != NULL) {
        ScriptComponent* scriptComponent = script_component_copy((ScriptComponent*) jsonSceneNode->components[CreComponentDataIndex_SCRIPT]);
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_SCRIPT, scriptComponent);
    }
    if (jsonSceneNode->components[CreComponentDataIndex_COLLIDER_2D] != NULL) {
        Collider2DComponent* collider2DComponent = collider2d_component_copy((Collider2DComponent*) jsonSceneNode->components[CreComponentDataIndex_COLLIDER_2D]);
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_COLLIDER_2D, collider2DComponent);
    }
    if (jsonSceneNode->components[CreComponentDataIndex_COLOR_RECT] != NULL) {
        ColorRectComponent* colorSquareComponent = color_rect_component_copy(
                    (ColorRectComponent*) jsonSceneNode->components[CreComponentDataIndex_COLOR_RECT]);
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_COLOR_RECT, colorSquareComponent);
    }
    if (jsonSceneNode->components[CreComponentDataIndex_PARALLAX] != NULL) {
        ParallaxComponent* parallaxComponent = parallax_component_copy((ParallaxComponent *) jsonSceneNode->components[CreComponentDataIndex_PARALLAX]);
        cre_component_manager_set_component(node->entity, CreComponentDataIndex_PARALLAX, parallaxComponent);
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
