#include "rbe_py_api_module.h"

#include "py_cache.h"
#include "../../game_properties.h"
#include "../../engine_context.h"
#include "../../asset_manager.h"
#include "../../input/input.h"
#include "../../audio/audio_manager.h"
#include "../../scripting/python/py_helper.h"
#include "../../scene/scene_manager.h"
#include "../../ecs/system/ec_system.h"
#include "../../ecs/component/transform2d_component.h"
#include "../../ecs/component/sprite_component.h"
#include "../../ecs/component/script_component.h"
#include "../../utils/rbe_assert.h"

// TODO: Clean up strdups

// --- Node Utils --- //
void setup_scene_stage_nodes(SceneTreeNode* parent, PyObject* stageNodeList);
void setup_scene_component_node(Entity entity, PyObject* component);

// --- RBE PY API --- //

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
    RBEEngineContext* engineContext = rbe_engine_context_get();
    return Py_BuildValue("(f)", engineContext->targetFPS);
}

PyObject* rbe_py_api_engine_get_average_fps(PyObject* self, PyObject* args) {
    RBEEngineContext* engineContext = rbe_engine_context_get();
    return Py_BuildValue("(f)", engineContext->averageFPS);
}

// Configure
PyObject* rbe_py_api_configure_game(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* gameTitle;
    int windowWidth;
    int windowHeight;
    int resolutionWidth;
    int resolutionHeight;
    int targetFPS;
    char* initialScenePath;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "siiiiis", rbePyApiProjectConfigureKWList, &gameTitle, &windowWidth, &windowHeight, &resolutionWidth, &resolutionHeight, &targetFPS, &initialScenePath)) {
        RBEGameProperties* gameProperties = rbe_game_props_get();
        gameProperties->gameTitle = strdup(gameTitle);
        gameProperties->windowWidth = windowWidth;
        gameProperties->windowHeight = windowHeight;
        gameProperties->resolutionWidth = resolutionWidth;
        gameProperties->resolutionHeight = resolutionHeight;
        gameProperties->targetFPS = targetFPS;
        gameProperties->initialScenePath = strdup(initialScenePath);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_configure_assets(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* audioSourcesList;
    PyObject* texturesList;
    PyObject* fontsList;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "OOO", rbePyApiConfigureAssetsKWList, &audioSourcesList, &texturesList, &fontsList)) {
        RBE_ASSERT_FMT(PyList_Check(audioSourcesList), "Passed in audio source assets are not a python list, check python api implementation...");
        RBE_ASSERT_FMT(PyList_Check(texturesList), "Passed in texture assets are not a python list, check python api implementation...");
        RBE_ASSERT_FMT(PyList_Check(fontsList), "Passed in font assets are not a python list, check python api implementation...");

        RBEGameProperties* gameProperties = rbe_game_props_get();

        // TODO: Actually put assets into structures
        // Audio Sources
        rbe_logger_debug("audio_sources:");
        for (Py_ssize_t i = 0; i < PyList_Size(audioSourcesList); i++) {
            PyObject* pAudioSourceAsset = PyList_GetItem(audioSourcesList, i);
            RBE_ASSERT(pAudioSourceAsset != NULL);
            const char* filePath = phy_get_string_from_var(pAudioSourceAsset, "file_path");
            rbe_logger_debug("file_path = %s", filePath);
            RBEAssetAudioSource assetAudioSource = { .file_path = strdup(filePath) };
            gameProperties->audioSources[gameProperties->audioSourceCount++] = assetAudioSource;
            Py_DECREF(pAudioSourceAsset);
        }
        Py_DECREF(audioSourcesList);

        // Textures
        rbe_logger_debug("textures:");
        for (Py_ssize_t i = 0; i < PyList_Size(texturesList); i++) {
            PyObject* pTextureAsset = PyList_GetItem(texturesList, i);
            RBE_ASSERT(pTextureAsset != NULL);
            const char* filePath = phy_get_string_from_var(pTextureAsset, "file_path");
            const char* wrapS = phy_get_string_from_var(pTextureAsset, "wrap_s");
            const char* wrapT = phy_get_string_from_var(pTextureAsset, "wrap_t");
            const char* filterMin = phy_get_string_from_var(pTextureAsset, "filter_min");
            const char* filterMag = phy_get_string_from_var(pTextureAsset, "filter_mag");
            rbe_logger_debug("file_path = %s, wrap_s = %s, wrap_t = %s, filter_min = %s, filter_mag = %s",
                             filePath, wrapS, wrapT, filterMin, filterMag);
            RBEAssetTexture assetTexture = { .file_path = strdup(filePath) };
            gameProperties->textures[gameProperties->textureCount++] = assetTexture;
            Py_DECREF(pTextureAsset);
        }
        Py_DECREF(texturesList);

        // Fonts
        rbe_logger_debug("fonts:");
        for (Py_ssize_t i = 0; i < PyList_Size(fontsList); i++) {
            PyObject* pFontAsset = PyList_GetItem(fontsList, i);
            RBE_ASSERT(pFontAsset != NULL);
            const char* filePath = phy_get_string_from_var(pFontAsset, "file_path");
            const char* uid = phy_get_string_from_var(pFontAsset, "uid");
            const int size = phy_get_int_from_var(pFontAsset, "size");
            rbe_logger_debug("file_path = %s, uid = %s, size = %d", filePath, uid, size);
            RBEAssetFont assetFont = { .file_path = strdup(filePath), .uid = strdup(uid), .size = size };
            gameProperties->fonts[gameProperties->fontCount++] = assetFont;
            Py_DECREF(pFontAsset);
        }
        Py_DECREF(fontsList);

        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_configure_inputs(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* inputActionsList;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "O", rbePyApiConfigureInputsKWList, &inputActionsList)) {
        RBE_ASSERT_FMT(PyList_Check(inputActionsList), "Passed in input actions are not a python list, check python api implementation...");

        RBEGameProperties* gameProperties = rbe_game_props_get();

        rbe_logger_debug("input actions:");
        for (Py_ssize_t i = 0; i < PyList_Size(inputActionsList); i++) {
            PyObject* pInputAction = PyList_GetItem(inputActionsList, i);
            RBE_ASSERT(pInputAction != NULL);
            const char* actionName = phy_get_string_from_var(pInputAction, "name");
            rbe_logger_debug("name = %s", actionName);
            PyObject* valuesList = PyObject_GetAttrString(pInputAction, "values");
            RBE_ASSERT(valuesList != NULL);
            RBE_ASSERT_FMT(PyList_Check(valuesList), "Input action values for '%s' is not a list!  Check python api implementation.", actionName);
            Py_ssize_t valueListSize = PyList_Size(valuesList);
            RBEInputAction inputAction = { .name = strdup(actionName), .valueCount = (size_t) valueListSize };
            for (Py_ssize_t actionIndex = 0; actionIndex < valueListSize; actionIndex++) {
                PyObject* pActionValue = PyList_GetItem(valuesList, actionIndex);
                const char* actionValue = pyh_get_string_from_obj(pActionValue);
                rbe_logger_debug("action value = '%s'", actionValue);
                inputAction.values[actionIndex] = strdup(actionValue);
            }
            gameProperties->inputActions[gameProperties->inputActionCount++] = inputAction;
            Py_DECREF(pInputAction);
        }
        Py_DECREF(inputActionsList);
        Py_RETURN_NONE;
    }
    return NULL;
}

// Stage
PyObject* rbe_py_api_create_stage_nodes(PyObject* self, PyObject* args, PyObject* kwargs) {
    PyObject* stageNodeList;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "O", rbePyApiCreateStageNodesKWList, &stageNodeList)) {
        RBE_ASSERT_FMT(PyList_Check(stageNodeList), "Passed in stage nodes are not a python list, check python api implementation...");
        rbe_logger_debug("setup stage nodes:");
        setup_scene_stage_nodes(NULL_ENTITY, stageNodeList); // Assumes this is the root entity node for the scene
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* PyInit_rbe_py_API(void) {
    return PyModule_Create(&rbePyAPIModDef);
}

// --- Node Utils --- //

// TODO: Pass whatever references the parent node structure
void setup_scene_stage_nodes(SceneTreeNode* parent, PyObject* stageNodeList) {
    for (Py_ssize_t i = 0; i < PyList_Size(stageNodeList); i++) {
        SceneTreeNode* node = rbe_scene_tree_create_tree_node(rbe_ec_system_create_entity(), parent);
        // Set tree root if parent is absent
        if (node->parent == NULL) {
            rbe_scene_manager_set_active_scene_root(node);
        } else {
            parent->children[parent->childCount++] = node;
        }

        rbe_scene_manager_queue_entity_for_creation(node->entity); // May move in a different place TODO: Figure out...

        PyObject* pStageNode = PyList_GetItem(stageNodeList, i);
        const char* nodeName = phy_get_string_from_var(pStageNode, "name");
        const char* nodeType = phy_get_string_from_var(pStageNode, "type");
        // Process tags if tags var is a list
        PyObject* tagsListVar = PyObject_GetAttrString(pStageNode, "tags");
        if (PyList_Check(tagsListVar)) {
            for (Py_ssize_t tagIndex = 0; tagIndex < PyList_Size(tagsListVar); tagIndex++) {}
        }
        // TODO: Actually load external scene file here and updated variables
        PyObject* externalNodeSourceVar = PyObject_GetAttrString(pStageNode, "external_node_source");
        if (externalNodeSourceVar != Py_None) {
            const char* externalNodeSourcePath = phy_get_string_from_var(externalNodeSourceVar, "external_node_source");
        }
        // Components
        // Testing module getting stuff to store component classes
        PyObject* componentsListVar = PyObject_GetAttrString(pStageNode, "components");
        if (PyList_Check(componentsListVar)) {
            for (Py_ssize_t componentIndex = 0; componentIndex < PyList_Size(componentsListVar); componentIndex++) {
                PyObject* pComponent = PyList_GetItem(componentsListVar, componentIndex);
                RBE_ASSERT(pComponent != NULL);
                setup_scene_component_node(node->entity, pComponent);
                Py_DecRef(pComponent);
            }
        }
        // TODO: Do in a different step or having different functionality to add node to scene tree
        rbe_ec_system_update_entity_signature_with_systems(node->entity);
        // Children Nodes
        PyObject* childrenListVar = PyObject_GetAttrString(pStageNode, "children");
        if (PyList_Check(childrenListVar)) {
            // Recurse through children nodes
            setup_scene_stage_nodes(node, childrenListVar);
        }

        rbe_logger_debug("node_name = %s, node_type = %s", nodeName, nodeType);
        Py_DecRef(tagsListVar);
        Py_DecRef(externalNodeSourceVar);
        Py_DecRef(componentsListVar);
        Py_DecRef(childrenListVar);
        Py_DecRef(pStageNode);
    }
    Py_DecRef(stageNodeList);
}

void setup_scene_component_node(Entity entity, PyObject* component) {
    const char* className = Py_TYPE(component)->tp_name; // TODO: Should probably Py_DecRed()?
    if (strcmp(className, "Transform2DComponent") == 0) {
        rbe_logger_debug("Building transform 2d component");
        PyObject* pPosition = PyObject_GetAttrString(component, "position");
        const float positionX = phy_get_float_from_var(pPosition, "x");
        const float positionY = phy_get_float_from_var(pPosition, "y");
        PyObject* pScale = PyObject_GetAttrString(component, "scale");
        const float scaleX = phy_get_float_from_var(pScale, "x");
        const float scaleY = phy_get_float_from_var(pScale, "y");
        const float rotation = phy_get_float_from_var(component, "rotation");
        const int zIndex = phy_get_int_from_var(component, "z_index");
        const bool zIndexRelativeToParent = phy_get_bool_from_var(component, "z_index_relative_to_parent");
        const bool ignoreCamera = phy_get_bool_from_var(component, "ignore_camera");
        Transform2DComponent* transform2DComponent = transform2d_component_create();
        transform2DComponent->position.x = positionX;
        transform2DComponent->position.y = positionY;
        transform2DComponent->scale.x = scaleX;
        transform2DComponent->scale.y = scaleY;
        transform2DComponent->zIndex = zIndex;
        transform2DComponent->isZIndexRelativeToParent = zIndexRelativeToParent;
        transform2DComponent->ignoreCamera = ignoreCamera;
        component_manager_set_component(entity, ComponentDataIndex_TRANSFORM_2D, transform2DComponent);
        rbe_logger_debug("position: (%f, %f), scale: (%f, %f), rotation: %f, z_index: %d, z_index_relative: %d, ignore_camera: %d",
                         positionX, positionY, scaleX, scaleY, rotation, zIndex, zIndexRelativeToParent, ignoreCamera);
        Py_DecRef(pPosition);
        Py_DecRef(pScale);
    } else if (strcmp(className, "SpriteComponent") == 0) {
        rbe_logger_debug("Building sprite component");
        const char* texturePath = phy_get_string_from_var(component, "texture_path");
        PyObject* pDrawSource = PyObject_GetAttrString(component, "draw_source");
        const float drawSourceX = phy_get_float_from_var(pDrawSource, "x");
        const float drawSourceY = phy_get_float_from_var(pDrawSource, "y");
        const float drawSourceW = phy_get_float_from_var(pDrawSource, "w");
        const float drawSourceH = phy_get_float_from_var(pDrawSource, "h");
        const bool flipX = phy_get_bool_from_var(component, "flip_x");
        const bool flipY = phy_get_bool_from_var(component, "flip_y");
        PyObject* pModulate = PyObject_GetAttrString(component, "modulate");
        const int modulateR = phy_get_int_from_var(pModulate, "r");
        const int modulateG = phy_get_int_from_var(pModulate, "g");
        const int modulateB = phy_get_int_from_var(pModulate, "b");
        const int modulateA = phy_get_int_from_var(pModulate, "a");
        SpriteComponent* spriteComponent = sprite_component_create();
        spriteComponent->texture = rbe_asset_manager_get_texture(texturePath);
        RBE_ASSERT_FMT(spriteComponent->texture != NULL, "Unable to read texture path '%s'", texturePath);
        spriteComponent->drawSource.x = drawSourceX;
        spriteComponent->drawSource.y = drawSourceY;
        spriteComponent->drawSource.w = drawSourceW;
        spriteComponent->drawSource.h = drawSourceH;
        spriteComponent->flipX = flipX;
        spriteComponent->flipY = flipY;
        Color modulateColor = rbe_color_get_normalized_color(modulateR, modulateG, modulateB, modulateA);
        spriteComponent->modulate.r = modulateColor.r;
        spriteComponent->modulate.g = modulateColor.g;
        spriteComponent->modulate.b = modulateColor.b;
        spriteComponent->modulate.a = modulateColor.a;
        component_manager_set_component(entity, ComponentDataIndex_SPRITE, spriteComponent);
        rbe_logger_debug("texture_path = %s, draw_source = (%f, %f, %f, %f), flip_x: %d, flip_y: %d, modulate: (%d, %d, %d, %d)",
                         texturePath, drawSourceX, drawSourceY, drawSourceW, drawSourceH, flipX, flipY, modulateR, modulateG, modulateB, modulateA);
        Py_DecRef(pDrawSource);
        Py_DecRef(pModulate);
    } else if (strcmp(className, "ScriptComponent") == 0) {
        rbe_logger_debug("Building script component");
        const char* scriptClassPath = phy_get_string_from_var(component, "class_path");
        const char* scriptClassName = phy_get_string_from_var(component, "class_name");
        ScriptComponent* scriptComponent = script_component_create();
        scriptComponent->classPath = scriptClassPath;
        scriptComponent->className = scriptClassName;
        scriptComponent->contextType = ScriptContextType_PYTHON;
        component_manager_set_component(entity, ComponentDataIndex_SCRIPT, scriptComponent);
        rbe_logger_debug("class_path: %s, class_name: %s", scriptClassPath, scriptClassName);
    } else {
        rbe_logger_error("Invalid component class name: '%s'", className);
    }
}

// Input
PyObject* rbe_py_api_input_add_action(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    char* actionValue;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "ss", rbePyApiInputAddActionKWList, &actionName, &actionValue)) {
        rbe_input_add_action_value(actionName, actionValue);
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_input_is_action_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiInputActionInputCheckKWList, &actionName)) {
        if (rbe_input_is_action_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* rbe_py_api_input_is_action_just_pressed(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiInputActionInputCheckKWList, &actionName)) {
        if (rbe_input_is_action_just_pressed(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
}

PyObject* rbe_py_api_input_is_action_just_released(PyObject* self, PyObject* args, PyObject* kwargs) {
    char* actionName;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "s", rbePyApiInputActionInputCheckKWList, &actionName)) {
        if (rbe_input_is_action_just_released(actionName)) {
            Py_RETURN_TRUE;
        }
        Py_RETURN_FALSE;
    }
    return NULL;
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

// Node2D
PyObject* rbe_py_api_node2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiNode2DSetPositionKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->position.x = x;
        transformComp->position.y = y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    float x;
    float y;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "iff", rbePyApiNode2DSetPositionKWList, &entity, &x, &y)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        transformComp->position.x += x;
        transformComp->position.y += y;
        Py_RETURN_NONE;
    }
    return NULL;
}

PyObject* rbe_py_api_node2D_get_position(PyObject* self, PyObject* args, PyObject* kwargs) {
    Entity entity;
    if (PyArg_ParseTupleAndKeywords(args, kwargs, "i", rbePyApiGenericGetEntityKWList, &entity)) {
        Transform2DComponent* transformComp = (Transform2DComponent*) component_manager_get_component(entity, ComponentDataIndex_TRANSFORM_2D);
        return Py_BuildValue("(ff)", transformComp->position.x, transformComp->position.y);
        Py_RETURN_NONE;
    }
    return NULL;
}
