#pragma once

#define PY_SSIZE_T_CLEAN
#include <Python.h>

// --- Module Methods --- //

// Engine
PyObject* cre_py_api_engine_exit(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_engine_set_target_fps(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_engine_get_target_fps(PyObject* self, PyObject* args);
PyObject* cre_py_api_engine_get_average_fps(PyObject* self, PyObject* args);
PyObject* cre_py_api_engine_set_fps_display_enabled(PyObject* self, PyObject* args, PyObject* kwargs);

// Input
PyObject* cre_py_api_input_add_action(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_input_is_action_pressed(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_input_is_action_just_pressed(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_input_is_action_just_released(PyObject* self, PyObject* args, PyObject* kwargs);

// Camera
PyObject* cre_py_api_camera2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_get_position(PyObject* self, PyObject* args);
PyObject* cre_py_api_camera2D_set_offset(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_add_to_offset(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_get_offset(PyObject* self, PyObject* args);
PyObject* cre_py_api_camera2D_set_zoom(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_add_to_zoom(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_get_zoom(PyObject* self, PyObject* args);
PyObject* cre_py_api_camera2D_set_boundary(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_get_boundary(PyObject* self, PyObject* args);

// SceneTree
PyObject* cre_py_api_scene_tree_change_scene(PyObject* self, PyObject* args, PyObject* kwargs);

// Audio Manager
PyObject* cre_py_api_audio_manager_play_sound(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_audio_manager_stop_sound(PyObject* self, PyObject* args, PyObject* kwargs);

// Node
PyObject* cre_py_api_node_new(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_queue_deletion(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_add_child(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_child(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_children(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_parent(PyObject* self, PyObject* args, PyObject* kwargs);

// Node2D
PyObject* cre_py_api_node2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_get_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_get_global_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_set_scale(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_add_to_scale(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_get_scale(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_set_rotation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_add_to_rotation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_get_rotation(PyObject* self, PyObject* args, PyObject* kwargs);

// Sprite
PyObject* cre_py_api_sprite_set_texture(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_texture(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_set_draw_source(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_draw_source(PyObject* self, PyObject* args, PyObject* kwargs);

// Animated Sprite
PyObject* cre_py_api_animated_sprite_play(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_stop(PyObject* self, PyObject* args, PyObject* kwargs);

// Text Label
PyObject* cre_py_api_text_label_set_text(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_get_text(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_set_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_get_color(PyObject* self, PyObject* args, PyObject* kwargs);

// Collider2D
PyObject* cre_py_api_collider2D_set_extents(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_collider2D_get_extents(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_collider2D_set_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_collider2D_get_color(PyObject* self, PyObject* args, PyObject* kwargs);

// ColorRect
PyObject* cre_py_api_color_rect_set_size(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_color_rect_get_size(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_color_rect_set_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_color_rect_get_color(PyObject* self, PyObject* args, PyObject* kwargs);

// Network
PyObject* cre_py_api_network_is_server(PyObject* self, PyObject* args);

PyObject* cre_py_api_server_start(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_server_stop(PyObject* self, PyObject* args);
PyObject* cre_py_api_server_send(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_server_subscribe(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* cre_py_api_client_start(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_client_stop(PyObject* self, PyObject* args);
PyObject* cre_py_api_client_send(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_client_subscribe(PyObject* self, PyObject* args, PyObject* kwargs);

// Collision Handler
PyObject* cre_py_api_collision_handler_process_collisions(PyObject* self, PyObject* args, PyObject* kwargs);

// --- Module Methods Definitions --- //
static struct PyMethodDef crePyApiMethods[] = {
    // ENGINE
    {
        "engine_exit", (PyCFunction) cre_py_api_engine_exit,
        METH_VARARGS | METH_KEYWORDS, "Exits the game."
    },
    {
        "engine_set_target_fps", (PyCFunction) cre_py_api_engine_set_target_fps,
        METH_VARARGS | METH_KEYWORDS, "Sets the target fps."
    },
    {
        "engine_get_target_fps", cre_py_api_engine_get_target_fps,
        METH_VARARGS, "Gets the target fps."
    },
    {
        "engine_get_average_fps", cre_py_api_engine_get_average_fps,
        METH_VARARGS, "Gets the average fps."
    },
    {
        "engine_set_fps_display_enabled", (PyCFunction) cre_py_api_engine_set_fps_display_enabled,
        METH_VARARGS | METH_KEYWORDS, "If enabled will display the average fps."
    },
    // INPUT
    {
        "input_add_action", (PyCFunction) cre_py_api_input_add_action,
        METH_VARARGS | METH_KEYWORDS, "Adds a new input action."
    },
    {
        "input_is_action_pressed", (PyCFunction) cre_py_api_input_is_action_pressed,
        METH_VARARGS | METH_KEYWORDS, "Checks if an input actions is pressed."
    },
    {
        "input_is_action_just_pressed", (PyCFunction) cre_py_api_input_is_action_just_pressed,
        METH_VARARGS | METH_KEYWORDS, "Checks if an input actions was just pressed."
    },
    {
        "input_is_action_just_released", (PyCFunction) cre_py_api_input_is_action_just_released,
        METH_VARARGS | METH_KEYWORDS, "Checks if an input actions was just released."
    },
    // CAMERA
    {
        "camera2D_set_position", (PyCFunction) cre_py_api_camera2D_set_position,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's position."
    },
    {
        "camera2D_add_to_position", (PyCFunction) cre_py_api_camera2D_add_to_position,
        METH_VARARGS | METH_KEYWORDS, "Adds to the camera's position."
    },
    {
        "camera2D_get_position", (PyCFunction) cre_py_api_camera2D_get_position,
        METH_VARARGS | METH_KEYWORDS, "Get the camera's position."
    },
    {
        "camera2D_set_offset", (PyCFunction) cre_py_api_camera2D_set_offset,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's offset."
    },
    {
        "camera2D_add_to_offset", (PyCFunction) cre_py_api_camera2D_add_to_offset,
        METH_VARARGS | METH_KEYWORDS, "Adds to the camera's offset."
    },
    {
        "camera2D_get_offset", (PyCFunction) cre_py_api_camera2D_get_offset,
        METH_VARARGS | METH_KEYWORDS, "Get the camera's offset."
    },
    {
        "camera2D_set_zoom", (PyCFunction) cre_py_api_camera2D_set_zoom,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's zoom."
    },
    {
        "camera2D_add_to_zoom", (PyCFunction) cre_py_api_camera2D_add_to_zoom,
        METH_VARARGS | METH_KEYWORDS, "Adds to the camera's zoom."
    },
    {
        "camera2D_get_zoom", (PyCFunction) cre_py_api_camera2D_get_zoom,
        METH_VARARGS | METH_KEYWORDS, "Get the camera's zoom."
    },
    {
        "camera2D_set_boundary", (PyCFunction) cre_py_api_camera2D_set_boundary,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's boundary."
    },
    {
        "camera2D_get_boundary", (PyCFunction) cre_py_api_camera2D_get_boundary,
        METH_VARARGS, "Gets the camera's boundary."
    },
    // SCENE TREE
    {
        "scene_tree_change_scene", (PyCFunction) cre_py_api_scene_tree_change_scene,
        METH_VARARGS | METH_KEYWORDS, "Change to a new scene."
    },
    // AUDIO MANAGER
    {
        "audio_manager_play_sound", (PyCFunction) cre_py_api_audio_manager_play_sound,
        METH_VARARGS | METH_KEYWORDS, "Plays a sound."
    },
    {
        "audio_manager_stop_sound", (PyCFunction) cre_py_api_audio_manager_stop_sound,
        METH_VARARGS | METH_KEYWORDS, "Stops a sound."
    },
    // NODE
    {
        "node_new", (PyCFunction) cre_py_api_node_new,
        METH_VARARGS | METH_KEYWORDS, "Creates a new node."
    },
    {
        "node_queue_deletion", (PyCFunction) cre_py_api_node_queue_deletion,
        METH_VARARGS | METH_KEYWORDS, "Queues a node for deletion."
    },
    {
        "node_add_child", (PyCFunction) cre_py_api_node_add_child,
        METH_VARARGS | METH_KEYWORDS, "Adds a node to the scene."
    },
    {
        "node_get_child", (PyCFunction) cre_py_api_node_get_child,
        METH_VARARGS | METH_KEYWORDS, "Node get child."
    },
    {
        "node_get_children", (PyCFunction) cre_py_api_node_get_children,
        METH_VARARGS | METH_KEYWORDS, "Get all child nodes from a node."
    },
    {
        "node_get_parent", (PyCFunction) cre_py_api_node_get_parent,
        METH_VARARGS | METH_KEYWORDS, "Node get parent."
    },
    // NODE2D
    {
        "node2D_set_position", (PyCFunction) cre_py_api_node2D_set_position,
        METH_VARARGS | METH_KEYWORDS, "Set the position of a node."
    },
    {
        "node2D_add_to_position", (PyCFunction) cre_py_api_node2D_add_to_position,
        METH_VARARGS | METH_KEYWORDS, "Adds to the position of a node."
    },
    {
        "node2D_get_position", (PyCFunction) cre_py_api_node2D_get_position,
        METH_VARARGS | METH_KEYWORDS, "Get the position of a node."
    },
    {
        "node2D_get_global_position", (PyCFunction) cre_py_api_node2D_get_global_position,
        METH_VARARGS | METH_KEYWORDS, "Get the global position of a node."
    },
    {
        "node2D_set_scale", (PyCFunction) cre_py_api_node2D_set_scale,
        METH_VARARGS | METH_KEYWORDS, "Set the scale of a node."
    },
    {
        "node2D_add_to_scale", (PyCFunction) cre_py_api_node2D_add_to_scale,
        METH_VARARGS | METH_KEYWORDS, "Adds to the scale of a node."
    },
    {
        "node2D_get_scale", (PyCFunction) cre_py_api_node2D_get_scale,
        METH_VARARGS | METH_KEYWORDS, "Get the scale of a node."
    },
    {
        "node2D_set_rotation", (PyCFunction) cre_py_api_node2D_set_rotation,
        METH_VARARGS | METH_KEYWORDS, "Set the rotation of a node."
    },
    {
        "node2D_add_to_rotation", (PyCFunction) cre_py_api_node2D_add_to_rotation,
        METH_VARARGS | METH_KEYWORDS, "Adds to the rotation of a node."
    },
    {
        "node2D_get_rotation", (PyCFunction) cre_py_api_node2D_get_rotation,
        METH_VARARGS | METH_KEYWORDS, "Get the rotation of a node."
    },
    // SPRITE
    {
        "sprite_set_texture", (PyCFunction) cre_py_api_sprite_set_texture,
        METH_VARARGS | METH_KEYWORDS, "Sets the texture for a sprite."
    },
    {
        "sprite_get_texture", (PyCFunction) cre_py_api_sprite_get_texture,
        METH_VARARGS | METH_KEYWORDS, "Gets the texture for a sprite."
    },
    {
        "sprite_set_draw_source", (PyCFunction) cre_py_api_sprite_set_draw_source,
        METH_VARARGS | METH_KEYWORDS, "Sets the draw source for a sprite."
    },
    {
        "sprite_get_draw_source", (PyCFunction) cre_py_api_sprite_get_draw_source,
        METH_VARARGS | METH_KEYWORDS, "Gets the draw source for a sprite."
    },
    // ANIMATED SPRITE
    {
        "animated_sprite_play", (PyCFunction) cre_py_api_animated_sprite_play,
        METH_VARARGS | METH_KEYWORDS, "Will play the animation."
    },
    {
        "animated_sprite_stop", (PyCFunction) cre_py_api_animated_sprite_stop,
        METH_VARARGS | METH_KEYWORDS, "Will stop the currently playing animation."
    },
    // TEXT LABEL
    {
        "text_label_set_text", (PyCFunction) cre_py_api_text_label_set_text,
        METH_VARARGS | METH_KEYWORDS, "Sets the text label's text."
    },
    {
        "text_label_get_text", (PyCFunction) cre_py_api_text_label_get_text,
        METH_VARARGS | METH_KEYWORDS, "Gets the text label's text."
    },
    {
        "text_label_set_color", (PyCFunction) cre_py_api_text_label_set_color,
        METH_VARARGS | METH_KEYWORDS, "Sets the text label's color."
    },
    {
        "text_label_get_color", (PyCFunction) cre_py_api_text_label_get_color,
        METH_VARARGS | METH_KEYWORDS, "Gets the text label's color."
    },
    // Collider2D
    {
        "collider2D_set_extents", (PyCFunction) cre_py_api_collider2D_set_extents,
        METH_VARARGS | METH_KEYWORDS, "Sets the collider's collision extents."
    },
    {
        "collider2D_get_extents", (PyCFunction) cre_py_api_collider2D_get_extents,
        METH_VARARGS | METH_KEYWORDS, "Gets the collider's collision extents."
    },
    {
        "collider2D_set_color", (PyCFunction) cre_py_api_collider2D_set_color,
        METH_VARARGS | METH_KEYWORDS, "Sets the collider's color."
    },
    {
        "collider2D_get_color", (PyCFunction) cre_py_api_collider2D_get_color,
        METH_VARARGS | METH_KEYWORDS, "Gets the collider's color."
    },
    // ColorRect
    {
        "color_rect_set_size", (PyCFunction) cre_py_api_color_rect_set_size,
        METH_VARARGS | METH_KEYWORDS, "Sets the color rectangle's size."
    },
    {
        "color_rect_get_size", (PyCFunction) cre_py_api_color_rect_get_size,
        METH_VARARGS | METH_KEYWORDS, "Gets the color rectangle's size."
    },
    {
        "color_rect_set_color", (PyCFunction) cre_py_api_color_rect_set_color,
        METH_VARARGS | METH_KEYWORDS, "Sets the color rectangle's color."
    },
    {
        "color_rect_get_color", (PyCFunction) cre_py_api_color_rect_get_color,
        METH_VARARGS | METH_KEYWORDS, "Gets the color rectangle's color."
    },
    // NETWORK
    {
        "network_is_server", cre_py_api_network_is_server,
        METH_VARARGS, "Returns if the current network is running a server."
    },
    // Server
    {
        "server_start", (PyCFunction) cre_py_api_server_start,
        METH_VARARGS | METH_KEYWORDS, "Starts a network server."
    },
    {
        "server_stop", cre_py_api_server_stop,
        METH_VARARGS, "Stops a network server."
    },
    {
        "server_send", (PyCFunction) cre_py_api_server_send,
        METH_VARARGS | METH_KEYWORDS, "Sends a message to a connected client."
    },
    {
        "server_subscribe", (PyCFunction) cre_py_api_server_subscribe,
        METH_VARARGS | METH_KEYWORDS, "Subscribe a node's function to one of the server's signals."
    },
    // Client
    {
        "client_start", (PyCFunction) cre_py_api_client_start,
        METH_VARARGS | METH_KEYWORDS, "Starts a network client."
    },
    {
        "client_stop", cre_py_api_client_stop,
        METH_VARARGS, "Stops a network client."
    },
    {
        "client_send", (PyCFunction) cre_py_api_client_send,
        METH_VARARGS | METH_KEYWORDS, "Sends a message to the server."
    },
    {
        "client_subscribe", (PyCFunction) cre_py_api_client_subscribe,
        METH_VARARGS | METH_KEYWORDS, "Subscribe a node's function to one of the client's signals."
    },
    {
        "collision_handler_process_collisions", (PyCFunction) cre_py_api_collision_handler_process_collisions,
        METH_VARARGS | METH_KEYWORDS, "Returns collided entities."
    },
    // COLLISION HANDLER
    { NULL, NULL, 0,NULL },
};

// --- Module Definition --- //
static struct PyModuleDef crePyAPIModDef = {
    PyModuleDef_HEAD_INIT, "crescent_api_internal", NULL, -1, crePyApiMethods,
    NULL, NULL, NULL, NULL
};

// --- Argument Lists --- //
static char* crePyApiGenericGetEntityKWList[] = {"entity_id", NULL};
static char* crePyApiGenericPathKWList[] = {"path", NULL};
static char* crePyApiGenericEnabledKWList[] = {"enabled", NULL};
static char* crePyApiGenericXYKWList[] = {"x", "y", NULL};
static char* crePyApiGenericXYWHKWList[] = {"x", "y", "w", "h", NULL};
static char* crePyApiGenericSetEntitySize2DKWList[] = {"entity_id", "w", "h", NULL};
static char* crePyApiGenericSetEntityRectKWList[] = {"entity_id", "x", "y", "w", "h", NULL};
static char* crePyApiGenericSetEntityColorKWList[] = {"entity_id", "r", "g", "b", "a", NULL};

static char* crePyApiEngineExitKWList[] = {"code", NULL};
static char* crePyApiEngineSetTargetFPSKWList[] = {"fps", NULL};

static char* crePyApiInputAddActionKWList[] = {"name", "value", "device_id", NULL};
static char* crePyApiInputActionInputCheckKWList[] = {"name", NULL};

static char* crePyApiAudioManagerPlaySoundKWList[] = {"path", "loops", NULL};

static char* crePyApiNodeNewKWList[] = {"class_path", "class_name", "node_type", NULL};
static char* crePyApiNodeAddChildKWList[] = {"parent_entity_id", "child_entity_id", NULL};
static char* crePyApiNodeGetChildKWList[] = {"entity_id", "child_name", NULL};

static char* crePyApiNode2DSetXYKWList[] = {"entity_id", "x", "y", NULL};
static char* crePyApiNode2DSetRotationKWList[] = {"entity_id", "rotation", NULL};

static char* crePyApiSpriteSetTextureKWList[] = {"entity_id", "file_path", NULL};

static char* crePyApiAnimatedSpriteSetAnimationKWList[] = {"entity_id", "animation_name", NULL};

static char* crePyApiTextLabelSetTextKWList[] = {"entity_id", "text", NULL};

static char* crePyApiNetworkSendMessageKWList[] = {"message", NULL};
static char* crePyApiNetworkSubscribeKWList[] = {"signal_id", "listener_node", "listener_func", NULL};

static char* crePyApiServerStartKWList[] = {"port", NULL};

static char* crePyApiClientStartKWList[] = {"host", "port", NULL};

// --- Module Init --- //
PyObject* PyInit_cre_py_API(void);
