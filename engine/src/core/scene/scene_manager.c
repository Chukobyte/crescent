#include "scene_manager.h"

#include <string.h>

#include "../seika/src/utils/logger.h"
#include "../seika/src/asset_manager.h"
#include "../seika/src/utils/rbe_assert.h"
#include "../seika/src/memory/rbe_mem.h"
#include "../seika/src/data_structures/rbe_hash_map.h"
#include "../seika/src/data_structures/rbe_static_array.h"

#include "scene_utils.h"
#include "../ecs/system/ec_system.h"
#include "../ecs/component/sprite_component.h"
#include "../ecs/component/animated_sprite_component.h"
#include "../ecs/component/text_label_component.h"
#include "../ecs/component/script_component.h"
#include "../ecs/component/collider2d_component.h"
#include "../ecs/component/color_rect_component.h"
#include "../camera/camera_manager.h"
#include "../json/json_file_loader.h"

// --- Scene Tree --- //
typedef void (*ExecuteOnAllTreeNodesFunc) (SceneTreeNode*);

// Executes function on passed in tree node and all child tree nodes
void rbe_scene_execute_on_all_tree_nodes(SceneTreeNode* treeNode, ExecuteOnAllTreeNodesFunc func) {
    for (size_t i = 0; i < treeNode->childCount; i++) {
        rbe_scene_execute_on_all_tree_nodes(treeNode->children[i], func);
    }
    func(treeNode);
}

typedef struct SceneTree {
    SceneTreeNode* root;
} SceneTree;

SceneTreeNode* rbe_scene_tree_create_tree_node(Entity entity, SceneTreeNode* parent) {
    SceneTreeNode* treeNode = RBE_MEM_ALLOCATE(SceneTreeNode);
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

Scene* rbe_scene_create_scene(const char* scenePath) {
    Scene* scene = RBE_MEM_ALLOCATE(Scene);
    scene->scenePath = scenePath;
    scene->sceneTree = RBE_MEM_ALLOCATE(SceneTree);
    scene->sceneTree->root = NULL;
    return scene;
}

// --- Scene Manager --- //
Entity entitiesQueuedForCreation[MAX_ENTITIES];
size_t entitiesQueuedForCreationSize = 0;
Entity entitiesQueuedForDeletion[MAX_ENTITIES];
size_t entitiesQueuedForDeletionSize = 0;

RBE_STATIC_ARRAY_CREATE(Entity, MAX_ENTITIES, entitiesToUnlinkParent);

Scene* activeScene = NULL;
Scene* queuedSceneToChangeTo = NULL;

static RBEHashMap* entityToTreeNodeMap = NULL;

void rbe_scene_manager_setup_scene_nodes_from_json(JsonSceneNode* jsonSceneNode);

void rbe_scene_manager_initialize() {
    RBE_ASSERT(entityToTreeNodeMap == NULL);
    entityToTreeNodeMap = rbe_hash_map_create(sizeof(Entity), sizeof(SceneTreeNode**), 16); // TODO: Update capacity
}

void rbe_scene_manager_finalize() {
    RBE_ASSERT(entityToTreeNodeMap != NULL);
    rbe_hash_map_destroy(entityToTreeNodeMap);
}

void rbe_scene_manager_queue_entity_for_creation(SceneTreeNode* treeNode) {
    entitiesQueuedForCreation[entitiesQueuedForCreationSize++] = treeNode->entity;
    RBE_ASSERT_FMT(!rbe_hash_map_has(entityToTreeNodeMap, &treeNode->entity), "Entity '%d' already in entity to tree map!", treeNode->entity);
    rbe_hash_map_add(entityToTreeNodeMap, &treeNode->entity, &treeNode);
}

void rbe_scene_manager_process_queued_creation_entities() {
    for (size_t i = 0; i < entitiesQueuedForCreationSize; i++) {
        rbe_ec_system_entity_start(entitiesQueuedForCreation[i]);
    }
    entitiesQueuedForCreationSize = 0;
}

void rbe_scene_manager_queue_entity_for_deletion(Entity entity) {
    // Check if entity is already queued exit out early if so
    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        if (entitiesQueuedForDeletion[i] == entity) {
            rbe_logger_warn("Entity '%d' already queued for deletion!", entity);
            return;
        }
    }
    // Insert queued entity
    entitiesQueuedForDeletion[entitiesQueuedForDeletionSize++] = entity;
    // Clean up
    rbe_ec_system_entity_end(entity);
}

void rbe_scene_manager_process_queued_deletion_entities() {
    for (size_t i = 0; i < entitiesToUnlinkParent_count; i++) {
        SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) rbe_hash_map_get(entityToTreeNodeMap, &entitiesToUnlinkParent[i]);
        SceneTreeNode* parentNode = treeNode->parent;
        CRE_ARRAY_REMOVE_AND_CONDENSE(parentNode->children, parentNode->childCount, treeNode, NULL);
    }
    entitiesToUnlinkParent_count = 0;

    for (size_t i = 0; i < entitiesQueuedForDeletionSize; i++) {
        // Remove entity from entity to tree node map
        Entity entityToDelete = entitiesQueuedForDeletion[i];
        RBE_ASSERT_FMT(rbe_hash_map_has(entityToTreeNodeMap, &entityToDelete), "Entity '%d' not in tree node map!?", entityToDelete);
        SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) rbe_hash_map_get(entityToTreeNodeMap, &entityToDelete);
        RBE_MEM_FREE(treeNode);
        rbe_hash_map_erase(entityToTreeNodeMap, &entityToDelete);
        // Remove entity from systems
        rbe_ec_system_remove_entity_from_all_systems(entityToDelete);
        // Remove all components
        component_manager_remove_all_components(entityToDelete);
        // Return entity id to pool
        cre_ec_system_return_entity_uid(entityToDelete);
    }
    entitiesQueuedForDeletionSize = 0;
}

void rbe_scene_manager_queue_scene_change(const char* scenePath) {
    if (queuedSceneToChangeTo == NULL) {
        queuedSceneToChangeTo = rbe_scene_create_scene(scenePath);
    } else {
        rbe_logger_warn("Scene already queued, not loading '%s'", scenePath);
    }
}

void rbe_queue_destroy_tree_node_entity(SceneTreeNode* treeNode) {
    rbe_scene_manager_queue_entity_for_deletion(treeNode->entity);
}

void rbe_queue_destroy_tree_node_entity_all(SceneTreeNode* treeNode) {
    rbe_scene_execute_on_all_tree_nodes(treeNode, rbe_queue_destroy_tree_node_entity);
    if (treeNode->parent != NULL) {
        RBE_STATIC_ARRAY_ADD(entitiesToUnlinkParent, treeNode->entity);
    }
}

void rbe_scene_manager_process_queued_scene_change() {
    if (queuedSceneToChangeTo != NULL) {
        // Destroy old scene
        if (activeScene != NULL) {
            rbe_queue_destroy_tree_node_entity_all(activeScene->sceneTree->root);
            RBE_MEM_FREE(activeScene);
        }

        // Reset Camera
        rbe_camera_manager_reset_current_camera();

        // Setup new scene
        activeScene = queuedSceneToChangeTo;
        queuedSceneToChangeTo = NULL;
        RBE_ASSERT(activeScene->scenePath != NULL);
        // Load scene file
        JsonSceneNode* rootJsonSceneNode = cre_json_load_scene_file(activeScene->scenePath);
        RBE_ASSERT_FMT(rootJsonSceneNode != NULL, "Root scene file at path '%s' is NULL!", activeScene->scenePath);
        rbe_scene_manager_setup_scene_nodes_from_json(rootJsonSceneNode);
        cre_json_delete_json_scene_node(rootJsonSceneNode);
    }
}

void rbe_scene_manager_set_active_scene_root(SceneTreeNode* root) {
    RBE_ASSERT_FMT(activeScene != NULL, "There is no active scene!");
    RBE_ASSERT_FMT(activeScene->sceneTree->root == NULL, "Trying to overwrite an already existing scene root!");
    activeScene->sceneTree->root = root;
}

SceneTreeNode* rbe_scene_manager_get_active_scene_root() {
    if (activeScene != NULL && activeScene->sceneTree != NULL) {
        return activeScene->sceneTree->root;
    }
    return NULL;
}

// TODO: Make function to update flags for all children
TransformModel2D* rbe_scene_manager_get_scene_node_global_transform(Entity entity, Transform2DComponent* transform2DComponent) {
    RBE_ASSERT_FMT(transform2DComponent != NULL, "Transform Model is NULL for entity '%d'", entity);
    if (transform2DComponent->isGlobalTransformDirty) {
        // Walk up scene to root node and calculate global transform
        cre_scene_utils_update_global_transform_model(entity, &transform2DComponent->globalTransform);
        // Flag is no longer dirty since the global transform is up to date
        transform2DComponent->isGlobalTransformDirty = false;
    }
    return &transform2DComponent->globalTransform;
}

SceneTreeNode* rbe_scene_manager_get_entity_tree_node(Entity entity) {
    RBE_ASSERT_FMT(rbe_hash_map_has(entityToTreeNodeMap, &entity), "Doesn't have entity '%d' in scene tree!", entity);
    SceneTreeNode* treeNode = (SceneTreeNode*) *(SceneTreeNode**) rbe_hash_map_get(entityToTreeNodeMap, &entity);
    RBE_ASSERT_FMT(treeNode != NULL, "Failed to get tree node for entity '%d'", entity);
    return treeNode;
}

Entity rbe_scene_manager_get_entity_child_by_name(Entity parent, const char* childName) {
    SceneTreeNode* parentNode = rbe_scene_manager_get_entity_tree_node(parent);
    for (size_t childIndex = 0; childIndex < parentNode->childCount; childIndex++) {
        const Entity childEntity = parentNode->children[childIndex]->entity;
        if (component_manager_has_component(childEntity, ComponentDataIndex_NODE)) {
            NodeComponent* childNodeComponent = component_manager_get_component(childEntity, ComponentDataIndex_NODE);
            if (strcmp(childNodeComponent->name, childName) == 0) {
                return childEntity;
            }
        }
    }
    return NULL_ENTITY;
}

// Scene node setup
void rbe_scene_manager_setup_json_scene_node(JsonSceneNode* jsonSceneNode, SceneTreeNode* parent);

void rbe_scene_manager_setup_scene_nodes_from_json(JsonSceneNode* jsonSceneNode) {
    rbe_scene_manager_setup_json_scene_node(jsonSceneNode, NULL);
}

void rbe_scene_manager_setup_json_scene_node(JsonSceneNode* jsonSceneNode, SceneTreeNode* parent) {
    SceneTreeNode* node = rbe_scene_tree_create_tree_node(cre_ec_system_create_entity_uid(), parent);

    const bool isRoot = parent == NULL;
    if (isRoot) {
        rbe_scene_manager_set_active_scene_root(node);
    }  else {
        parent->children[parent->childCount++] = node;
    }

    // Components
    NodeComponent* nodeComponent = node_component_create();
    strcpy(nodeComponent->name, jsonSceneNode->name);
    nodeComponent->type = jsonSceneNode->type;
    RBE_ASSERT_FMT(nodeComponent->type != NodeBaseType_INVALID, "Node '%s' has an invalid node type '%d'", nodeComponent->name, nodeComponent->type);
    component_manager_set_component(node->entity, ComponentDataIndex_NODE, nodeComponent);
    rbe_logger_info("Creating entity - name: '%s', entity_id = '%d', type: '%s'", nodeComponent->name, node->entity,
                    node_get_base_type_string(nodeComponent->type));

    if (jsonSceneNode->components[ComponentDataIndex_TRANSFORM_2D] != NULL) {
        Transform2DComponent* transform2DComponent = transform2d_component_copy((Transform2DComponent*) jsonSceneNode->components[ComponentDataIndex_TRANSFORM_2D]);
        component_manager_set_component(node->entity, ComponentDataIndex_TRANSFORM_2D, transform2DComponent);
    }
    if (jsonSceneNode->components[ComponentDataIndex_SPRITE] != NULL) {
        SpriteComponent* spriteComponent = sprite_component_copy((SpriteComponent*) jsonSceneNode->components[ComponentDataIndex_SPRITE]);
        spriteComponent->texture = rbe_asset_manager_get_texture(jsonSceneNode->spriteTexturePath);
        component_manager_set_component(node->entity, ComponentDataIndex_SPRITE, spriteComponent);
    }
    if (jsonSceneNode->components[ComponentDataIndex_ANIMATED_SPRITE] != NULL) {
        AnimatedSpriteComponent* animatedSpriteComponent = animated_sprite_component_data_copy_to_animated_sprite((AnimatedSpriteComponentData*) jsonSceneNode->components[ComponentDataIndex_ANIMATED_SPRITE]);
        component_manager_set_component(node->entity, ComponentDataIndex_ANIMATED_SPRITE, animatedSpriteComponent);
    }
    if (jsonSceneNode->components[ComponentDataIndex_TEXT_LABEL] != NULL) {
        TextLabelComponent* textLabelComponent = text_label_component_copy((TextLabelComponent*) jsonSceneNode->components[ComponentDataIndex_TEXT_LABEL]);
        textLabelComponent->font = rbe_asset_manager_get_font(jsonSceneNode->fontUID);
        component_manager_set_component(node->entity, ComponentDataIndex_TEXT_LABEL, textLabelComponent);
    }
    if (jsonSceneNode->components[ComponentDataIndex_SCRIPT] != NULL) {
        ScriptComponent* scriptComponent = script_component_copy((ScriptComponent*) jsonSceneNode->components[ComponentDataIndex_SCRIPT]);
        component_manager_set_component(node->entity, ComponentDataIndex_SCRIPT, scriptComponent);
    }
    if (jsonSceneNode->components[ComponentDataIndex_COLLIDER_2D] != NULL) {
        Collider2DComponent* collider2DComponent = collider2d_component_copy((Collider2DComponent*) jsonSceneNode->components[ComponentDataIndex_COLLIDER_2D]);
        component_manager_set_component(node->entity, ComponentDataIndex_COLLIDER_2D, collider2DComponent);
    }
    if (jsonSceneNode->components[ComponentDataIndex_COLOR_RECT] != NULL) {
        ColorRectComponent* colorSquareComponent = color_rect_component_copy(
                    (ColorRectComponent*) jsonSceneNode->components[ComponentDataIndex_COLOR_RECT]);
        component_manager_set_component(node->entity, ComponentDataIndex_COLOR_RECT, colorSquareComponent);
    }

    rbe_ec_system_update_entity_signature_with_systems(node->entity);

    // Children
    for (size_t i = 0; i < jsonSceneNode->childrenCount; i++) {
        rbe_scene_manager_setup_json_scene_node(jsonSceneNode->children[i], node);
    }

    rbe_scene_manager_queue_entity_for_creation(node);
}
