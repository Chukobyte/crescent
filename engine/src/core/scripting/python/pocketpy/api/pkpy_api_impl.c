#include "pkpy_api_impl.h"

// Shader Instance
bool cre_pkpy_api_shader_instance_delete(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_create_bool_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_set_bool_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_get_bool_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_create_int_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_set_int_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_get_int_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_create_float_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_set_float_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_get_float_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_create_float2_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_set_float2_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_get_float2_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_create_float3_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_set_float3_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_get_float3_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_create_float4_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_set_float4_param(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_instance_get_float4_param(int argc, py_StackRef argv) { return true; }

// Shader Util
bool cre_pkpy_api_shader_util_compile_shader(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_util_compile_shader_raw(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_util_set_screen_shader(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_util_get_current_screen_shader(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_shader_util_reset_screen_shader_to_default(int argc, py_StackRef argv) { return true; }

// Engine
bool cre_pkpy_api_engine_exit(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_engine_set_target_fps(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_engine_get_target_fps(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_engine_get_average_fps(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_engine_set_fps_display_enabled(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_engine_get_global_physics_delta_time(int argc, py_StackRef argv) { return true; }

// Input
bool cre_pkpy_api_input_is_key_pressed(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_is_key_just_pressed(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_is_key_just_released(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_add_action(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_is_action_pressed(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_is_action_just_pressed(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_is_action_just_released(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_start_gamepad_vibration(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_stop_gamepad_vibration(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_mouse_get_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_input_mouse_get_world_position(int argc, py_StackRef argv) { return true; }

// Scene Tree
bool cre_pkpy_api_scene_tree_change_scene(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_scene_tree_get_root(int argc, py_StackRef argv) { return true; }

// Scene Manager
bool cre_pkpy_api_scene_manager_process_queued_creation_entities(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_scene_manager_process_queued_scene_change(int argc, py_StackRef argv) { return true; }

// Game Properties
bool cre_pkpy_api_game_properties_get(int argc, py_StackRef argv) { return true; }

// Camera2D
bool cre_pkpy_api_camera2d_set_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_add_to_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_set_offset(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_add_to_offset(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_offset(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_set_zoom(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_add_to_zoom(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_zoom(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_set_boundary(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_get_boundary(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_follow_node(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_camera2d_unfollow_node(int argc, py_StackRef argv) { return true; }

// World
bool cre_pkpy_api_world_set_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_world_get_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_world_get_delta_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_world_get_variable_delta_time(int argc, py_StackRef argv) { return true; }

// Audio Source
bool cre_pkpy_api_audio_source_set_pitch(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_audio_source_get_pitch(int argc, py_StackRef argv) { return true; }

// Audio Manager
bool cre_pkpy_api_audio_manager_play_sound(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_audio_manager_stop_sound(int argc, py_StackRef argv) { return true; }

// Game Config
bool cre_pkpy_api_game_config_save(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_game_config_load(int argc, py_StackRef argv) { return true; }

// Packed Scene
bool cre_pkpy_api_packed_scene_create_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_packed_scene_load(int argc, py_StackRef argv) { return true; }

// Collision Handler
bool cre_pkpy_api_collision_handler_process_collisions(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collision_handler_process_mouse_collisions(int argc, py_StackRef argv) { return true; }

// Network
bool cre_pkpy_api_network_is_server(int argc, py_StackRef argv) { return true; }
// Server
bool cre_pkpy_api_server_start(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_server_stop(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_server_send(int argc, py_StackRef argv) { return true; }
// Client
bool cre_pkpy_api_client_start(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_client_stop(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_client_send(int argc, py_StackRef argv) { return true; }

// Node
bool cre_pkpy_api_node_new(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_name(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_add_child(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_child(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_children(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_queue_deletion(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_is_queued_for_deletion(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_set_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_time_dilation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node_get_total_time_dilation(int argc, py_StackRef argv) { return true; }

// Node2D
bool cre_pkpy_api_node2d_set_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_global_position(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_scale(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_add_to_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_rotation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_z_index(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_z_index(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_z_index_relative_to_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_z_index_relative_to_parent(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_set_ignore_camera(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_node2d_get_ignore_camera(int argc, py_StackRef argv) { return true; }

// Sprite
bool cre_pkpy_api_sprite_get_texture(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_texture(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_draw_source(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_draw_source(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_get_shader_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_sprite_set_shader_instance(int argc, py_StackRef argv) { return true; }

// Animated Sprite
bool cre_pkpy_api_animated_sprite_play(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_stop(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_current_animation_frame(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_add_animation(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_stagger_animation_start_times(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_stagger_animation_start_times(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_flip_h(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_flip_v(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_modulate(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_origin(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_get_shader_instance(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_animated_sprite_set_shader_instance(int argc, py_StackRef argv) { return true; }

// Text Label
bool cre_pkpy_api_text_label_get_text(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_text(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_text_label_set_font_uid(int argc, py_StackRef argv) { return true; }

// Collider2D
bool cre_pkpy_api_collider2d_get_extents(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_set_extents(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_collider2d_set_color(int argc, py_StackRef argv) { return true; }

// Color Rect
bool cre_pkpy_api_color_rect_get_size(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_set_size(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_color_rect_set_color(int argc, py_StackRef argv) { return true; }

// Parallax
bool cre_pkpy_api_parallax_get_scroll_speed(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_parallax_set_scroll_speed(int argc, py_StackRef argv) { return true; }

// Particles2D
bool cre_pkpy_api_particles2d_get_amount(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_amount(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_life_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_life_time(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_damping(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_damping(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_explosiveness(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_explosiveness(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_color(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_initial_velocity(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_initial_velocity(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_get_spread(int argc, py_StackRef argv) { return true; }
bool cre_pkpy_api_particles2d_set_spread(int argc, py_StackRef argv) { return true; }
