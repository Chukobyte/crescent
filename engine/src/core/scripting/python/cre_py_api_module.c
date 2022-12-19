#include "cre_py_api_module.h"

#include <string.h>

#include "../seika/src/asset/asset_manager.h"
#include "../seika/src/input/input.h"
#include "../seika/src/input/mouse.h"
#include "../seika/src/audio/audio_manager.h"
#include "../seika/src/networking/se_network.h"
#include "../seika/src/memory/se_mem.h"
#include "../seika/src/rendering/render_context.h"
#include "../seika/src/utils/se_assert.h"
#include "../seika/src/utils/se_file_system_utils.h"
#include "../seika/src/utils/se_string_util.h"

#include "py_cache.h"
#include "py_script_context.h"
#include "../../engine_context.h"
#include "../../game_properties.h"
#include "../../node_event.h"
#include "../../world.h"
#include "../../scripting/script_context.h"
#include "../../physics/collision/collision.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../ecs/ecs_manager.h"
#include "../../ecs/system/ec_system.h"
#include "../../ecs/component/animated_sprite_component.h"
#include "../../ecs/component/color_rect_component.h"
#include "../../ecs/component/script_component.h"
#include "../../ecs/component/sprite_component.h"
#include "../../ecs/component/text_label_component.h"
#include "../../ecs/component/node_component.h"
#include "../../scene/scene_manager.h"
#include "../../ecs/component/parallax_component.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

//--- Helper Functions ---//
// TODO: May want to move into another location...

Vector2 py_api_mouse_get_global_position(SEMouse* mouse, Vector2* offset) {
    CRECamera2D* camera = cre_camera_manager_get_current_camera();
    SEMouse* globalMouse = se_mouse_get();
    CREGameProperties* gameProps = cre_game_props_get();
    RenderContext* renderContext = se_render_context_get();
    const Vector2 mouse_pixel_coord = {
        se_math_map_to_range(globalMouse->position.x, 0.0f, (float) renderContext->windowWidth, 0.0f, (float) gameProps->resolutionWidth),
        se_math_map_to_range(globalMouse->position.y, 0.0f, (float) renderContext->windowHeight, 0.0f, (float) gameProps->resolutionHeight)
    };
    const Vector2 mouseWorldPos = {
        (camera->viewport.x + camera->offset.x + mouse_pixel_coord.x + offset->x) * camera->zoom.x,
        (camera->viewport.y + camera->offset.y + mouse_pixel_coord.y + offset->y) * camera->zoom.y
    };
    return mouseWorldPos;
}

void py_api_update_entity_local_position(Entity entity, Vector2* position) {
    Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
    const Vector2 prevPosition = transformComp->localTransform.position;
    transformComp->localTransform.position.x = position->x;
    transformComp->localTransform.position.y = position->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.position.x != prevPosition.x || transformComp->localTransform.position.y != prevPosition.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void py_api_update_entity_local_scale(Entity entity, Vector2 * scale) {
    Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
    const Vector2 prevScale = transformComp->localTransform.scale;
    transformComp->localTransform.scale.x = scale->x;
    transformComp->localTransform.scale.y = scale->y;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.scale.x != prevScale.x || transformComp->localTransform.scale.y != prevScale.y) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

void py_api_update_entity_local_rotation(Entity entity, float rotation) {
    Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
    const float prevRotation = transformComp->localTransform.rotation;
    transformComp->localTransform.rotation = rotation;
    transformComp->isGlobalTransformDirty = true;
    if (transformComp->localTransform.rotation != prevRotation) {
        cre_scene_manager_notify_all_on_transform_events(entity, transformComp);
    }
}

//--- Py Utils ---//
PyObject* cre_py_utils_get_entity_instance(Entity entity);

//--- CRE PY API ---//

// Engine
PyObject* cre_py_api_engine_exit(PyObject* self, PyObject* args, PyObject* kwargs) {
    int exitCode;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiEngineExitKWList, &exitCode)) {
        CREEngineContext* engineContext = cre_engine_context_get();
        engineContext->isRunning = false;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_engine_set_target_fps(PyObject* self, PyObject* args, PyObject* kwargs) {
    int targetFPS;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiEngineSetTargetFPSKWList, &targetFPS)) {
        CREEngineContext* engineContext = cre_engine_context_get();
        engineContext->targetFPS = targetFPS;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_engine_get_target_fps(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    return Py_BuildValue("(f)", engineContext->targetFPS);
}

PyObject* cre_py_api_engine_get_average_fps(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    return Py_BuildValue("(f)", engineContext->stats.averageFPS);
}

PyObject* cre_py_api_engine_set_fps_display_enabled(PyObject* self, PyObject* args, PyObject* kwargs) {
    bool isEnabled;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "b", crePyApiGenericEnabledKWList, &isEnabled)) {
        cre_ecs_manager_enable_fps_display_entity(isEnabled);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_engine_get_global_physics_delta_time(PyObject* self, PyObject* args) {
    return Py_BuildValue("f", CRE_GLOBAL_PHYSICS_DELTA_TIME);
}

PyObject* PyInit_cre_py_API(void) {
    return PyModule_Create(&crePyAPIModDef);
}

// Input
PyObject* cre_py_api_input_add_action(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    char* actionValue;
    int deviceId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ssi", crePyApiInputAddActionKWList, &actionName, &actionValue, &deviceId)) {
        se_input_add_action_value(actionName, actionValue, deviceId);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_input_is_action_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiInputActionInputCheckKWList, &actionName)) {
        if (se_input_is_action_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_input_is_action_just_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiInputActionInputCheckKWList, &actionName)) {
        if (se_input_is_action_just_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_input_is_action_just_released(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiInputActionInputCheckKWList, &actionName)) {
        if (se_input_is_action_just_released(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

// Mouse
PyObject* cre_py_api_mouse_get_position(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    const SEMouse* globalMouse = se_mouse_get();
    return Py_BuildValue("(ff)", globalMouse->position.x, globalMouse->position.y);
}

PyObject* cre_py_api_mouse_get_world_position(PyObject* self, PyObject* args) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    SEMouse* globalMouse = se_mouse_get();
    static Vector2 zeroOffset = { 0.0f, 0.0f };
    const Vector2 worldPosition = py_api_mouse_get_global_position(globalMouse, &zeroOffset);
    return Py_BuildValue("(ff)", worldPosition.x, worldPosition.y);
}

// Camera
PyObject* cre_py_api_camera2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->viewport.x = x;
        camera2D->viewport.y = y;
        cre_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->viewport.x += x;
        camera2D->viewport.y += y;
        cre_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_position(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->viewport.x, camera2D->viewport.y);
}

PyObject* cre_py_api_camera2D_set_offset(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->offset.x = x;
        camera2D->offset.y = y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_add_to_offset(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->offset.x += x;
        camera2D->offset.y += y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_offset(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->offset.x, camera2D->offset.y);
}

PyObject* cre_py_api_camera2D_set_zoom(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->zoom.x = x;
        camera2D->zoom.y = y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_add_to_zoom(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", crePyApiGenericXYKWList, &x, &y)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->zoom.x += x;
        camera2D->zoom.y += y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_zoom(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->zoom.x, camera2D->zoom.y);
}

// Scene Tree
PyObject* cre_py_api_scene_tree_change_scene(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* scenePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiGenericPathKWList, &scenePath)) {
        cre_scene_manager_queue_scene_change(scenePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_scene_tree_get_root(PyObject* self, PyObject* args) {
    SceneTreeNode* rootNode = cre_scene_manager_get_active_scene_root();
    SE_ASSERT(rootNode != NULL);
    return cre_py_utils_get_entity_instance(rootNode->entity);
}

PyObject* cre_py_api_camera2D_set_boundary(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ffff", crePyApiGenericXYWHKWList, &x, &y, &w, &h)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        camera2D->boundary.x = x;
        camera2D->boundary.y = y;
        camera2D->boundary.w = w;
        camera2D->boundary.h = h;
        cre_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_get_boundary(PyObject* self, PyObject* args) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    return Py_BuildValue("(ffff)", camera2D->boundary.x, camera2D->boundary.y, camera2D->boundary.w, camera2D->boundary.h);
}

PyObject* cre_py_api_camera2D_follow_node(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        cre_camera2d_follow_entity(camera2D, entity);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_camera2D_unfollow_node(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
        cre_camera2d_unfollow_entity(camera2D, entity);
        Py_RETURN_NONE;
    }
    return NULL;
}

// World
PyObject* cre_py_api_world_set_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    float timeDilation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "f", crePyApiWorldSetTimeDilationKWList, &timeDilation)) {
        cre_world_set_time_dilation(timeDilation);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_world_get_time_dilation(PyObject* self, PyObject* args) {
    return Py_BuildValue("(f)", cre_world_get_time_dilation());
}

// Audio Manager
PyObject* cre_py_api_audio_manager_play_sound(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    bool loops;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sb", crePyApiAudioManagerPlaySoundKWList, &audioPath, &loops)) {
        se_audio_manager_play_sound(audioPath, loops);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_audio_manager_stop_sound(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiGenericPathKWList, &audioPath)) {
        se_audio_manager_stop_sound(audioPath);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Game Properties
PyObject* cre_py_api_game_properties_get(PyObject* self, PyObject* args) {
    const CREGameProperties* gameProps = cre_game_props_get();
    return Py_BuildValue("(siiiiisb)",
                         gameProps->gameTitle, gameProps->resolutionWidth, gameProps->resolutionHeight,
                         gameProps->windowWidth, gameProps->windowHeight, gameProps->targetFPS,
                         gameProps->initialScenePath, gameProps->areCollidersVisible);
}

// Node
PyObject* cre_py_api_node_new(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* classPath;
    char* className;
    char* nodeType;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sss", crePyApiNodeNewKWList, &classPath, &className, &nodeType)) {
        const Entity newEntity = cre_ec_system_create_entity_uid();

        // Setup script component first
        ScriptComponent* scriptComponent = script_component_create(classPath, className);
        scriptComponent->contextType = ScriptContextType_PYTHON;
        component_manager_set_component(newEntity, ComponentDataIndex_SCRIPT, scriptComponent);
        // Call create instance on script context
        // TODO: Not a big fan of updating the scripting system signature this way, but I guess it will suffice for now...
        cre_ec_system_update_entity_signature_with_systems(newEntity);
        PyObject* entityInstance = cre_py_get_script_instance(newEntity);
        SE_ASSERT_FMT(entityInstance != NULL, "Entity instance '%d' is NULL!", newEntity);

        NodeComponent* nodeComponent = node_component_create();
        strcpy(nodeComponent->name, nodeType);
        nodeComponent->type = node_get_base_type(nodeType);
        SE_ASSERT_FMT(nodeComponent->type != NodeBaseType_INVALID, "Node '%s' has an invalid node type '%s'", nodeType, nodeType);
        component_manager_set_component(newEntity, ComponentDataIndex_NODE, nodeComponent);

        const NodeBaseInheritanceType inheritanceType = node_get_type_inheritance(nodeComponent->type);

        if ((NodeBaseInheritanceType_NODE2D & inheritanceType) == NodeBaseInheritanceType_NODE2D) {
            Transform2DComponent* transform2DComponent = transform2d_component_create();
            component_manager_set_component(newEntity, ComponentDataIndex_TRANSFORM_2D, transform2d_component_create());
        }
        if ((NodeBaseInheritanceType_SPRITE & inheritanceType) == NodeBaseInheritanceType_SPRITE) {
            component_manager_set_component(newEntity, ComponentDataIndex_SPRITE, sprite_component_create());
        }
        if ((NodeBaseInheritanceType_ANIMATED_SPRITE & inheritanceType) == NodeBaseInheritanceType_ANIMATED_SPRITE) {
            component_manager_set_component(newEntity, ComponentDataIndex_ANIMATED_SPRITE, animated_sprite_component_create());
        }
        if ((NodeBaseInheritanceType_TEXT_LABEL & inheritanceType) == NodeBaseInheritanceType_TEXT_LABEL) {
            component_manager_set_component(newEntity, ComponentDataIndex_TEXT_LABEL, text_label_component_create());
        }
        if ((NodeBaseInheritanceType_COLLIDER2D & inheritanceType) == NodeBaseInheritanceType_COLLIDER2D) {
            component_manager_set_component(newEntity, ComponentDataIndex_COLLIDER_2D, collider2d_component_create());
        }
        if ((NodeBaseInheritanceType_COLOR_RECT & inheritanceType) == NodeBaseInheritanceType_COLOR_RECT) {
            component_manager_set_component(newEntity, ComponentDataIndex_COLOR_RECT, color_rect_component_create());
        }
        if ((NodeBaseInheritanceType_PARALLAX & inheritanceType) == NodeBaseInheritanceType_PARALLAX) {
            component_manager_set_component(newEntity, ComponentDataIndex_PARALLAX, parallax_component_create());
        }

        Py_IncRef(entityInstance);
        return Py_BuildValue("O", entityInstance);
    }
    return NULL;
}

PyObject* cre_py_api_node_queue_deletion(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        SceneTreeNode* node = cre_scene_manager_get_entity_tree_node(entity);
        cre_queue_destroy_tree_node_entity_all(node);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_add_child(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity parentEntity;
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ii", crePyApiNodeAddChildKWList, &parentEntity, &entity)) {
        SceneTreeNode* parentNode = cre_scene_manager_get_entity_tree_node(parentEntity);
        SceneTreeNode* node = cre_scene_tree_create_tree_node(entity, parentNode);
        if (parentNode != NULL) {
            parentNode->children[parentNode->childCount++] = node;
        }

        cre_ec_system_update_entity_signature_with_systems(entity);
        cre_scene_manager_queue_entity_for_creation(node);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_utils_get_entity_instance(Entity entity) {
#define TYPE_BUFFER_SIZE 32
    PyObject* scriptInstance = cre_py_get_script_instance(entity);
    if (scriptInstance != NULL) {
        Py_IncRef(scriptInstance);
        return scriptInstance;
    }
    // If script instance doesn't exist, create a proxy to be used on the scripting side
    char typeBuffer[TYPE_BUFFER_SIZE];
    NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component(entity, ComponentDataIndex_NODE);
    strcpy(typeBuffer, node_get_base_type_string(nodeComponent->type));
    return Py_BuildValue("(is)", entity, typeBuffer);
#undef TYPE_BUFFER_SIZE
}

PyObject* cre_py_api_node_get_child(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity parentEntity;
    char* childName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiNodeGetChildKWList, &parentEntity, &childName)) {
        Entity childEntity = cre_scene_manager_get_entity_child_by_name(parentEntity, childName);
        if (childEntity == NULL_ENTITY) {
            se_logger_warn("Failed to get child node from parent entity '%d' with the name '%s'", parentEntity,
                           childName);
            Py_RETURN_NONE;
        }
        return cre_py_utils_get_entity_instance(childEntity);
    }
    return NULL;
}

PyObject* cre_py_api_node_get_children(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity parentEntity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &parentEntity)) {
        const SceneTreeNode* parentTreeNode = cre_scene_manager_get_entity_tree_node(parentEntity);
        PyObject* pyChildList = PyList_New(0);
        for (size_t i = 0; i < parentTreeNode->childCount; i++) {
            const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
            PyObject* childNode = cre_py_utils_get_entity_instance(childTreeNode->entity);
            if (PyList_Append(pyChildList, childNode) == -1) {
                se_logger_error("Failed to append entity '%d' to '%d' children list!", parentEntity,
                                childTreeNode->entity);
                PyErr_Print();
            }
        }
        return pyChildList;
    }
    return NULL;
}

PyObject* cre_py_api_node_get_parent(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        SceneTreeNode* treeNode = cre_scene_manager_get_entity_tree_node(entity);
        if (treeNode->parent == NULL) {
            Py_RETURN_NONE;
        }
        return cre_py_utils_get_entity_instance(treeNode->parent->entity);
    }
    return NULL;
}

PyObject* cre_py_api_node_get_name(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component(entity, ComponentDataIndex_NODE);
        return Py_BuildValue("s", nodeComponent->name);
    }
    return NULL;
}

// TODO: Move function to another place...
void cre_invalidate_time_dilation_nodes_with_children(Entity entity) {
    NodeComponent* nodeComponent = component_manager_get_component(entity, ComponentDataIndex_NODE);
    nodeComponent->timeDilation.cacheInvalid = true;
    SceneTreeNode* sceneTreeNode = cre_scene_manager_get_entity_tree_node(entity);
    for (size_t i = 0; i < sceneTreeNode->childCount; i++) {
        cre_invalidate_time_dilation_nodes_with_children(sceneTreeNode->children[i]->entity);
    }
}

PyObject* cre_py_api_node_set_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float timeDilation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", crePyApiNodeSetTimeDilationKWList, &entity, &timeDilation)) {
        NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component(entity, ComponentDataIndex_NODE);
        nodeComponent->timeDilation.value = timeDilation;
        cre_invalidate_time_dilation_nodes_with_children(entity);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_get_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component(entity, ComponentDataIndex_NODE);
        return Py_BuildValue("f", nodeComponent->timeDilation.value);
    }
    return NULL;
}

PyObject* cre_py_api_node_get_full_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        return Py_BuildValue("f", cre_scene_manager_get_node_full_time_dilation(entity));
    }
    return NULL;
}

// Event related stuff
void py_api_node_event_callback(void* observerData, NodeEventNotifyPayload* notifyPayload) {
    PyObject* pyCallbackFunc = (PyObject*) observerData;
    PyObject* pyEventArgs = (PyObject*) notifyPayload->data;
    SE_ASSERT(pyCallbackFunc != NULL);
    SE_ASSERT(pyEventArgs != NULL);

    PyObject* listenerFuncArg = Py_BuildValue("(O)", pyEventArgs);
    PyObject_CallObject(pyCallbackFunc, listenerFuncArg);
}

void py_api_node_event_data_delete_callback(void* data) {
    PyObject* pyCallbackFunc = (PyObject*) data;
    if (Py_IsTrue(pyCallbackFunc)) {
        Py_DecRef(pyCallbackFunc);
    }
}

PyObject* cre_py_api_node_create_event(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* eventId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiNodeCreateEventKWList, &entity, &eventId)) {
        node_event_create_event(entity, eventId);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_subscribe_to_event(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* eventId;
    Entity scopedEntity;
    PyObject* pCallbackFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isiO", crePyApiNodeSubscribeToEventKWList, &entity, &eventId, &scopedEntity, &pCallbackFunc)) {
        // Decreases ref in event data delete callback
        Py_IncRef(pCallbackFunc);
        node_event_subscribe_to_event(entity, eventId, scopedEntity, py_api_node_event_callback, pCallbackFunc, py_api_node_event_data_delete_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node_broadcast_event(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* eventId;
    PyObject* broadcastArgs;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isO", crePyApiNodeBroadcastEventKWList, &entity, &eventId, &broadcastArgs)) {
        node_event_notify_observers(entity, eventId, &(NodeEventNotifyPayload) {
            .data = broadcastArgs
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

// Node2D
PyObject* cre_py_api_node2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        py_api_update_entity_local_position(entity, &(Vector2) {
            x, y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        py_api_update_entity_local_position(entity, &(Vector2) {
            x + transformComp->localTransform.position.x, y + transformComp->localTransform.position.y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("(ff)", transformComp->localTransform.position.x, transformComp->localTransform.position.y);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_global_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        TransformModel2D* globalTransform = cre_scene_manager_get_scene_node_global_transform(entity, transformComp);
        return Py_BuildValue("(ff)", globalTransform->position.x, globalTransform->position.y);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_set_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        py_api_update_entity_local_scale(entity, &(Vector2) {
            x, y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_add_to_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        py_api_update_entity_local_scale(entity, &(Vector2) {
            x + transformComp->localTransform.scale.x, y + transformComp->localTransform.scale.y
        });
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("(ff)", transformComp->localTransform.scale.x, transformComp->localTransform.scale.y);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_set_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float rotation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", crePyApiNode2DSetRotationKWList, &entity, &rotation)) {
        py_api_update_entity_local_rotation(entity, rotation);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_add_to_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float rotation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", crePyApiNode2DSetRotationKWList, &entity, &rotation)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        py_api_update_entity_local_rotation(entity, rotation + transformComp->localTransform.rotation);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("f", transformComp->localTransform.rotation);
    }
    return NULL;
}

PyObject* cre_py_api_node2D_set_z_index(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    int zIndex;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ii", crePyApiNode2DSetZIndexKWList, &entity, &zIndex)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->zIndex = zIndex;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_node2D_get_z_index(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("i", transformComp->zIndex);
    }
    return NULL;
}

// Sprite
PyObject* cre_py_api_sprite_set_texture(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* filePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiSpriteSetTextureKWList, &entity, &filePath)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        SE_ASSERT_FMT(se_asset_manager_has_texture(filePath), "Doesn't have texture with file path '%s'", filePath);
        spriteComponent->texture = se_asset_manager_get_texture(filePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_texture(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        return Py_BuildValue("(sssss)", spriteComponent->texture->fileName, "clamp_to_border", "clamp_to_border", "nearest", "nearest");
    }
    return NULL;
}

PyObject* cre_py_api_sprite_set_draw_source(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iffff", crePyApiGenericSetEntityRectKWList, &entity, &x, &y, &w, &h)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        spriteComponent->drawSource.x = x;
        spriteComponent->drawSource.y = y;
        spriteComponent->drawSource.w = w;
        spriteComponent->drawSource.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_sprite_get_draw_source(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        return Py_BuildValue("(ffff)", spriteComponent->drawSource.x, spriteComponent->drawSource.y, spriteComponent->drawSource.w, spriteComponent->drawSource.h);
    }
    return NULL;
}

// Animated Sprite
PyObject* cre_py_api_animated_sprite_play(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* animationName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiAnimatedSpriteSetAnimationKWList, &entity, &animationName)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) component_manager_get_component(entity, ComponentDataIndex_ANIMATED_SPRITE);
        const bool success = animated_sprite_component_set_animation(animatedSpriteComponent, animationName);
        animatedSpriteComponent->isPlaying = true;
        if (success) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_stop(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiGenericGetEntityKWList, &entity)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) component_manager_get_component(entity, ComponentDataIndex_ANIMATED_SPRITE);
        animatedSpriteComponent->isPlaying = false;
    }
    return NULL;
}

PyObject* cre_py_api_animated_sprite_add_animation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* name;
    int speed;
    bool loops;
    PyObject* framesList;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "isibO", crePyApiAnimatedSpriteAddAnimationKWList, &entity, &name, &speed, &loops, &framesList)) {
        Py_RETURN_NONE;
    }
    return NULL;
}

// Text Label
PyObject* cre_py_api_text_label_set_text(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* text;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiTextLabelSetTextKWList, &entity, &text)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        strcpy(textLabelComponent->text, text);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_text_label_get_text(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        return Py_BuildValue("s", textLabelComponent->text);
    }
    return NULL;
}

PyObject* cre_py_api_text_label_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iffff", crePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        textLabelComponent->color = se_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_text_label_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        const int red = (int) (textLabelComponent->color.r * 255.0f);
        const int green = (int) (textLabelComponent->color.r * 255.0f);
        const int blue = (int) (textLabelComponent->color.r * 255.0f);
        const int alpha = (int) (textLabelComponent->color.r * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

PyObject* cre_py_api_text_label_set_font_uid(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* uid;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", crePyApiTextLabelSetFontUIDKWList, &entity, &uid)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        if (se_asset_manager_has_font(uid)) {
            textLabelComponent->font = se_asset_manager_get_font(uid);
        } else {
            se_logger_error("Failed to set font to '%s' as it doesn't exist in the asset manager!", uid);
        }
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_text_label_get_font_uid(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        return Py_BuildValue("s", "default"); // TODO: Do want this?
    }
    return NULL;
}

// Collider2D
PyObject* cre_py_api_collider2D_set_extents(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiGenericSetEntitySize2DKWList, &entity, &w, &h)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent*) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        collider2DComponent->extents.w = w;
        collider2DComponent->extents.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_collider2D_get_extents(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const Collider2DComponent* collider2DComponent = (Collider2DComponent *) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        return Py_BuildValue("(ff)", collider2DComponent->extents.w, collider2DComponent->extents.h);
    }
    return NULL;
}

PyObject* cre_py_api_collider2D_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", crePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent*) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        collider2DComponent->color = se_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_collider2D_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent *) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        const int red = (int) (collider2DComponent->color.r * 255.0f);
        const int green = (int) (collider2DComponent->color.g * 255.0f);
        const int blue = (int) (collider2DComponent->color.b * 255.0f);
        const int alpha = (int) (collider2DComponent->color.a * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

// ColorRect
PyObject* cre_py_api_color_rect_set_size(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", crePyApiGenericSetEntitySize2DKWList, &entity, &w, &h)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent*) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        colorSquareComponent->size.w = w;
        colorSquareComponent->size.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_color_rect_get_size(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        const ColorRectComponent* colorSquareComponent = (ColorRectComponent*) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        return Py_BuildValue("(ff)", colorSquareComponent->size.w, colorSquareComponent->size.h);
    }
    return NULL;
}

PyObject* cre_py_api_color_rect_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", crePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent *) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        colorSquareComponent->color = se_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_color_rect_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent*) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        const int red = (int) (colorSquareComponent->color.r * 255.0f);
        const int green = (int) (colorSquareComponent->color.g * 255.0f);
        const int blue = (int) (colorSquareComponent->color.b * 255.0f);
        const int alpha = (int) (colorSquareComponent->color.a * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

// Network
PyObject* cre_py_api_network_is_server(PyObject* self, PyObject* args) {
    if (se_network_is_server()) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Server
PyObject* cre_py_api_server_start(PyObject* self, PyObject* args, PyObject* kwargs) {
    int port;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiServerStartKWList, &port)) {
        se_udp_server_initialize(port, cre_ec_system_network_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_server_stop(PyObject* self, PyObject* args) {
    se_udp_server_finalize();
    Py_RETURN_NONE;
}

PyObject* cre_py_api_server_send(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* message;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiNetworkSendMessageKWList, &message)) {
        se_udp_server_send_message(message);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_server_subscribe(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* signalId;
    Entity listenerNode;
    PyObject* listenerFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siO", crePyApiNetworkSubscribeKWList, &signalId, &listenerNode, &listenerFunc)) {
        SE_ASSERT(PyObject_IsTrue(listenerFunc));
        const CREScriptContext* scriptContext = cre_py_get_script_context();
        SE_ASSERT(scriptContext != NULL && scriptContext->on_entity_subscribe_to_network_callback != NULL);
        scriptContext->on_entity_subscribe_to_network_callback(listenerNode, listenerFunc, signalId);

        Py_DECREF(listenerFunc);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Client
PyObject* cre_py_api_client_start(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* host;
    int port;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "si", crePyApiClientStartKWList, &host, &port)) {
        se_udp_client_initialize(host, port, cre_ec_system_network_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_client_stop(PyObject* self, PyObject* args) {
    se_udp_client_finalize();
    Py_RETURN_NONE;
}

PyObject* cre_py_api_client_send(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* message;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", crePyApiNetworkSendMessageKWList, &message)) {
        se_udp_client_send_message(message);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* cre_py_api_client_subscribe(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* signalId;
    Entity listenerNode;
    PyObject* listenerFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siO", crePyApiNetworkSubscribeKWList, &signalId, &listenerNode, &listenerFunc)) {
        SE_ASSERT(PyObject_IsTrue(listenerFunc));
        const CREScriptContext* scriptContext = cre_py_get_script_context();
        SE_ASSERT(scriptContext != NULL && scriptContext->on_entity_subscribe_to_network_callback != NULL);
        scriptContext->on_entity_subscribe_to_network_callback(listenerNode, listenerFunc, signalId);

        Py_DECREF(listenerFunc);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Collision Handler
PyObject* cre_py_api_collision_handler_process_collisions(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", crePyApiGenericGetEntityKWList, &entity)) {
        PyObject* pyCollidedEntityList = PyList_New(0);
        CollisionResult collisionResult = cre_collision_process_entity_collisions(entity);
        for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
            const Entity collidedEntity = collisionResult.collidedEntities[i];
            PyObject* collidedNode = cre_py_utils_get_entity_instance(collidedEntity);
            if (PyList_Append(pyCollidedEntityList, collidedNode) == -1) {
                se_logger_error("Failed to append collided entity '%d' to collision list!", collidedEntity);
                PyErr_Print();
            }
        }
        return Py_BuildValue("O", pyCollidedEntityList);
    }
    return NULL;
}

PyObject* cre_py_api_collision_handler_process_mouse_collisions(PyObject* self, PyObject* args, PyObject* kwargs) {
    float positionOffsetX;
    float positionOffsetY;
    float collisionSizeW;
    float collisionSizeH;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ffff", crePyApiCollisionHandlerProcessMouseCollisionsKWList, &positionOffsetX, &positionOffsetY, &collisionSizeW, &collisionSizeH)) {
        PyObject* pyCollidedEntityList = PyList_New(0);
        // TODO: Transform mouse screen position into world position.
        SEMouse* globalMouse = se_mouse_get();
        Vector2 positionOffset = { positionOffsetX, positionOffsetY };
        const Vector2 mouseWorldPos = py_api_mouse_get_global_position(globalMouse, &positionOffset);
        Rect2 collisionRect = { mouseWorldPos.x, mouseWorldPos.y, collisionSizeW, collisionSizeH };
        CollisionResult collisionResult = cre_collision_process_mouse_collisions(&collisionRect);
        for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
            const Entity collidedEntity = collisionResult.collidedEntities[i];
            PyObject* collidedNode = cre_py_utils_get_entity_instance(collidedEntity);
            if (PyList_Append(pyCollidedEntityList, collidedNode) == -1) {
                se_logger_error("Failed to append mouse collided entity '%d' to collision list!", collidedEntity);
                PyErr_Print();
            }
        }
        return Py_BuildValue("O", pyCollidedEntityList);
    }
    return NULL;
}

// Game Config
PyObject* cre_py_api_game_config_save(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* path;
    char* dataJson;
    char* encryptionKey;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sss", crePyApiGameConfigSaveKWList, &path, &dataJson, &encryptionKey)) {
        CREGameProperties* gameProps = cre_game_props_get();
        char* validGameTitle = se_strdup(gameProps->gameTitle);
        se_str_to_lower_and_underscore_whitespace(validGameTitle);
        char* fullSavePath = se_fs_get_user_save_path("crescent", validGameTitle, path);
        const bool success = se_fs_write_to_file(fullSavePath, dataJson);
        SE_MEM_FREE(validGameTitle);
        SE_MEM_FREE(fullSavePath);
        if (success) {
            Py_RETURN_TRUE;
        }
    }
    Py_RETURN_FALSE;
}

PyObject* cre_py_api_game_config_load(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* path;
    char* encryptionKey;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ss", crePyApiGameConfigLoadKWList, &path, &encryptionKey)) {
        CREGameProperties* gameProps = cre_game_props_get();
        char* validGameTitle = se_strdup(gameProps->gameTitle);
        se_str_to_lower_and_underscore_whitespace(validGameTitle);
        char* fullSavePath = se_fs_get_user_save_path("crescent", validGameTitle, path);
        char* fileContents = se_fs_read_file_contents(fullSavePath, NULL);
        PyObject* returnValue =  Py_BuildValue("s", fileContents);
        SE_MEM_FREE(validGameTitle);
        SE_MEM_FREE(fullSavePath);
        SE_MEM_FREE(fileContents);
        return returnValue;
    }
    return Py_BuildValue("s", "{}");
}
