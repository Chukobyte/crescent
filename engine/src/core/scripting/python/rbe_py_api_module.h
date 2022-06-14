#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// --- Module Methods --- //

// Engine
PyObject* rbe_py_api_engine_exit(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_engine_set_target_fps(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_engine_get_target_fps(PyObject* self, PyObject* args);
PyObject* rbe_py_api_engine_get_average_fps(PyObject* self, PyObject* args);
PyObject* rbe_py_api_engine_set_fps_display_enabled(PyObject* self, PyObject* args, PyObject* kwargs);

// Configuration
PyObject* rbe_py_api_configure_game(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_configure_assets(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_configure_inputs(PyObject* self, PyObject* args, PyObject* kwargs);

// Stage (Scene)
PyObject* rbe_py_api_create_stage_nodes(PyObject* self, PyObject* args, PyObject* kwargs);

// Input
PyObject* rbe_py_api_input_add_action(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_input_is_action_pressed(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_input_is_action_just_pressed(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_input_is_action_just_released(PyObject* self, PyObject* args, PyObject* kwargs);

// SceneTree
PyObject* rbe_py_api_scene_tree_change_scene(PyObject* self, PyObject* args, PyObject* kwargs);

// Audio Manager
PyObject* rbe_py_api_audio_manager_play_sound(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_audio_manager_stop_sound(PyObject* self, PyObject* args, PyObject* kwargs);

// Node2D
PyObject* rbe_py_api_node2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_get_position(PyObject* self, PyObject* args, PyObject* kwargs);


// --- Module Methods Definitions --- //
static struct PyMethodDef rbePyApiMethods[] = {
    // ENGINE
    {
        "engine_exit", (PyCFunction) rbe_py_api_engine_exit,
        METH_VARARGS | METH_KEYWORDS, "Exits the game."
    },
    {
        "engine_set_target_fps", (PyCFunction) rbe_py_api_engine_set_target_fps,
        METH_VARARGS | METH_KEYWORDS, "Sets the target fps."
    },
    {
        "engine_get_target_fps", rbe_py_api_engine_get_target_fps,
        METH_VARARGS, "Gets the target fps."
    },
    {
        "engine_get_average_fps", rbe_py_api_engine_get_average_fps,
        METH_VARARGS, "Gets the average fps."
    },
    {
        "engine_set_fps_display_enabled", (PyCFunction) rbe_py_api_engine_set_fps_display_enabled,
        METH_VARARGS | METH_KEYWORDS, "If enabled will display the average fps."
    },
    // CONFIGURATION
    {
        "configure_game", (PyCFunction) rbe_py_api_configure_game,
        METH_VARARGS | METH_KEYWORDS, "Configures the game project properties."
    },
    {
        "configure_assets", (PyCFunction) rbe_py_api_configure_assets,
        METH_VARARGS | METH_KEYWORDS, "Configure metadata for game assets."
    },
    {
        "configure_inputs", (PyCFunction) rbe_py_api_configure_inputs,
        METH_VARARGS | METH_KEYWORDS, "Configure the game inputs."
    },
    // STAGE
    {
        "create_stage_nodes", (PyCFunction) rbe_py_api_create_stage_nodes,
        METH_VARARGS | METH_KEYWORDS, "Create stage nodes that will be added to the scene."
    },
    // INPUT
    {
        "input_add_action", (PyCFunction) rbe_py_api_input_add_action,
        METH_VARARGS | METH_KEYWORDS, "Adds a new input action."
    },
    {
        "input_is_action_pressed", (PyCFunction) rbe_py_api_input_is_action_pressed,
        METH_VARARGS | METH_KEYWORDS, "Checks if an input actions is pressed."
    },
    {
        "input_is_action_just_pressed", (PyCFunction) rbe_py_api_input_is_action_just_pressed,
        METH_VARARGS | METH_KEYWORDS, "Checks if an input actions was just pressed."
    },
    {
        "input_is_action_just_released", (PyCFunction) rbe_py_api_input_is_action_just_released,
        METH_VARARGS | METH_KEYWORDS, "Checks if an input actions was just released."
    },
    // SCENE TREE
    {
        "scene_tree_change_scene", (PyCFunction) rbe_py_api_scene_tree_change_scene,
        METH_VARARGS | METH_KEYWORDS, "Change to a new scene."
    },
    // AUDIO MANAGER
    {
        "audio_manager_play_sound", (PyCFunction) rbe_py_api_audio_manager_play_sound,
        METH_VARARGS | METH_KEYWORDS, "Plays a sound."
    },
    {
        "audio_manager_stop_sound", (PyCFunction) rbe_py_api_audio_manager_stop_sound,
        METH_VARARGS | METH_KEYWORDS, "Stops a sound."
    },
    // NODE2D
    {
        "node2D_set_position", (PyCFunction) rbe_py_api_node2D_set_position,
        METH_VARARGS | METH_KEYWORDS, "Set the position of a node."
    },
    {
        "node2D_add_to_position", (PyCFunction) rbe_py_api_node2D_add_to_position,
        METH_VARARGS | METH_KEYWORDS, "Adds to the position of a node."
    },
    {
        "node2D_get_position", (PyCFunction) rbe_py_api_node2D_get_position,
        METH_VARARGS | METH_KEYWORDS, "Get the position of a node."
    },
    { NULL, NULL, 0,NULL },
};

// --- Module Definition --- //
static struct PyModuleDef rbePyAPIModDef = {
    PyModuleDef_HEAD_INIT, "rbe_py_api_internal", NULL, -1, rbePyApiMethods,
    NULL, NULL, NULL, NULL
};

// --- Argument Lists --- //
static char *rbePyApiGenericGetEntityKWList[] = {"entity_id", NULL};
static char *rbePyApiGenericPathKWList[] = {"path", NULL};
static char *rbePyApiGenericEnabledKWList[] = {"enabled", NULL};

static char *rbePyApiEngineExitKWList[] = {"code", NULL};
static char *rbePyApiEngineSetTargetFPSKWList[] = {"fps", NULL};

static char *rbePyApiProjectConfigureKWList[] = {"game_tile", "window_width", "window_height", "resolution_width", "resolution_height", "target_fps", "initial_node_path", NULL};
static char *rbePyApiConfigureAssetsKWList[] = {"audio_sources", "textures", "fonts", NULL};
static char *rbePyApiConfigureInputsKWList[] = {"input_actions", NULL};

static char *rbePyApiCreateStageNodesKWList[] = {"stage_nodes", NULL};

static char *rbePyApiInputAddActionKWList[] = {"name", "value", NULL};
static char *rbePyApiInputActionInputCheckKWList[] = {"name", NULL};

static char *rbePyApiAudioManagerPlaySoundKWList[] = {"path", "loops", NULL};

static char *rbePyApiNode2DSetPositionKWList[] = {"entity_id", "x", "y", NULL};

// --- Module Init --- //
PyObject* PyInit_rbe_py_API(void);
