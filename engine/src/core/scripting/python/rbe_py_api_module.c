#include "rbe_py_api_module.h"

#include "../../game_properties.h"
#include "../../asset_manager.h"
#include "../../utils/rbe_assert.h"
#include "../../scripting/python/py_helper.h"
#include "../../ecs/system/ec_system.h"

// --- Node Utils --- //
void setup_scene_stage_nodes(Entity parentEntity, PyObject* stageNodeList);
void setup_scene_component_node(Entity entity, PyObject* component);

// --- RBE PY API --- //

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
void setup_scene_stage_nodes(Entity parentEntity, PyObject* stageNodeList) {
    for (Py_ssize_t i = 0; i < PyList_Size(stageNodeList); i++) {
        const Entity nodeEntity = rbe_ec_system_create_entity();
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
                setup_scene_component_node(nodeEntity, pComponent);
            }
        }
        // Children Nodes
        PyObject* childrenListVar = PyObject_GetAttrString(pStageNode, "children");
        if (PyList_Check(childrenListVar)) {
            // Recurse through children nodes
            setup_scene_stage_nodes(nodeEntity, childrenListVar);
        }

        rbe_logger_debug("node_name = %s, node_type = %s", nodeName, nodeType);
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
        rbe_logger_debug("position: (%f, %f), scale: (%f, %f), rotation: %f, z_index: %d, z_index_relative: %d, ignore_camera: %d",
                         positionX, positionY, scaleX, scaleY, rotation, zIndex, zIndexRelativeToParent, ignoreCamera);
        Py_DecRef(pPosition);
    } else if (strcmp(className, "SpriteComponent") == 0) {
        rbe_logger_debug("Building sprite component");
    } else if (strcmp(className, "ScriptComponent") == 0) {
        rbe_logger_debug("Building script component");
    } else {
        rbe_logger_error("Invalid component class name: '%s'", className);
    }
}

//class Transform2DComponent:
//    def __init__(
//        self,
//        position: Vector2,
//        scale: Vector2,
//        rotation: float,
//        z_index: int,
//        z_index_relative_to_parent: bool,
//        ignore_camera: bool,
//    ):
//        self.position = position
//        self.scale = scale
//        self.rotation = rotation
//        self.z_index = z_index
//        self.z_index_relative_to_parent = z_index_relative_to_parent
//        self.ignore_camera = ignore_camera
//
//
//class SpriteComponent:
//    def __init__(
//        self,
//        texture_path: str,
//        draw_source: Rect2,
//        flip_x: bool,
//        flip_y: bool,
//        modulate: Color,
//    ):
//        self.texture_path = texture_path
//        self.draw_source = draw_source
//        self.flip_x = flip_x
//        self.flip_y = flip_y
//        self.modulate = modulate
//
//
//class ScriptComponent:
//    def __init__(self, class_path: str, class_name: str):
//        self.class_path = class_path
//        self.class_name = class_name