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
PyObject* cre_py_api_engine_get_global_physics_delta_time(PyObject* self, PyObject* args);

// Curve Float
PyObject* cre_py_api_curve_float_create_new(PyObject* self, PyObject* args);
PyObject* cre_py_api_curve_float_delete(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_curve_float_load_from_file(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_curve_float_add_point(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_curve_float_remove_point(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_curve_float_eval(PyObject* self, PyObject* args, PyObject* kwargs);

// Shader Instance
PyObject* cre_py_api_shader_instance_delete(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_create_bool_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_set_bool_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_get_bool_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_create_int_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_set_int_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_get_int_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_create_float_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_set_float_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_get_float_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_create_float2_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_set_float2_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_get_float2_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_create_float3_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_set_float3_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_get_float3_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_create_float4_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_set_float4_param(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_instance_get_float4_param(PyObject* self, PyObject* args, PyObject* kwargs);

// Shader Util
PyObject* cre_py_api_shader_util_compile_shader(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_util_compile_shader_raw(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_util_set_screen_shader(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_shader_util_reset_screen_shader_to_default(PyObject* self, PyObject* args);

// Input
PyObject* cre_py_api_input_add_action(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_input_is_action_pressed(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_input_is_action_just_pressed(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_input_is_action_just_released(PyObject* self, PyObject* args, PyObject* kwargs);

// Mouse
PyObject* cre_py_api_mouse_get_position(PyObject* self, PyObject* args);
PyObject* cre_py_api_mouse_get_world_position(PyObject* self, PyObject* args);

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
PyObject* cre_py_api_camera2D_follow_node(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_camera2D_unfollow_node(PyObject* self, PyObject* args, PyObject* kwargs);

// SceneTree
PyObject* cre_py_api_scene_tree_change_scene(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_scene_tree_get_root(PyObject* self, PyObject* args);

// World
PyObject* cre_py_api_world_set_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_world_get_time_dilation(PyObject* self, PyObject* args);

// Audio Manager
PyObject* cre_py_api_audio_manager_play_sound(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_audio_manager_stop_sound(PyObject* self, PyObject* args, PyObject* kwargs);

// Audio Source
PyObject* cre_py_api_audio_source_set_pitch(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_audio_source_get_pitch(PyObject* self, PyObject* args, PyObject* kwargs);

// Game Properties
PyObject* cre_py_api_game_properties_get(PyObject* self, PyObject* args);

// Node
PyObject* cre_py_api_node_new(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_queue_deletion(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_is_queued_for_deletion(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_add_child(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_child(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_children(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_parent(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_name(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_set_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_full_time_dilation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_get_full_time_dilation_with_physics_delta(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_create_event(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_subscribe_to_event(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node_broadcast_event(PyObject* self, PyObject* args, PyObject* kwargs);

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
PyObject* cre_py_api_node2D_set_z_index(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_get_z_index(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_set_ignore_camera(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_node2D_get_ignore_camera(PyObject* self, PyObject* args, PyObject* kwargs);

// Sprite
PyObject* cre_py_api_sprite_set_texture(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_texture(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_set_draw_source(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_draw_source(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_set_flip_h(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_flip_h(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_set_flip_v(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_flip_v(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_set_modulate(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_modulate(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_set_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_sprite_get_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs);

// Animated Sprite
PyObject* cre_py_api_animated_sprite_play(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_stop(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_add_animation(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_set_flip_h(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_get_flip_h(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_set_flip_v(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_get_flip_v(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_set_modulate(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_get_modulate(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_set_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_animated_sprite_get_shader_instance(PyObject* self, PyObject* args, PyObject* kwargs);

// Text Label
PyObject* cre_py_api_text_label_set_text(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_get_text(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_set_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_get_color(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_set_font_uid(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_text_label_get_font_uid(PyObject* self, PyObject* args, PyObject* kwargs);

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
PyObject* cre_py_api_collision_handler_process_mouse_collisions(PyObject* self, PyObject* args, PyObject* kwargs);

// Game Config
PyObject* cre_py_api_game_config_save(PyObject* self, PyObject* args, PyObject* kwargs);
PyObject* cre_py_api_game_config_load(PyObject* self, PyObject* args, PyObject* kwargs);

// Packed Scene
PyObject* cre_py_api_packed_scene_create_instance(PyObject* self, PyObject* args, PyObject* kwargs);

// Scene Utils
PyObject* cre_py_api_scene_util_load_scene(PyObject* self, PyObject* args, PyObject* kwargs);


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
    {
        "engine_get_global_physics_delta_time", cre_py_api_engine_get_global_physics_delta_time,
        METH_VARARGS, "Gets the global physics delta time."
    },
    // CURVE FLOAT
    {
        "curve_float_create_new", cre_py_api_curve_float_create_new,
        METH_VARARGS, "Creates a new curve float to be managed by the engine."
    },
    {
        "curve_float_delete", (PyCFunction) cre_py_api_curve_float_delete,
        METH_VARARGS | METH_KEYWORDS, "Deletes a curve float."
    },
    {
        "curve_float_load_from_file", (PyCFunction) cre_py_api_curve_float_load_from_file,
        METH_VARARGS | METH_KEYWORDS, "Load from file."
    },
    {
        "curve_float_add_point", (PyCFunction) cre_py_api_curve_float_add_point,
        METH_VARARGS | METH_KEYWORDS, "Adds a control point to the curve."
    },
    {
        "curve_float_remove_point", (PyCFunction) cre_py_api_curve_float_remove_point,
        METH_VARARGS | METH_KEYWORDS, "Removes a control point from the curve."
    },
    {
        "curve_float_eval", (PyCFunction) cre_py_api_curve_float_eval,
        METH_VARARGS | METH_KEYWORDS, "Evaluates a curve at an x (t) position."
    },
    // SHADER INSTANCE
    {
        "shader_instance_delete", (PyCFunction) cre_py_api_shader_instance_delete,
        METH_VARARGS | METH_KEYWORDS, "Deletes a shader from the shader instance cache."
    },
    {
        "shader_instance_create_bool_param", (PyCFunction) cre_py_api_shader_instance_create_bool_param,
        METH_VARARGS | METH_KEYWORDS, "Creates a bool parameter."
    },
    {
        "shader_instance_set_bool_param", (PyCFunction) cre_py_api_shader_instance_set_bool_param,
        METH_VARARGS | METH_KEYWORDS, "Sets a bool parameter."
    },
    {
        "shader_instance_get_bool_param", (PyCFunction) cre_py_api_shader_instance_get_bool_param,
        METH_VARARGS | METH_KEYWORDS, "Returns a bool parameter."
    },
    {
        "shader_instance_create_int_param", (PyCFunction) cre_py_api_shader_instance_create_int_param,
        METH_VARARGS | METH_KEYWORDS, "Creates a int parameter."
    },
    {
        "shader_instance_set_int_param", (PyCFunction) cre_py_api_shader_instance_set_int_param,
        METH_VARARGS | METH_KEYWORDS, "Sets a int parameter."
    },
    {
        "shader_instance_get_int_param", (PyCFunction) cre_py_api_shader_instance_get_int_param,
        METH_VARARGS | METH_KEYWORDS, "Returns a int parameter."
    },
    {
        "shader_instance_create_float_param", (PyCFunction) cre_py_api_shader_instance_create_float_param,
        METH_VARARGS | METH_KEYWORDS, "Creates a float parameter."
    },
    {
        "shader_instance_set_float_param", (PyCFunction) cre_py_api_shader_instance_set_float_param,
        METH_VARARGS | METH_KEYWORDS, "Sets a float parameter."
    },
    {
        "shader_instance_get_float_param", (PyCFunction) cre_py_api_shader_instance_get_float_param,
        METH_VARARGS | METH_KEYWORDS, "Returns a float parameter."
    },
    {
        "shader_instance_create_float2_param", (PyCFunction) cre_py_api_shader_instance_create_float2_param,
        METH_VARARGS | METH_KEYWORDS, "Creates a float2 parameter."
    },
    {
        "shader_instance_set_float2_param", (PyCFunction) cre_py_api_shader_instance_set_float2_param,
        METH_VARARGS | METH_KEYWORDS, "Sets a float2 parameter."
    },
    {
        "shader_instance_get_float2_param", (PyCFunction) cre_py_api_shader_instance_get_float2_param,
        METH_VARARGS | METH_KEYWORDS, "Returns a float2 parameter."
    },
    {
        "shader_instance_create_float3_param", (PyCFunction) cre_py_api_shader_instance_create_float3_param,
        METH_VARARGS | METH_KEYWORDS, "Creates a float3 parameter."
    },
    {
        "shader_instance_set_float3_param", (PyCFunction) cre_py_api_shader_instance_set_float3_param,
        METH_VARARGS | METH_KEYWORDS, "Sets a float3 parameter."
    },
    {
        "shader_instance_get_float3_param", (PyCFunction) cre_py_api_shader_instance_get_float3_param,
        METH_VARARGS | METH_KEYWORDS, "Returns a float3 parameter."
    },
    {
        "shader_instance_create_float4_param", (PyCFunction) cre_py_api_shader_instance_create_float4_param,
        METH_VARARGS | METH_KEYWORDS, "Creates a float4 parameter."
    },
    {
        "shader_instance_set_float4_param", (PyCFunction) cre_py_api_shader_instance_set_float4_param,
        METH_VARARGS | METH_KEYWORDS, "Sets a float4 parameter."
    },
    {
        "shader_instance_get_float4_param", (PyCFunction) cre_py_api_shader_instance_get_float4_param,
        METH_VARARGS | METH_KEYWORDS, "Returns a float4 parameter."
    },
    // SHADER UTIL
    {
        "shader_util_compile_shader", (PyCFunction) cre_py_api_shader_util_compile_shader,
        METH_VARARGS | METH_KEYWORDS, "Compiles new shader and adds it to the shader instance cache."
    },
    {
        "shader_util_compile_shader_raw", (PyCFunction) cre_py_api_shader_util_compile_shader_raw,
        METH_VARARGS | METH_KEYWORDS, "Compiles raw new shader and adds it to the shader instance cache.  Should only be used if you know what you are doing."
    },
    {
        "shader_util_set_screen_shader", (PyCFunction) cre_py_api_shader_util_set_screen_shader,
        METH_VARARGS | METH_KEYWORDS, "Sets a screen shader from an instance in the shader instance cache."
    },
    {
        "shader_util_reset_screen_shader_to_default", cre_py_api_shader_util_reset_screen_shader_to_default,
        METH_VARARGS, "Resets screen shader to default one."
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
    // MOUSE
    {
        "mouse_get_position", cre_py_api_mouse_get_position,
        METH_VARARGS, "Returns the position of the mouse."
    },
    {
        "mouse_get_world_position", cre_py_api_mouse_get_world_position,
        METH_VARARGS, "Returns the world position of the mouse."
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
        "camera2D_get_boundary", cre_py_api_camera2D_get_boundary,
        METH_VARARGS, "Gets the camera's boundary."
    },
    {
        "camera2D_follow_node", (PyCFunction) cre_py_api_camera2D_follow_node,
        METH_VARARGS | METH_KEYWORDS, "The camera will follow a node."
    },
    {
        "camera2D_unfollow_node", (PyCFunction) cre_py_api_camera2D_unfollow_node,
        METH_VARARGS | METH_KEYWORDS, "The camera will unfollow a node."
    },
    // SCENE TREE
    {
        "scene_tree_change_scene", (PyCFunction) cre_py_api_scene_tree_change_scene,
        METH_VARARGS | METH_KEYWORDS, "Change to a new scene."
    },
    {
        "scene_tree_get_root", cre_py_api_scene_tree_get_root,
        METH_VARARGS, "Returns the current scene's root node."
    },
    // WORLD
    {
        "world_set_time_dilation", (PyCFunction) cre_py_api_world_set_time_dilation,
        METH_VARARGS | METH_KEYWORDS, "Set the global time dilation for world."
    },
    {
        "world_get_time_dilation", cre_py_api_world_get_time_dilation,
        METH_VARARGS, "Returns the global time dilation for world."
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
    // AUDIO SOURCE
    {
        "audio_source_set_pitch", (PyCFunction) cre_py_api_audio_source_set_pitch,
        METH_VARARGS | METH_KEYWORDS, "Sets the pitch for an audio source."
    },
    {
        "audio_source_get_pitch", (PyCFunction) cre_py_api_audio_source_get_pitch,
        METH_VARARGS | METH_KEYWORDS, "Gets the pitch for an audio source."
    },
    // GAME PROPERTIES
    {
        "game_properties_get", cre_py_api_game_properties_get,
        METH_VARARGS, "returns GameProperties(game_title, res_w, res_h, window_w, window_h, target_fps, initial_scene_path, are_colliders_visible)."
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
        "node_is_queued_for_deletion", (PyCFunction) cre_py_api_node_is_queued_for_deletion,
        METH_VARARGS | METH_KEYWORDS, "Returns whether a node is queued for deletion."
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
    {
        "node_get_name", (PyCFunction) cre_py_api_node_get_name,
        METH_VARARGS | METH_KEYWORDS, "Returns the node's name."
    },
    {
        "node_set_time_dilation", (PyCFunction) cre_py_api_node_set_time_dilation,
        METH_VARARGS | METH_KEYWORDS, "Sets a node's time dilation."
    },
    {
        "node_get_time_dilation", (PyCFunction) cre_py_api_node_get_time_dilation,
        METH_VARARGS | METH_KEYWORDS, "Returns a node's time dilation."
    },
    {
        "node_get_full_time_dilation", (PyCFunction) cre_py_api_node_get_full_time_dilation,
        METH_VARARGS | METH_KEYWORDS, "Returns a node's total time dilation, including global world and parents."
    },
    {
        "node_get_full_time_dilation_with_physics_delta", (PyCFunction) cre_py_api_node_get_full_time_dilation_with_physics_delta,
        METH_VARARGS | METH_KEYWORDS, "Returns a node's total time dilation, including global world, parents, and physics delta time."
    },
    {
        "node_create_event", (PyCFunction) cre_py_api_node_create_event,
        METH_VARARGS | METH_KEYWORDS, "Creates an event to be observed by others."
    },
    {
        "node_subscribe_to_event", (PyCFunction) cre_py_api_node_subscribe_to_event,
        METH_VARARGS | METH_KEYWORDS, "Subscribe to an existing event."
    },
    {
        "node_broadcast_event", (PyCFunction) cre_py_api_node_broadcast_event,
        METH_VARARGS | METH_KEYWORDS, "Broadcast an existing event to observers."
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
    {
        "node2D_set_z_index", (PyCFunction) cre_py_api_node2D_set_z_index,
        METH_VARARGS | METH_KEYWORDS, "Set the node's z index."
    },
    {
        "node2D_get_z_index", (PyCFunction) cre_py_api_node2D_get_z_index,
        METH_VARARGS | METH_KEYWORDS, "Get the node's z index."
    },
    {
        "node2D_set_ignore_camera", (PyCFunction) cre_py_api_node2D_set_ignore_camera,
        METH_VARARGS | METH_KEYWORDS, "Sets the value of ignore camera."
    },
    {
        "node2D_get_ignore_camera", (PyCFunction) cre_py_api_node2D_get_ignore_camera,
        METH_VARARGS | METH_KEYWORDS, "Gets the value of ignore camera."
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
    {
        "sprite_set_flip_h", (PyCFunction) cre_py_api_sprite_set_flip_h,
        METH_VARARGS | METH_KEYWORDS, "Sets the sprite's flip h."
    },
    {
        "sprite_get_flip_h", (PyCFunction) cre_py_api_sprite_get_flip_h,
        METH_VARARGS | METH_KEYWORDS, "Gets the sprite's flip h."
    },
    {
        "sprite_set_flip_v", (PyCFunction) cre_py_api_sprite_set_flip_v,
        METH_VARARGS | METH_KEYWORDS, "Sets the sprite's flip v."
    },
    {
        "sprite_get_flip_v", (PyCFunction) cre_py_api_sprite_get_flip_v,
        METH_VARARGS | METH_KEYWORDS, "Gets the sprite's flip v."
    },
    {
        "sprite_set_modulate", (PyCFunction) cre_py_api_sprite_set_modulate,
        METH_VARARGS | METH_KEYWORDS, "Sets the sprite's modulate."
    },
    {
        "sprite_get_modulate", (PyCFunction) cre_py_api_sprite_get_modulate,
        METH_VARARGS | METH_KEYWORDS, "Gets the sprite's modulate."
    },
    {
        "sprite_set_shader_instance", (PyCFunction) cre_py_api_sprite_set_shader_instance,
        METH_VARARGS | METH_KEYWORDS, "Sets the shader instance for the sprite."
    },
    {
        "sprite_get_shader_instance", (PyCFunction) cre_py_api_sprite_get_shader_instance,
        METH_VARARGS | METH_KEYWORDS, "Gets the shader instance for the sprite."
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
    {
        "animated_sprite_add_animation", (PyCFunction) cre_py_api_animated_sprite_add_animation,
        METH_VARARGS | METH_KEYWORDS, "Adds a new animation."
    },
    {
        "animated_sprite_set_flip_h", (PyCFunction) cre_py_api_animated_sprite_set_flip_h,
        METH_VARARGS | METH_KEYWORDS, "Sets the animated sprite's flip h."
    },
    {
        "animated_sprite_get_flip_h", (PyCFunction) cre_py_api_animated_sprite_get_flip_h,
        METH_VARARGS | METH_KEYWORDS, "Gets the animated sprite's flip h."
    },
    {
        "animated_sprite_set_flip_v", (PyCFunction) cre_py_api_animated_sprite_set_flip_v,
        METH_VARARGS | METH_KEYWORDS, "Sets the animated sprite's flip v."
    },
    {
        "animated_sprite_get_flip_v", (PyCFunction) cre_py_api_animated_sprite_get_flip_v,
        METH_VARARGS | METH_KEYWORDS, "Gets the animated sprite's flip v."
    },
    {
        "animated_sprite_set_modulate", (PyCFunction) cre_py_api_animated_sprite_set_modulate,
        METH_VARARGS | METH_KEYWORDS, "Sets the animated sprite's modulate."
    },
    {
        "animated_sprite_get_modulate", (PyCFunction) cre_py_api_animated_sprite_get_modulate,
        METH_VARARGS | METH_KEYWORDS, "Gets the animated sprite's modulate."
    },
    {
        "animated_sprite_set_shader_instance", (PyCFunction) cre_py_api_animated_sprite_set_shader_instance,
        METH_VARARGS | METH_KEYWORDS, "Sets the shader instance for the animated sprite."
    },
    {
        "animated_sprite_get_shader_instance", (PyCFunction) cre_py_api_animated_sprite_get_shader_instance,
        METH_VARARGS | METH_KEYWORDS, "Gets the shader instance for the animated sprite."
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
    {
        "text_label_set_font_uid", (PyCFunction) cre_py_api_text_label_set_font_uid,
        METH_VARARGS | METH_KEYWORDS, "Sets the text label's font uid."
    },
    {
        "text_label_get_font_uid", (PyCFunction) cre_py_api_text_label_get_font_uid,
        METH_VARARGS | METH_KEYWORDS, "Gets the text label's font uid."
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
    // Collision Handler
    {
        "collision_handler_process_collisions", (PyCFunction) cre_py_api_collision_handler_process_collisions,
        METH_VARARGS | METH_KEYWORDS, "Returns collided entities."
    },
    {
        "collision_handler_process_mouse_collisions", (PyCFunction) cre_py_api_collision_handler_process_mouse_collisions,
        METH_VARARGS | METH_KEYWORDS, "Returns entities that collide with the mouse."
    },
    // Game Config
    {
        "game_config_save", (PyCFunction) cre_py_api_game_config_save,
        METH_VARARGS | METH_KEYWORDS, "Saves a game config."
    },
    {
        "game_config_load", (PyCFunction) cre_py_api_game_config_load,
        METH_VARARGS | METH_KEYWORDS, "Loads a game config."
    },
    // PACKED SCENE
    {
        "packed_scene_create_instance", (PyCFunction) cre_py_api_packed_scene_create_instance,
        METH_VARARGS | METH_KEYWORDS, "Creates a node instance from a packed scene."
    },
    // SCENE UTIL
    {
        "scene_util_load_scene", (PyCFunction) cre_py_api_scene_util_load_scene,
        METH_VARARGS | METH_KEYWORDS, "Loads a packed scene by path."
    },
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
static char* crePyApiGenericSetEntityFlipHKWList[] = {"entity_id", "flip_h", NULL};
static char* crePyApiGenericSetEntityFlipVKWList[] = {"entity_id", "flip_v", NULL};
static char* crePyApiGenericSetShaderInstanceKWList[] = {"entity_id", "shader_instance_id", NULL};

static char* crePyApiEngineExitKWList[] = {"code", NULL};
static char* crePyApiEngineSetTargetFPSKWList[] = {"fps", NULL};

static char* crePyApiCurveFloatDeleteKWList[] = {"curve_id", NULL};
static char* crePyApiCurveFloatLoadFromFileKWList[] = {"curve_id", "file_path", NULL};
static char* crePyApiCurveFloatAddPointKWList[] = {"curve_id", "x", "y", "tangent_in", "tangent_out", NULL};
static char* crePyApiCurveFloatRemovePointKWList[] = {"curve_id", "x", "y", NULL};
static char* crePyApiCurveFloatEvalKWList[] = {"curve_id", "t", NULL};

static char* crePyApiGenericShaderIdKWList[] = {"shader_id", NULL};

static char* crePyApiShaderInstanceCreateParamKWList[] = {"shader_id", "name", "initial_value", NULL};
static char* crePyApiShaderInstanceSetParamKWList[] = {"shader_id", "name", "value", NULL};
static char* crePyApiShaderInstanceGetParamKWList[] = {"shader_id", "name", NULL};
static char* crePyApiShaderInstanceCreateFloat2ParamKWList[] = {"shader_id", "name", "initial_value_x", "initial_value_y", NULL};
static char* crePyApiShaderInstanceSetFloat2ParamKWList[] = {"shader_id", "name", "value_x", "value_y", NULL};
static char* crePyApiShaderInstanceCreateFloat3ParamKWList[] = {"shader_id", "name", "initial_value_x", "initial_value_y", "initial_value_z", NULL};
static char* crePyApiShaderInstanceSetFloat3ParamKWList[] = {"shader_id", "name", "value_x", "value_y", "value_z", NULL};
static char* crePyApiShaderInstanceCreateFloat4ParamKWList[] = {"shader_id", "name", "initial_value_x", "initial_value_y", "initial_value_z", "initial_value_w", NULL};
static char* crePyApiShaderInstanceSetFloat4ParamKWList[] = {"shader_id", "name", "value_x", "value_y", "value_z", "value_w", NULL};

static char* crePyApiShaderUtilCompileShaderKWList[] = {"shader_path", NULL};
static char* crePyApiShaderUtilCompileShaderRawKWList[] = {"vertex_path", "fragment_path", NULL};

static char* crePyApiInputAddActionKWList[] = {"name", "value", "device_id", NULL};
static char* crePyApiInputActionInputCheckKWList[] = {"name", NULL};

static char* crePyApiAudioManagerPlaySoundKWList[] = {"path", "loops", NULL};

static char* crePyApiAudioSourceSetPitchKWList[] = {"path", "pitch", NULL};

static char* crePyApiNodeNewKWList[] = {"class_path", "class_name", "node_type", NULL};
static char* crePyApiNodeAddChildKWList[] = {"parent_entity_id", "child_entity_id", NULL};
static char* crePyApiNodeGetChildKWList[] = {"entity_id", "child_name", NULL};
static char* crePyApiNodeSetTimeDilationKWList[] = {"entity_id", "time_dilation", NULL};
static char* crePyApiNodeCreateEventKWList[] = {"entity_id", "event_id", NULL};
static char* crePyApiNodeSubscribeToEventKWList[] = {"entity_id", "event_id", "scoped_entity_id", "callback_func", NULL};
static char* crePyApiNodeBroadcastEventKWList[] = {"entity_id", "event_id", "args", NULL};

static char* crePyApiNode2DSetXYKWList[] = {"entity_id", "x", "y", NULL};
static char* crePyApiNode2DSetRotationKWList[] = {"entity_id", "rotation", NULL};
static char* crePyApiNode2DSetZIndexKWList[] = {"entity_id", "z_index", NULL};
static char* crePyApiNode2DSetIgnoreCameraKWList[] = {"entity_id", "ignore_camera", NULL};

static char* crePyApiSpriteSetTextureKWList[] = {"entity_id", "file_path", NULL};

static char* crePyApiAnimatedSpriteSetAnimationKWList[] = {"entity_id", "name", NULL};
static char* crePyApiAnimatedSpriteAddAnimationKWList[] = {"entity_id", "name", "speed", "loops", "frames", NULL};

static char* crePyApiTextLabelSetTextKWList[] = {"entity_id", "text", NULL};
static char* crePyApiTextLabelSetFontUIDKWList[] = {"entity_id", "uid", NULL};

static char* crePyApiNetworkSendMessageKWList[] = {"message", NULL};
static char* crePyApiNetworkSubscribeKWList[] = {"signal_id", "listener_node", "listener_func", NULL};

static char* crePyApiServerStartKWList[] = {"port", NULL};

static char* crePyApiClientStartKWList[] = {"host", "port", NULL};

static char* crePyApiWorldSetTimeDilationKWList[] = {"time_dilation", NULL};

static char* crePyApiCollisionHandlerProcessMouseCollisionsKWList[] = {"pos_offset_x", "pos_offset_y", "collision_size_w", "collision_size_h", NULL};

static char* crePyApiPackedSceneCreateInstanceKWList[] = {"scene_cache_id", NULL};

static char* crePyApiGameConfigSaveKWList[] = {"path", "data_json", "encryption_key", NULL};
static char* crePyApiGameConfigLoadKWList[] = {"path", "encryption_key", NULL};

// --- Module Init --- //
PyObject* PyInit_cre_py_API(void);
