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

// Camera
PyObject* rbe_py_api_camera2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_camera2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_camera2D_get_position(PyObject* self, PyObject* args);
PyObject* rbe_py_api_camera2D_set_offset(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_camera2D_add_to_offset(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_camera2D_get_offset(PyObject* self, PyObject* args);
PyObject* rbe_py_api_camera2D_set_zoom(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_camera2D_add_to_zoom(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_camera2D_get_zoom(PyObject* self, PyObject* args);
PyObject* rbe_py_api_camera2D_set_boundary(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_camera2D_get_boundary(PyObject* self, PyObject* args);

// SceneTree
PyObject* rbe_py_api_scene_tree_change_scene(PyObject* self, PyObject* args, PyObject* kwargs);

// Audio Manager
PyObject* rbe_py_api_audio_manager_play_sound(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_audio_manager_stop_sound(PyObject* self, PyObject* args, PyObject* kwargs);

// Node
PyObject* rbe_py_api_node_new(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node_queue_deletion(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node_add_child(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node_get_child(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node_get_children(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node_get_parent(PyObject* self, PyObject* args, PyObject* kwargs);

// Node2D
PyObject* rbe_py_api_node2D_set_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_add_to_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_get_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_get_global_position(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_set_scale(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_add_to_scale(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_get_scale(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_set_rotation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_add_to_rotation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_node2D_get_rotation(PyObject* self, PyObject* args, PyObject* kwargs);

// Sprite
PyObject* rbe_py_api_sprite_set_texture(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_sprite_get_texture(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_sprite_set_draw_source(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_sprite_get_draw_source(PyObject* self, PyObject* args, PyObject* kwargs);

// Animated Sprite
PyObject* rbe_py_api_animated_sprite_play(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_animated_sprite_stop(PyObject* self, PyObject* args, PyObject* kwargs);

// Text Label
PyObject* rbe_py_api_text_label_set_text(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_text_label_get_text(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_text_label_set_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_text_label_get_color(PyObject* self, PyObject* args, PyObject* kwargs);

// Collider2D
PyObject* rbe_py_api_collider2D_set_extents(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_collider2D_get_extents(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_collider2D_set_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_collider2D_get_color(PyObject* self, PyObject* args, PyObject* kwargs);

// ColorRect
PyObject* rbe_py_api_color_rect_set_size(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_color_rect_get_size(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_color_rect_set_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_color_rect_get_color(PyObject* self, PyObject* args, PyObject* kwargs);

// Network
PyObject* rbe_py_api_network_is_server(PyObject* self, PyObject* args);

PyObject* rbe_py_api_server_start(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_server_stop(PyObject* self, PyObject* args);
PyObject* rbe_py_api_server_send(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_server_subscribe(PyObject* self, PyObject* args, PyObject* kwargs);

PyObject* rbe_py_api_client_start(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_client_stop(PyObject* self, PyObject* args);
PyObject* rbe_py_api_client_send(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* rbe_py_api_client_subscribe(PyObject* self, PyObject* args, PyObject* kwargs);

// Collision Handler
PyObject* rbe_py_api_collision_handler_process_collisions(PyObject* self, PyObject* args, PyObject* kwargs);

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
    // CAMERA
    {
        "camera2D_set_position", (PyCFunction) rbe_py_api_camera2D_set_position,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's position."
    },
    {
        "camera2D_add_to_position", (PyCFunction) rbe_py_api_camera2D_add_to_position,
        METH_VARARGS | METH_KEYWORDS, "Adds to the camera's position."
    },
    {
        "camera2D_get_position", (PyCFunction) rbe_py_api_camera2D_get_position,
        METH_VARARGS | METH_KEYWORDS, "Get the camera's position."
    },
    {
        "camera2D_set_offset", (PyCFunction) rbe_py_api_camera2D_set_offset,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's offset."
    },
    {
        "camera2D_add_to_offset", (PyCFunction) rbe_py_api_camera2D_add_to_offset,
        METH_VARARGS | METH_KEYWORDS, "Adds to the camera's offset."
    },
    {
        "camera2D_get_offset", (PyCFunction) rbe_py_api_camera2D_get_offset,
        METH_VARARGS | METH_KEYWORDS, "Get the camera's offset."
    },
    {
        "camera2D_set_zoom", (PyCFunction) rbe_py_api_camera2D_set_zoom,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's zoom."
    },
    {
        "camera2D_add_to_zoom", (PyCFunction) rbe_py_api_camera2D_add_to_zoom,
        METH_VARARGS | METH_KEYWORDS, "Adds to the camera's zoom."
    },
    {
        "camera2D_get_zoom", (PyCFunction) rbe_py_api_camera2D_get_zoom,
        METH_VARARGS | METH_KEYWORDS, "Get the camera's zoom."
    },
    {
        "camera2D_set_boundary", (PyCFunction) rbe_py_api_camera2D_set_boundary,
        METH_VARARGS | METH_KEYWORDS, "Sets the camera's boundary."
    },
    {
        "camera2D_get_boundary", (PyCFunction) rbe_py_api_camera2D_get_boundary,
        METH_VARARGS, "Gets the camera's boundary."
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
    // NODE
    {
        "node_new", (PyCFunction) rbe_py_api_node_new,
        METH_VARARGS | METH_KEYWORDS, "Creates a new node."
    },
    {
        "node_queue_deletion", (PyCFunction) rbe_py_api_node_queue_deletion,
        METH_VARARGS | METH_KEYWORDS, "Queues a node for deletion."
    },
    {
        "node_add_child", (PyCFunction) rbe_py_api_node_add_child,
        METH_VARARGS | METH_KEYWORDS, "Adds a node to the scene."
    },
    {
        "node_get_child", (PyCFunction) rbe_py_api_node_get_child,
        METH_VARARGS | METH_KEYWORDS, "Node get child."
    },
    {
        "node_get_children", (PyCFunction) rbe_py_api_node_get_children,
        METH_VARARGS | METH_KEYWORDS, "Get all child nodes from a node."
    },
    {
        "node_get_parent", (PyCFunction) rbe_py_api_node_get_parent,
        METH_VARARGS | METH_KEYWORDS, "Node get parent."
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
    {
        "node2D_get_global_position", (PyCFunction) rbe_py_api_node2D_get_global_position,
        METH_VARARGS | METH_KEYWORDS, "Get the global position of a node."
    },
    {
        "node2D_set_scale", (PyCFunction) rbe_py_api_node2D_set_scale,
        METH_VARARGS | METH_KEYWORDS, "Set the scale of a node."
    },
    {
        "node2D_add_to_scale", (PyCFunction) rbe_py_api_node2D_add_to_scale,
        METH_VARARGS | METH_KEYWORDS, "Adds to the scale of a node."
    },
    {
        "node2D_get_scale", (PyCFunction) rbe_py_api_node2D_get_scale,
        METH_VARARGS | METH_KEYWORDS, "Get the scale of a node."
    },
    {
        "node2D_set_rotation", (PyCFunction) rbe_py_api_node2D_set_rotation,
        METH_VARARGS | METH_KEYWORDS, "Set the rotation of a node."
    },
    {
        "node2D_add_to_rotation", (PyCFunction) rbe_py_api_node2D_add_to_rotation,
        METH_VARARGS | METH_KEYWORDS, "Adds to the rotation of a node."
    },
    {
        "node2D_get_rotation", (PyCFunction) rbe_py_api_node2D_get_rotation,
        METH_VARARGS | METH_KEYWORDS, "Get the rotation of a node."
    },
    // SPRITE
    {
        "sprite_set_texture", (PyCFunction) rbe_py_api_sprite_set_texture,
        METH_VARARGS | METH_KEYWORDS, "Sets the texture for a sprite."
    },
    {
        "sprite_get_texture", (PyCFunction) rbe_py_api_sprite_get_texture,
        METH_VARARGS | METH_KEYWORDS, "Gets the texture for a sprite."
    },
    {
        "sprite_set_draw_source", (PyCFunction) rbe_py_api_sprite_set_draw_source,
        METH_VARARGS | METH_KEYWORDS, "Sets the draw source for a sprite."
    },
    {
        "sprite_get_draw_source", (PyCFunction) rbe_py_api_sprite_get_draw_source,
        METH_VARARGS | METH_KEYWORDS, "Gets the draw source for a sprite."
    },
    // ANIMATED SPRITE
    {
        "animated_sprite_play", (PyCFunction) rbe_py_api_animated_sprite_play,
        METH_VARARGS | METH_KEYWORDS, "Will play the animation."
    },
    {
        "animated_sprite_stop", (PyCFunction) rbe_py_api_animated_sprite_stop,
        METH_VARARGS | METH_KEYWORDS, "Will stop the currently playing animation."
    },
    // TEXT LABEL
    {
        "text_label_set_text", (PyCFunction) rbe_py_api_text_label_set_text,
        METH_VARARGS | METH_KEYWORDS, "Sets the text label's text."
    },
    {
        "text_label_get_text", (PyCFunction) rbe_py_api_text_label_get_text,
        METH_VARARGS | METH_KEYWORDS, "Gets the text label's text."
    },
    {
        "text_label_set_color", (PyCFunction) rbe_py_api_text_label_set_color,
        METH_VARARGS | METH_KEYWORDS, "Sets the text label's color."
    },
    {
        "text_label_get_color", (PyCFunction) rbe_py_api_text_label_get_color,
        METH_VARARGS | METH_KEYWORDS, "Gets the text label's color."
    },
    // Collider2D
    {
        "collider2D_set_extents", (PyCFunction) rbe_py_api_collider2D_set_extents,
        METH_VARARGS | METH_KEYWORDS, "Sets the collider's collision extents."
    },
    {
        "collider2D_get_extents", (PyCFunction) rbe_py_api_collider2D_get_extents,
        METH_VARARGS | METH_KEYWORDS, "Gets the collider's collision extents."
    },
    {
        "collider2D_set_color", (PyCFunction) rbe_py_api_collider2D_set_color,
        METH_VARARGS | METH_KEYWORDS, "Sets the collider's color."
    },
    {
        "collider2D_get_color", (PyCFunction) rbe_py_api_collider2D_get_color,
        METH_VARARGS | METH_KEYWORDS, "Gets the collider's color."
    },
    // ColorRect
    {
        "color_rect_set_size", (PyCFunction) rbe_py_api_color_rect_set_size,
        METH_VARARGS | METH_KEYWORDS, "Sets the color rectangle's size."
    },
    {
        "color_rect_get_size", (PyCFunction) rbe_py_api_color_rect_get_size,
        METH_VARARGS | METH_KEYWORDS, "Gets the color rectangle's size."
    },
    {
        "color_rect_set_color", (PyCFunction) rbe_py_api_color_rect_set_color,
        METH_VARARGS | METH_KEYWORDS, "Sets the color rectangle's color."
    },
    {
        "color_rect_get_color", (PyCFunction) rbe_py_api_color_rect_get_color,
        METH_VARARGS | METH_KEYWORDS, "Gets the color rectangle's color."
    },
    // NETWORK
    {
        "network_is_server", rbe_py_api_network_is_server,
        METH_VARARGS, "Returns if the current network is running a server."
    },
    // Server
    {
        "server_start", (PyCFunction) rbe_py_api_server_start,
        METH_VARARGS | METH_KEYWORDS, "Starts a network server."
    },
    {
        "server_stop", rbe_py_api_server_stop,
        METH_VARARGS, "Stops a network server."
    },
    {
        "server_send", (PyCFunction) rbe_py_api_server_send,
        METH_VARARGS | METH_KEYWORDS, "Sends a message to a connected client."
    },
    {
        "server_subscribe", (PyCFunction) rbe_py_api_server_subscribe,
        METH_VARARGS | METH_KEYWORDS, "Subscribe a node's function to one of the server's signals."
    },
    // Client
    {
        "client_start", (PyCFunction) rbe_py_api_client_start,
        METH_VARARGS | METH_KEYWORDS, "Starts a network client."
    },
    {
        "client_stop", rbe_py_api_client_stop,
        METH_VARARGS, "Stops a network client."
    },
    {
        "client_send", (PyCFunction) rbe_py_api_client_send,
        METH_VARARGS | METH_KEYWORDS, "Sends a message to the server."
    },
    {
        "client_subscribe", (PyCFunction) rbe_py_api_client_subscribe,
        METH_VARARGS | METH_KEYWORDS, "Subscribe a node's function to one of the client's signals."
    },
    {
        "collision_handler_process_collisions", (PyCFunction) rbe_py_api_collision_handler_process_collisions,
        METH_VARARGS | METH_KEYWORDS, "Returns collided entities."
    },
    // COLLISION HANDLER
    { NULL, NULL, 0,NULL },
};

// --- Module Definition --- //
static struct PyModuleDef rbePyAPIModDef = {
    PyModuleDef_HEAD_INIT, "crescent_api_internal", NULL, -1, rbePyApiMethods,
    NULL, NULL, NULL, NULL
};

// --- Argument Lists --- //
static char *rbePyApiGenericGetEntityKWList[] = {"entity_id", NULL};
static char *rbePyApiGenericPathKWList[] = {"path", NULL};
static char *rbePyApiGenericEnabledKWList[] = {"enabled", NULL};
static char *rbePyApiGenericXYKWList[] = {"x", "y", NULL};
static char *rbePyApiGenericXYWHKWList[] = {"x", "y", "w", "h", NULL};
static char *rbePyApiGenericSetEntitySize2DKWList[] = {"entity_id", "w", "h", NULL};
static char *rbePyApiGenericSetEntityRectKWList[] = {"entity_id", "x", "y", "w", "h", NULL};
static char *rbePyApiGenericSetEntityColorKWList[] = {"entity_id", "r", "g", "b", "a", NULL};

static char *rbePyApiEngineExitKWList[] = {"code", NULL};
static char *rbePyApiEngineSetTargetFPSKWList[] = {"fps", NULL};

static char *rbePyApiProjectConfigureKWList[] = {"game_title", "window_width", "window_height", "resolution_width", "resolution_height", "target_fps", "initial_node_path", "colliders_visible", NULL};
static char *rbePyApiConfigureAssetsKWList[] = {"audio_sources", "textures", "fonts", NULL};
static char *rbePyApiConfigureInputsKWList[] = {"input_actions", NULL};

static char *rbePyApiCreateStageNodesKWList[] = {"stage_nodes", NULL};

static char *rbePyApiInputAddActionKWList[] = {"name", "value", "device_id", NULL};
static char *rbePyApiInputActionInputCheckKWList[] = {"name", NULL};

static char *rbePyApiAudioManagerPlaySoundKWList[] = {"path", "loops", NULL};

static char *rbePyApiNodeNewKWList[] = {"class_path", "class_name", "node_type", NULL};
static char *rbePyApiNodeAddChildKWList[] = {"parent_entity_id", "child_entity_id", NULL};
static char *rbePyApiNodeGetChildKWList[] = {"entity_id", "child_name", NULL};

static char *rbePyApiNode2DSetXYKWList[] = {"entity_id", "x", "y", NULL};
static char *rbePyApiNode2DSetRotationKWList[] = {"entity_id", "rotation", NULL};

static char *rbePyApiSpriteSetTextureKWList[] = {"entity_id", "file_path", NULL};

static char *rbePyApiAnimatedSpriteSetAnimationKWList[] = {"entity_id", "animation_name", NULL};

static char *rbePyApiTextLabelSetTextKWList[] = {"entity_id", "text", NULL};

static char *rbePyApiNetworkSendMessageKWList[] = {"message", NULL};
static char *rbePyApiNetworkSubscribeKWList[] = {"signal_id", "listener_node", "listener_func", NULL};

static char *rbePyApiServerStartKWList[] = {"port", NULL};

static char *rbePyApiClientStartKWList[] = {"host", "port", NULL};

// --- Module Init --- //
PyObject* PyInit_rbe_py_API(void);
