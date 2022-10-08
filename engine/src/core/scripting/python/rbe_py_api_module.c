#include "rbe_py_api_module.h"

#include <string.h>

#include "py_cache.h"
#include "py_script_context.h"
#include "../../engine_context.h"
#include "../../asset_manager.h"
#include "../../input/input.h"
#include "../../audio/audio_manager.h"
#include "../../scripting/script_context.h"
#include "../../physics/collision/collision.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../../ecs/ecs_manager.h"
#include "../../ecs/system/ec_system.h"
#include "../../ecs/component/animated_sprite_component.h"
#include "../../ecs/component/collider2d_component.h"
#include "../../ecs/component/color_rect_component.h"
#include "../../ecs/component/script_component.h"
#include "../../ecs/component/sprite_component.h"
#include "../../ecs/component/text_label_component.h"
#include "../../ecs/component/node_component.h"
#include "../../scene/scene_manager.h"
#include "../../networking/rbe_network.h"
#include "../../utils/rbe_assert.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

// TODO: Clean up strdups

//--- Py Utils ---//
PyObject* rbe_py_utils_get_entity_instance(Entity entity);

//--- RBE PY API ---//

// Engine
PyObject* rbe_py_api_engine_exit(PyObject* self, PyObject* args, PyObject* kwargs) {
    int exitCode;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiEngineExitKWList, &exitCode)) {
        RBEEngineContext* engineContext = rbe_engine_context_get();
        engineContext->isRunning = false;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_engine_set_target_fps(PyObject* self, PyObject* args, PyObject* kwargs) {
    int targetFPS;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiEngineSetTargetFPSKWList, &targetFPS)) {
        RBEEngineContext* engineContext = rbe_engine_context_get();
        engineContext->targetFPS = targetFPS;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_engine_get_target_fps(PyObject* self, PyObject* args) {
    const RBEEngineContext* engineContext = rbe_engine_context_get();
    return Py_BuildValue("(f)", engineContext->targetFPS);
}

PyObject* rbe_py_api_engine_get_average_fps(PyObject* self, PyObject* args) {
    const RBEEngineContext* engineContext = rbe_engine_context_get();
    return Py_BuildValue("(f)", engineContext->averageFPS);
}

PyObject* rbe_py_api_engine_set_fps_display_enabled(PyObject* self, PyObject* args, PyObject* kwargs) {
    bool isEnabled;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "b", rbePyApiGenericEnabledKWList, &isEnabled)) {
        rbe_ecs_manager_enable_fps_display_entity(isEnabled);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* PyInit_rbe_py_API(void) {
    return PyModule_Create(&rbePyAPIModDef);
}

// Input
PyObject* rbe_py_api_input_add_action(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    char* actionValue;
    int deviceId;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ssi", rbePyApiInputAddActionKWList, &actionName, &actionValue, &deviceId)) {
        cre_input_add_action_value(actionName, actionValue, deviceId);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_input_is_action_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiInputActionInputCheckKWList, &actionName)) {
        if (cre_input_is_action_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* rbe_py_api_input_is_action_just_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiInputActionInputCheckKWList, &actionName)) {
        if (cre_input_is_action_just_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* rbe_py_api_input_is_action_just_released(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiInputActionInputCheckKWList, &actionName)) {
        if (cre_input_is_action_just_released(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

// Camera
PyObject* rbe_py_api_camera2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", rbePyApiGenericXYKWList, &x, &y)) {
        RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
        camera2D->viewport.x = x;
        camera2D->viewport.y = y;
        rbe_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", rbePyApiGenericXYKWList, &x, &y)) {
        RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
        camera2D->viewport.x += x;
        camera2D->viewport.y += y;
        rbe_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_get_position(PyObject* self, PyObject* args) {
    RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->viewport.x, camera2D->viewport.y);
}

PyObject* rbe_py_api_camera2D_set_offset(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", rbePyApiGenericXYKWList, &x, &y)) {
        RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
        camera2D->offset.x = x;
        camera2D->offset.y = y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_add_to_offset(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", rbePyApiGenericXYKWList, &x, &y)) {
        RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
        camera2D->offset.x += x;
        camera2D->offset.y += y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_get_offset(PyObject* self, PyObject* args) {
    RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->offset.x, camera2D->offset.y);
}

PyObject* rbe_py_api_camera2D_set_zoom(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", rbePyApiGenericXYKWList, &x, &y)) {
        RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
        camera2D->zoom.x = x;
        camera2D->zoom.y = y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_add_to_zoom(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ff", rbePyApiGenericXYKWList, &x, &y)) {
        RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
        camera2D->zoom.x += x;
        camera2D->zoom.y += y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_get_zoom(PyObject* self, PyObject* args) {
    RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    return Py_BuildValue("(ff)", camera2D->zoom.x, camera2D->zoom.y);
}

// Scene Tree
PyObject* rbe_py_api_scene_tree_change_scene(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* scenePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiGenericPathKWList, &scenePath)) {
        rbe_scene_manager_queue_scene_change(scenePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_set_boundary(PyObject* self, PyObject* args, PyObject* kwargs) {
    float x;
    float y;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ffff", rbePyApiGenericXYWHKWList, &x, &y, &w, &h)) {
        RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
        camera2D->boundary.x = x;
        camera2D->boundary.y = y;
        camera2D->boundary.w = w;
        camera2D->boundary.h = h;
        rbe_camera2d_clamp_viewport_to_boundary(camera2D);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_camera2D_get_boundary(PyObject* self, PyObject* args) {
    RBECamera2D* camera2D = rbe_camera_manager_get_current_camera();
    return Py_BuildValue("(ffff)", camera2D->boundary.x, camera2D->boundary.y, camera2D->boundary.w, camera2D->boundary.h);
}

// Audio Manager
PyObject* rbe_py_api_audio_manager_play_sound(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    bool loops;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sb", rbePyApiAudioManagerPlaySoundKWList, &audioPath, &loops)) {
        rbe_audio_manager_play_sound(audioPath, loops);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_audio_manager_stop_sound(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* audioPath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiGenericPathKWList, &audioPath)) {
        rbe_audio_manager_stop_sound(audioPath);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Node
PyObject* rbe_py_api_node_new(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* classPath;
    char* className;
    char* nodeType;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "sss", rbePyApiNodeNewKWList, &classPath, &className, &nodeType)) {
        const Entity newEntity = rbe_ec_system_create_entity();

        // Setup script component first
        ScriptComponent* scriptComponent = script_component_create(classPath, className);
        scriptComponent->contextType = ScriptContextType_PYTHON;
        component_manager_set_component(newEntity, ComponentDataIndex_SCRIPT, scriptComponent);
        // Call create instance on script context
        // TODO: Not a big fan of updating the scripting system signature this way, but I guess it will suffice for now...
        rbe_ec_system_update_entity_signature_with_systems(newEntity);
        PyObject* entityInstance = rbe_py_get_script_instance(newEntity);
        RBE_ASSERT_FMT(entityInstance != NULL, "Entity instance '%d' is NULL!", newEntity);

        NodeComponent* nodeComponent = node_component_create();
        strcpy(nodeComponent->name, nodeType);
        nodeComponent->type = node_get_base_type(nodeType);
        RBE_ASSERT_FMT(nodeComponent->type != NodeBaseType_INVALID, "Node '%s' has an invalid node type '%s'", nodeType, nodeType);
        component_manager_set_component(newEntity, ComponentDataIndex_NODE, nodeComponent);

        const NodeBaseInheritanceType inheritanceType = node_get_type_inheritance(nodeComponent->type);

        if ((NodeBaseInheritanceType_NODE2D & inheritanceType) == NodeBaseInheritanceType_NODE2D) {
            Transform2DComponent* transform2DComponent = transform2d_component_create();
            component_manager_set_component(newEntity, ComponentDataIndex_TRANSFORM_2D, transform2DComponent);
        }

        if ((NodeBaseInheritanceType_SPRITE & inheritanceType) == NodeBaseInheritanceType_SPRITE) {
            SpriteComponent* spriteComponent = sprite_component_create();
            component_manager_set_component(newEntity, ComponentDataIndex_SPRITE, spriteComponent);
        }

        if ((NodeBaseInheritanceType_ANIMATED_SPRITE & inheritanceType) == NodeBaseInheritanceType_ANIMATED_SPRITE) {
            AnimatedSpriteComponent* animatedSpriteComponent = animated_sprite_component_create();
            component_manager_set_component(newEntity, ComponentDataIndex_ANIMATED_SPRITE, animatedSpriteComponent);
        }

        if ((NodeBaseInheritanceType_TEXT_LABEL & inheritanceType) == NodeBaseInheritanceType_TEXT_LABEL) {
            TextLabelComponent* textLabelComponent = text_label_component_create();
            component_manager_set_component(newEntity, ComponentDataIndex_TEXT_LABEL, textLabelComponent);
        }

        if ((NodeBaseInheritanceType_COLLIDER2D & inheritanceType) == NodeBaseInheritanceType_COLLIDER2D) {
            Collider2DComponent* collider2DComponent = collider2d_component_create();
            component_manager_set_component(newEntity, ComponentDataIndex_COLLIDER_2D, collider2DComponent);
        }

        if ((NodeBaseInheritanceType_COLOR_RECT & inheritanceType) == NodeBaseInheritanceType_COLOR_RECT) {
            ColorRectComponent* colorSquareComponent = color_rect_component_create();
            component_manager_set_component(newEntity, ComponentDataIndex_COLOR_RECT, colorSquareComponent);
        }

        Py_IncRef(entityInstance);
        return Py_BuildValue("O", entityInstance);
    }
    return NULL;
}

PyObject* rbe_py_api_node_queue_deletion(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        SceneTreeNode* node = rbe_scene_manager_get_entity_tree_node(entity);
        rbe_queue_destroy_tree_node_entity_all(node);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node_add_child(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity parentEntity;
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ii", rbePyApiNodeAddChildKWList, &parentEntity, &entity)) {
        SceneTreeNode* parentNode = rbe_scene_manager_get_entity_tree_node(parentEntity);
        SceneTreeNode* node = rbe_scene_tree_create_tree_node(entity, parentNode);
        if (parentNode != NULL) {
            parentNode->children[parentNode->childCount++] = node;
        }

        rbe_ec_system_update_entity_signature_with_systems(entity);
        rbe_scene_manager_queue_entity_for_creation(node);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_utils_get_entity_instance(Entity entity) {
#define TYPE_BUFFER_SIZE 32
    PyObject* scriptInstance = rbe_py_get_script_instance(entity);
    if (scriptInstance != NULL) {
        Py_IncRef(scriptInstance);
        return scriptInstance;
    }
    char typeBuffer[TYPE_BUFFER_SIZE];
    NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component(entity, ComponentDataIndex_NODE);
    strcpy(typeBuffer, node_get_base_type_string(nodeComponent->type));

    return Py_BuildValue("(is)", entity, typeBuffer);
#undef TYPE_BUFFER_SIZE
}

PyObject* rbe_py_api_node_get_child(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity parentEntity;
    char* childName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", rbePyApiNodeGetChildKWList, &parentEntity, &childName)) {
        Entity childEntity = rbe_scene_manager_get_entity_child_by_name(parentEntity, childName);
        if (childEntity == NULL_ENTITY) {
            rbe_logger_warn("Failed to get child node from parent entity '%d' with the name '%s'", parentEntity, childName);
            Py_RETURN_NONE;
        }
        return rbe_py_utils_get_entity_instance(childEntity);
    }
    return NULL;
}

PyObject* rbe_py_api_node_get_children(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity parentEntity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &parentEntity)) {
        const SceneTreeNode* parentTreeNode = rbe_scene_manager_get_entity_tree_node(parentEntity);
        PyObject* pyChildList = PyList_New(0);
        for (size_t i = 0; i < parentTreeNode->childCount; i++) {
            const SceneTreeNode* childTreeNode = parentTreeNode->children[i];
            PyObject* childNode = rbe_py_utils_get_entity_instance(childTreeNode->entity);
            if (PyList_Append(pyChildList, childNode) == -1) {
                rbe_logger_error("Failed to append entity '%d' to '%d' children list!", parentEntity, childTreeNode->entity);
                PyErr_Print();
            }
        }
        return pyChildList;
    }
    return NULL;
}

PyObject* rbe_py_api_node_get_parent(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        SceneTreeNode* treeNode = rbe_scene_manager_get_entity_tree_node(entity);
        if (treeNode->parent == NULL) {
            Py_RETURN_NONE;
        }
        return rbe_py_utils_get_entity_instance(treeNode->parent->entity);
    }
    return NULL;
}

// Node2D
PyObject* rbe_py_api_node2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->localTransform.position.x = x;
        transformComp->localTransform.position.y = y;
        transformComp->isGlobalTransformDirty = true;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->localTransform.position.x += x;
        transformComp->localTransform.position.y += y;
        transformComp->isGlobalTransformDirty = true;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_get_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("(ff)", transformComp->localTransform.position.x, transformComp->localTransform.position.y);
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_get_global_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        TransformModel2D* globalTransform = rbe_scene_manager_get_scene_node_global_transform(entity, transformComp);
        return Py_BuildValue("(ff)", globalTransform->position.x, globalTransform->position.y);
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_set_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->localTransform.scale.x = x;
        transformComp->localTransform.scale.y = y;
        transformComp->isGlobalTransformDirty = true;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_add_to_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiNode2DSetXYKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->localTransform.scale.x += x;
        transformComp->localTransform.scale.y += y;
        transformComp->isGlobalTransformDirty = true;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_get_scale(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("(ff)", transformComp->localTransform.scale.x, transformComp->localTransform.scale.y);
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_set_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float rotation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", rbePyApiNode2DSetRotationKWList, &entity, &rotation)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->localTransform.rotation = rotation;
        transformComp->isGlobalTransformDirty = true;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_add_to_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float rotation;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "if", rbePyApiNode2DSetRotationKWList, &entity, &rotation)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->localTransform.rotation += rotation;
        transformComp->isGlobalTransformDirty = true;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_get_rotation(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        const Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("f", transformComp->localTransform.rotation);
    }
    return NULL;
}

// Sprite
PyObject* rbe_py_api_sprite_set_texture(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* filePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", rbePyApiSpriteSetTextureKWList, &entity, &filePath)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        RBE_ASSERT_FMT(rbe_asset_manager_has_texture(filePath), "Doesn't have texture with file path '%s'", filePath);
        spriteComponent->texture = rbe_asset_manager_get_texture(filePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_sprite_get_texture(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        return Py_BuildValue("(sssss)", spriteComponent->texture->fileName, "clamp_to_border", "clamp_to_border", "nearest", "nearest");
    }
    return NULL;
}

PyObject* rbe_py_api_sprite_set_draw_source(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iffff", rbePyApiGenericSetEntityRectKWList, &entity, &x, &y, &w, &h)) {
        SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        spriteComponent->drawSource.x = x;
        spriteComponent->drawSource.y = y;
        spriteComponent->drawSource.w = w;
        spriteComponent->drawSource.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_sprite_get_draw_source(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        const SpriteComponent* spriteComponent = (SpriteComponent*) component_manager_get_component(entity, ComponentDataIndex_SPRITE);
        return Py_BuildValue("(ffff)", spriteComponent->drawSource.x, spriteComponent->drawSource.y, spriteComponent->drawSource.w, spriteComponent->drawSource.h);
    }
    return NULL;
}

// Animated Sprite
PyObject* rbe_py_api_animated_sprite_play(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* animationName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", rbePyApiAnimatedSpriteSetAnimationKWList, &entity, &animationName)) {
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

PyObject* rbe_py_api_animated_sprite_stop(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", rbePyApiGenericGetEntityKWList, &entity)) {
        AnimatedSpriteComponent* animatedSpriteComponent = (AnimatedSpriteComponent *) component_manager_get_component(entity, ComponentDataIndex_ANIMATED_SPRITE);
        animatedSpriteComponent->isPlaying = false;
    }
    return NULL;
}

// Text Label
PyObject* rbe_py_api_text_label_set_text(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    char* text;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "is", rbePyApiTextLabelSetTextKWList, &entity, &text)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        strcpy(textLabelComponent->text, text);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_text_label_get_text(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        return Py_BuildValue("s", textLabelComponent->text);
    }
    return NULL;
}

PyObject* rbe_py_api_text_label_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iffff", rbePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        textLabelComponent->color = rbe_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_text_label_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        TextLabelComponent* textLabelComponent = (TextLabelComponent*) component_manager_get_component(entity, ComponentDataIndex_TEXT_LABEL);
        const int red = (int) (textLabelComponent->color.r * 255.0f);
        const int green = (int) (textLabelComponent->color.r * 255.0f);
        const int blue = (int) (textLabelComponent->color.r * 255.0f);
        const int alpha = (int) (textLabelComponent->color.r * 255.0f);
        return Py_BuildValue("(iiii)", red, green, blue, alpha);
    }
    return NULL;
}

// Collider2D
PyObject* rbe_py_api_collider2D_set_extents(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiGenericSetEntitySize2DKWList, &entity, &w, &h)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent*) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        collider2DComponent->extents.w = w;
        collider2DComponent->extents.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_collider2D_get_extents(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        const Collider2DComponent* collider2DComponent = (Collider2DComponent *) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        return Py_BuildValue("(ff)", collider2DComponent->extents.w, collider2DComponent->extents.h);
    }
    return NULL;
}

PyObject* rbe_py_api_collider2D_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", rbePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        Collider2DComponent* collider2DComponent = (Collider2DComponent*) component_manager_get_component(entity, ComponentDataIndex_COLLIDER_2D);
        collider2DComponent->color = rbe_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_collider2D_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
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
PyObject* rbe_py_api_color_rect_set_size(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float w;
    float h;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiGenericSetEntitySize2DKWList, &entity, &w, &h)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent*) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        colorSquareComponent->size.w = w;
        colorSquareComponent->size.h = h;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_color_rect_get_size(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        const ColorRectComponent* colorSquareComponent = (ColorRectComponent*) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        return Py_BuildValue("(ff)", colorSquareComponent->size.w, colorSquareComponent->size.h);
    }
    return NULL;
}

PyObject* rbe_py_api_color_rect_set_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    int red;
    int green;
    int blue;
    int alpha;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iiiii", rbePyApiGenericSetEntityColorKWList, &entity, &red, &green, &blue, &alpha)) {
        ColorRectComponent* colorSquareComponent = (ColorRectComponent *) component_manager_get_component(entity, ComponentDataIndex_COLOR_RECT);
        colorSquareComponent->color = rbe_color_get_normalized_color(red, green, blue, alpha);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_color_rect_get_color(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
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
PyObject* rbe_py_api_network_is_server(PyObject* self, PyObject* args) {
    if (rbe_network_is_server()) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Server
PyObject* rbe_py_api_server_start(PyObject* self, PyObject* args, PyObject* kwargs) {
    int port;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiServerStartKWList, &port)) {
        rbe_udp_server_initialize(port, rbe_ec_system_network_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_server_stop(PyObject* self, PyObject* args) {
    rbe_udp_server_finalize();
    Py_RETURN_NONE;
}

PyObject* rbe_py_api_server_send(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* message;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiNetworkSendMessageKWList, &message)) {
        rbe_udp_server_send_message(message);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_server_subscribe(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* signalId;
    Entity listenerNode;
    PyObject* listenerFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siO", rbePyApiNetworkSubscribeKWList, &signalId, &listenerNode, &listenerFunc)) {
        RBE_ASSERT(PyObject_IsTrue(listenerFunc));
        const RBEScriptContext* scriptContext =  rbe_py_get_script_context();
        RBE_ASSERT(scriptContext != NULL && scriptContext->on_entity_subscribe_to_network_callback != NULL);
        scriptContext->on_entity_subscribe_to_network_callback(listenerNode, listenerFunc, signalId);

        Py_DECREF(listenerFunc);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Client
PyObject* rbe_py_api_client_start(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* host;
    int port;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "si", rbePyApiClientStartKWList, &host, &port)) {
        rbe_udp_client_initialize(host, port, rbe_ec_system_network_callback);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_client_stop(PyObject* self, PyObject* args) {
    rbe_udp_client_finalize();
    Py_RETURN_NONE;
}

PyObject* rbe_py_api_client_send(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* message;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiNetworkSendMessageKWList, &message)) {
        rbe_udp_client_send_message(message);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_client_subscribe(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* signalId;
    Entity listenerNode;
    PyObject* listenerFunc;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siO", rbePyApiNetworkSubscribeKWList, &signalId, &listenerNode, &listenerFunc)) {
        RBE_ASSERT(PyObject_IsTrue(listenerFunc));
        const RBEScriptContext* scriptContext =  rbe_py_get_script_context();
        RBE_ASSERT(scriptContext != NULL && scriptContext->on_entity_subscribe_to_network_callback != NULL);
        scriptContext->on_entity_subscribe_to_network_callback(listenerNode, listenerFunc, signalId);

        Py_DECREF(listenerFunc);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Collision Handler
PyObject* rbe_py_api_collision_handler_process_collisions(PyObject* self, PyObject* args, PyObject* kwargs) {
#define TYPE_BUFFER_SIZE 32
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        char typeBuffer[TYPE_BUFFER_SIZE];
        PyObject* pyCollidedEntityList = PyList_New(0);
        CollisionResult collisionResult = rbe_collision_process_entity_collisions(entity);
        for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
            const Entity collidedEntity = collisionResult.collidedEntities[i];
            NodeComponent* nodeComponent = (NodeComponent*) component_manager_get_component(collidedEntity, ComponentDataIndex_NODE);
            strcpy(typeBuffer, node_get_base_type_string(nodeComponent->type));
            if (PyList_Append(pyCollidedEntityList, Py_BuildValue("(is)", collidedEntity, typeBuffer)) == -1) {
                PyErr_Print();
            }
        }
        return Py_BuildValue("O", pyCollidedEntityList);
    }
    return NULL;
#undef TYPE_BUFFER_SIZE
}
