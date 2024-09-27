#include "pkpy_api.h"

#include <pocketpy.h>

#include "pkpy_api_impl.h"
#include "../pkpy_util.h"
#include "../cre_pkpy.h"
#include "../cre_pkpy_api_source.h"

void cre_pkpy_api_load_internal_modules() {
    // Load crescent internal first
    cre_pkpy_util_create_module(&(CrePPModule) {
        .name = "crescent_internal",
        .functions = {
            // Shader Instance
            {.signature = "shader_instance_delete(shader_id: int) -> bool", .function = cre_pkpy_api_shader_instance_delete},
            {.signature = "shader_instance_create_bool_param(shader_id: int, initial_value: bool) -> None", .function = cre_pkpy_api_shader_instance_create_bool_param},
            {.signature = "shader_instance_set_bool_param(shader_id: int, name: str, value: bool) -> None", .function = cre_pkpy_api_shader_instance_set_bool_param},
            {.signature = "shader_instance_get_bool_param(shader_id: int, name: str) -> bool", .function = cre_pkpy_api_shader_instance_get_bool_param},
            {.signature = "shader_instance_create_int_param(shader_id: int, initial_value: int) -> None", .function = cre_pkpy_api_shader_instance_create_int_param},
            {.signature = "shader_instance_set_int_param(shader_id: int, name: str, value: int) -> None", .function = cre_pkpy_api_shader_instance_set_int_param},
            {.signature = "shader_instance_get_int_param(shader_id: int, name: str) -> int", .function = cre_pkpy_api_shader_instance_get_int_param},
            {.signature = "shader_instance_create_float_param(shader_id: int, initial_value: float) -> None", .function = cre_pkpy_api_shader_instance_create_float_param},
            {.signature = "shader_instance_set_float_param(shader_id: int, name: str, value: float) -> None", .function = cre_pkpy_api_shader_instance_set_float_param},
            {.signature = "shader_instance_get_float_param(shader_id: int, name: str) -> float", .function = cre_pkpy_api_shader_instance_get_float_param},
            {.signature = "shader_instance_create_float2_param(shader_id: int, initial_value_x: float, initial_value_y: float) -> None", .function = cre_pkpy_api_shader_instance_create_float2_param},
            {.signature = "shader_instance_set_float2_param(shader_id: int, name: str, value_x: float, value_y: float) -> None", .function = cre_pkpy_api_shader_instance_set_float2_param},
            {.signature = "shader_instance_get_float2_param(shader_id: int, name: str) -> Tuple[float, float]", .function = cre_pkpy_api_shader_instance_get_float2_param},
            {.signature = "shader_instance_create_float3_param(shader_id: int, initial_value_x: float, initial_value_y: float, initial_value_z: float) -> None", .function = cre_pkpy_api_shader_instance_create_float3_param},
            {.signature = "shader_instance_set_float3_param(shader_id: int, name: str, value_x: float, value_y: float, value_z: float) -> None", .function = cre_pkpy_api_shader_instance_set_float3_param},
            {.signature = "shader_instance_get_float3_param(shader_id: int, name: str) -> Tuple[float, float, float]", .function = cre_pkpy_api_shader_instance_get_float3_param},
            {.signature = "shader_instance_create_float4_param(shader_id: int, initial_value_x: float, initial_value_y: float, initial_value_z: float, initial_value_w: float) -> None", .function = cre_pkpy_api_shader_instance_create_float4_param},
            {.signature = "shader_instance_set_float4_param(shader_id: int, name: str, value_x: float, value_y: float, value_z: float, value_w: float) -> None", .function = cre_pkpy_api_shader_instance_set_float4_param},
            {.signature = "shader_instance_get_float4_param(shader_id: int, name: str) -> Tuple[float, float, float, float]", .function = cre_pkpy_api_shader_instance_get_float4_param},
            // Shader Util
            {.signature = "shader_util_compile_shader(shader_path: str) -> int", .function = cre_pkpy_api_shader_util_compile_shader},
            {.signature = "shader_util_compile_shader_raw(vertex_path, fragment_path) -> int", .function = cre_pkpy_api_shader_util_compile_shader_raw},
            {.signature = "shader_util_set_screen_shader(shader_path: str) -> bool", .function = cre_pkpy_api_shader_util_set_screen_shader},
            {.signature = "shader_util_get_current_screen_shader() -> int", .function = cre_pkpy_api_shader_util_get_current_screen_shader},
            {.signature = "shader_util_reset_screen_shader_to_default() -> None", .function = cre_pkpy_api_shader_util_reset_screen_shader_to_default},
            // Engine
            {.signature = "engine_exit(code: int) -> None", .function = cre_pkpy_api_engine_exit},
            {.signature = "engine_set_target_fps(fps: int) -> None", .function = cre_pkpy_api_engine_set_target_fps},
            {.signature = "engine_get_target_fps() -> int", .function = cre_pkpy_api_engine_get_target_fps},
            {.signature = "engine_get_average_fps() -> int", .function = cre_pkpy_api_engine_get_average_fps},
            {.signature = "engine_set_fps_display_enabled(enabled: bool, font_uid: str, position_x: float, position_y: float) -> None", .function = cre_pkpy_api_engine_set_fps_display_enabled},
            {.signature = "engine_get_global_physics_delta_time() -> float", .function = cre_pkpy_api_engine_get_global_physics_delta_time},
            // Input
            {.signature = "input_is_key_pressed(key: int) -> bool", .function = cre_pkpy_api_input_is_key_pressed},
            {.signature = "input_is_key_just_pressed(key: int) -> bool", .function = cre_pkpy_api_input_is_key_just_pressed},
            {.signature = "input_is_key_just_released(key: int) -> bool", .function = cre_pkpy_api_input_is_key_just_released},
            {.signature = "input_add_action(name: str, value_key: int, device_id: int) -> None", .function = cre_pkpy_api_input_add_action},
            {.signature = "input_is_action_pressed(name: str) -> bool", .function = cre_pkpy_api_input_is_action_pressed},
            {.signature = "input_is_action_just_pressed(name: str) -> bool", .function = cre_pkpy_api_input_is_action_just_pressed},
            {.signature = "input_is_action_just_released(name: str) -> bool", .function = cre_pkpy_api_input_is_action_just_released},
            {.signature = "input_start_gamepad_vibration(device_id: int, weak_magnitude: float, strong_magnitude: float, duration: float) -> None", .function = cre_pkpy_api_input_start_gamepad_vibration},
            {.signature = "input_stop_gamepad_vibration(device_id: int) -> None", .function = cre_pkpy_api_input_stop_gamepad_vibration},
            {.signature = "input_mouse_get_position() -> Tuple[float, float]", .function = cre_pkpy_api_input_mouse_get_position},
            {.signature = "input_mouse_get_world_position() -> Tuple[float, float]", .function = cre_pkpy_api_input_mouse_get_world_position},
            // Node
            {.signature = "node_new(class_path: str, class_name: str, node_type_flag: int) -> \"Node\"", .function = cre_pkpy_api_node_new},
            {.signature = "node_get_name(entity_id: int) -> str", .function = cre_pkpy_api_node_get_name},
            {.signature = "node_add_child(parent_entity_id: int, child_entity_id: int) -> None", .function = cre_pkpy_api_node_add_child},
            {.signature = "node_get_child(parent_entity_id: int, child_entity_name: str) -> Optional[\"Node\"]", .function = cre_pkpy_api_node_get_child},
            {.signature = "node_get_children(entity_id: int) -> Tuple[\"Node\", ...]", .function = cre_pkpy_api_node_get_children},
            {.signature = "node_get_parent(child_entity_id: int) -> Optional[\"Node\"]", .function = cre_pkpy_api_node_get_parent},
            {.signature = "node_queue_deletion(entity_id: int) -> None", .function = cre_pkpy_api_node_queue_deletion},
            {.signature = "node_is_queued_for_deletion(entity_id: int) -> bool", .function = cre_pkpy_api_node_is_queued_for_deletion},
            {.signature = "node_set_time_dilation(entity_id: int, dilation: float) -> None", .function = cre_pkpy_api_node_set_time_dilation},
            {.signature = "node_get_time_dilation(entity_id: int) -> float", .function = cre_pkpy_api_node_get_time_dilation},
            {.signature = "node_get_total_time_dilation(entity_id: int) -> float", .function = cre_pkpy_api_node_get_total_time_dilation},
            // Node2D
            {.signature = "node2d_set_position(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_node2d_set_position},
            {.signature = "node2d_add_to_position(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_node2d_add_to_position},
            {.signature = "node2d_get_position(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_node2d_get_position},
            {.signature = "node2d_get_global_position(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_node2d_get_global_position},
            {.signature = "node2d_set_scale(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_node2d_set_scale},
            {.signature = "node2d_add_to_scale(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_node2d_add_to_scale},
            {.signature = "node2d_get_scale(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_node2d_get_scale},
            {.signature = "node2d_set_rotation(entity_id: int, rotation: float) -> None", .function = cre_pkpy_api_node2d_set_rotation},
            {.signature = "node2d_add_to_rotation(entity_id: int, rotation: float) -> None", .function = cre_pkpy_api_node2d_add_to_rotation},
            {.signature = "node2d_get_rotation(entity_id: int) -> float", .function = cre_pkpy_api_node2d_get_rotation},
            {.signature = "node2d_set_z_index(entity_id: int, z_index: int) -> None", .function = cre_pkpy_api_node2d_set_z_index},
            {.signature = "node2d_get_z_index(entity_id: int) -> int", .function = cre_pkpy_api_node2d_get_z_index},
            {.signature = "node2d_set_z_index_relative_to_parent(entity_id: int, is_relative: bool) -> None", .function = cre_pkpy_api_node2d_set_z_index_relative_to_parent},
            {.signature = "node2d_get_z_index_relative_to_parent(entity_id: int) -> bool", .function = cre_pkpy_api_node2d_get_z_index_relative_to_parent},
            {.signature = "node2d_set_ignore_camera(entity_id: int, ignore_camera: bool) -> None", .function = cre_pkpy_api_node2d_set_ignore_camera},
            {.signature = "node2d_get_ignore_camera(entity_id: int) -> bool", .function = cre_pkpy_api_node2d_get_ignore_camera},
            // Sprite
            {.signature = "sprite_get_texture(entity_id: int) -> Tuple[str, str, str, bool]", .function = cre_pkpy_api_sprite_get_texture},
            {.signature = "sprite_set_texture(entity_id: int, file_path: str) -> None", .function = cre_pkpy_api_sprite_set_texture},
            {.signature = "sprite_get_draw_source(entity_id: int) -> Tuple[float, float, float, float]", .function = cre_pkpy_api_sprite_get_draw_source},
            {.signature = "sprite_set_draw_source(entity_id: int, x: float, y: float, w: float, h: float) -> None", .function = cre_pkpy_api_sprite_set_draw_source},
            {.signature = "sprite_get_flip_h(entity_id: int) -> bool", .function = cre_pkpy_api_sprite_get_flip_h},
            {.signature = "sprite_set_flip_h(entity_id: int, flip_h: bool) -> None", .function = cre_pkpy_api_sprite_set_flip_h},
            {.signature = "sprite_get_flip_v(entity_id: int) -> bool", .function = cre_pkpy_api_sprite_get_flip_v},
            {.signature = "sprite_set_flip_v(entity_id: int, flip_v: bool) -> None", .function = cre_pkpy_api_sprite_set_flip_v},
            {.signature = "sprite_get_modulate(entity_id: int) -> Tuple[int, int, int, int]", .function = cre_pkpy_api_sprite_get_modulate},
            {.signature = "sprite_set_modulate(entity_id: int, r: int, g: int, b: int, a: int) -> None", .function = cre_pkpy_api_sprite_set_modulate},
            {.signature = "sprite_get_origin(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_sprite_get_origin},
            {.signature = "sprite_set_origin(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_sprite_set_origin},
            {.signature = "sprite_get_shader_instance(entity_id: int) -> int", .function = cre_pkpy_api_sprite_get_shader_instance},
            {.signature = "sprite_set_shader_instance(entity_id: int, shader_instance_id: int) -> None", .function = cre_pkpy_api_sprite_set_shader_instance},
            // Animated Sprite
            {.signature = "animated_sprite_play(entity_id: int, animation_name: str) -> bool", .function = cre_pkpy_api_animated_sprite_play},
            {.signature = "animated_sprite_stop(entity_id: int) -> None", .function = cre_pkpy_api_animated_sprite_stop},
            {.signature = "animated_sprite_set_current_animation_frame(entity_id: int, frame: int) -> None", .function = cre_pkpy_api_animated_sprite_set_current_animation_frame},
            {.signature = "animated_sprite_add_animation(entity_id: int, name: str, speed: float, does_loop: bool, frame_count: int, *args) -> None", .function = cre_pkpy_api_animated_sprite_add_animation},
            {.signature = "animated_sprite_get_stagger_animation_start_times(entity_id: int) -> bool", .function = cre_pkpy_api_animated_sprite_get_stagger_animation_start_times},
            {.signature = "animated_sprite_set_stagger_animation_start_times(entity_id: int, stagger: bool) -> None", .function = cre_pkpy_api_animated_sprite_set_stagger_animation_start_times},
            {.signature = "animated_sprite_get_flip_h(entity_id: int) -> bool", .function = cre_pkpy_api_animated_sprite_get_flip_h},
            {.signature = "animated_sprite_set_flip_h(entity_id: int, flip_h: bool) -> None", .function = cre_pkpy_api_animated_sprite_set_flip_h},
            {.signature = "animated_sprite_get_flip_v(entity_id: int) -> bool", .function = cre_pkpy_api_animated_sprite_get_flip_v},
            {.signature = "animated_sprite_set_flip_v(entity_id: int, flip_v: bool) -> None", .function = cre_pkpy_api_animated_sprite_set_flip_v},
            {.signature = "animated_sprite_get_modulate(entity_id: int) -> Tuple[int, int, int, int]", .function = cre_pkpy_api_animated_sprite_get_modulate},
            {.signature = "animated_sprite_set_modulate(entity_id: int, r: int, g: int, b: int, a: int) -> None", .function = cre_pkpy_api_animated_sprite_set_modulate},
            {.signature = "animated_sprite_get_origin(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_animated_sprite_get_origin},
            {.signature = "animated_sprite_set_origin(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_animated_sprite_set_origin},
            {.signature = "animated_sprite_get_shader_instance(entity_id: int) -> int", .function = cre_pkpy_api_animated_sprite_get_shader_instance},
            {.signature = "animated_sprite_set_shader_instance(entity_id: int, shader_instance_id: int) -> None", .function = cre_pkpy_api_animated_sprite_set_shader_instance},
            // Text Label
            {.signature = "text_label_get_text(entity_id: int) -> str", .function = cre_pkpy_api_text_label_get_text},
            {.signature = "text_label_set_text(entity_id: int, text: str) -> None", .function = cre_pkpy_api_text_label_set_text},
            {.signature = "text_label_get_color(entity_id: int) -> Tuple[int, int, int, int]", .function = cre_pkpy_api_text_label_get_color},
            {.signature = "text_label_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None", .function = cre_pkpy_api_text_label_set_color},
            {.signature = "text_label_set_font_uid(entity_id: int, uid: str) -> None", .function = cre_pkpy_api_text_label_set_font_uid},
            // Collider2D
            {.signature = "collider2d_get_extents(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_collider2d_get_extents},
            {.signature = "collider2d_set_extents(entity_id: int, w: float, h: float)", .function = cre_pkpy_api_collider2d_set_extents},
            {.signature = "collider2d_get_color(entity_id: int) -> Tuple[int, int, int, int]", .function = cre_pkpy_api_collider2d_get_color},
            {.signature = "collider2d_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None", .function = cre_pkpy_api_collider2d_set_color},
            // Color Rect
            {.signature = "color_rect_get_size(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_color_rect_get_size},
            {.signature = "color_rect_set_size(entity_id: int, w: float, h: float) -> None", .function = cre_pkpy_api_color_rect_set_size},
            {.signature = "color_rect_get_color(entity_id: int) -> Tuple[int, int, int, int]", .function = cre_pkpy_api_color_rect_get_color},
            {.signature = "color_rect_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None", .function = cre_pkpy_api_color_rect_set_color},
            // Parallax
            {.signature = "parallax_get_scroll_speed(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_parallax_get_scroll_speed},
            {.signature = "parallax_set_scroll_speed(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_parallax_set_scroll_speed},
            // Particles2D
            {.signature = "particles2d_get_amount(entity_id: int) -> int", .function = cre_pkpy_api_particles2d_get_amount},
            {.signature = "particles2d_set_amount(entity_id: int, amount: int) -> None", .function = cre_pkpy_api_particles2d_set_amount},
            {.signature = "particles2d_get_life_time(entity_id: int) -> float", .function = cre_pkpy_api_particles2d_get_life_time},
            {.signature = "particles2d_set_life_time(entity_id: int, life_time: float) -> None", .function = cre_pkpy_api_particles2d_set_life_time},
            {.signature = "particles2d_set_damping(entity_id: int, damping: float) -> None", .function = cre_pkpy_api_particles2d_set_damping},
            {.signature = "particles2d_get_damping(entity_id: int) -> float", .function = cre_pkpy_api_particles2d_get_damping},
            {.signature = "particles2d_set_explosiveness(entity_id: int, explosiveness: float) -> None", .function = cre_pkpy_api_particles2d_set_explosiveness},
            {.signature = "particles2d_get_explosiveness(entity_id: int) -> float", .function = cre_pkpy_api_particles2d_get_explosiveness},
            {.signature = "particles2d_get_color(entity_id: int) -> Tuple[int, int, int, int]", .function = cre_pkpy_api_particles2d_get_color},
            {.signature = "particles2d_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None", .function = cre_pkpy_api_particles2d_set_color},
            {.signature = "particles2d_get_initial_velocity(entity_id: int) -> Tuple[float, float, float, float]", .function = cre_pkpy_api_particles2d_get_initial_velocity},
            {.signature = "particles2d_set_initial_velocity(entity_id: int, min_x: float, min_y: float, max_x: float, max_y: float) -> None", .function = cre_pkpy_api_particles2d_set_initial_velocity},
            {.signature = "particles2d_get_spread(entity_id: int) -> float", .function = cre_pkpy_api_particles2d_get_spread},
            {.signature = "particles2d_set_spread(entity_id: int, spread: float) -> None", .function = cre_pkpy_api_particles2d_set_spread},
            // Scene Tree
            {.signature = "scene_tree_change_scene(path: str) -> None", .function = cre_pkpy_api_scene_tree_change_scene},
            {.signature = "scene_tree_get_root()", .function = cre_pkpy_api_scene_tree_get_root},
            // Scene Manager
            {.signature = "_scene_manager_process_queued_creation_entities() -> None", .function = cre_pkpy_api_scene_manager_process_queued_creation_entities},
            {.signature = "_scene_manager_process_queued_scene_change() -> None", .function = cre_pkpy_api_scene_manager_process_queued_scene_change},
            // Game Properties
            {.signature = "game_properties_get() -> Tuple[str, int, int, int, int, int, str, bool]", .function = cre_pkpy_api_game_properties_get},
            // Camera2D
            {.signature = "camera2d_set_position(x: float, y: float) -> None", .function = cre_pkpy_api_camera2d_set_position},
            {.signature = "camera2d_add_to_position(x: float, y: float) -> None", .function = cre_pkpy_api_camera2d_add_to_position},
            {.signature = "camera2d_get_position() -> Tuple[float, float]", .function = cre_pkpy_api_camera2d_get_position},
            {.signature = "camera2d_set_offset(x: float, y: float) -> None", .function = cre_pkpy_api_camera2d_set_offset},
            {.signature = "camera2d_add_to_offset(x: float, y: float) -> None", .function = cre_pkpy_api_camera2d_add_to_offset},
            {.signature = "camera2d_get_offset() -> Tuple[float, float]", .function = cre_pkpy_api_camera2d_get_offset},
            {.signature = "camera2d_set_zoom(x: float, y: float) -> None", .function = cre_pkpy_api_camera2d_set_zoom},
            {.signature = "camera2d_add_to_zoom(x: float, y: float) -> None", .function = cre_pkpy_api_camera2d_add_to_zoom},
            {.signature = "camera2d_get_zoom() -> Tuple[float, float]", .function = cre_pkpy_api_camera2d_get_zoom},
            {.signature = "camera2d_set_boundary(x: float, y: float, w: float, h: float) -> None", .function = cre_pkpy_api_camera2d_set_boundary},
            {.signature = "camera2d_get_boundary() -> Tuple[float, float, float, float]", .function = cre_pkpy_api_camera2d_get_boundary},
            {.signature = "camera2d_follow_node(entity_id: int) -> None", .function = cre_pkpy_api_camera2d_follow_node},
            {.signature = "camera2d_unfollow_node(entity_id: int) -> None", .function = cre_pkpy_api_camera2d_unfollow_node},
            // World
            {.signature = "world_set_time_dilation(time_dilation: float) -> None", .function = cre_pkpy_api_world_set_time_dilation},
            {.signature = "world_get_time_dilation() -> float", .function = cre_pkpy_api_world_get_time_dilation},
            {.signature = "world_get_delta_time() -> float", .function = cre_pkpy_api_world_get_delta_time},
            {.signature = "world_get_variable_delta_time() -> float", .function = cre_pkpy_api_world_get_variable_delta_time},
            // Audio Source
            {.signature = "audio_source_set_pitch(path: str, pitch: float) -> None", .function = cre_pkpy_api_audio_source_set_pitch},
            {.signature = "audio_source_get_pitch(path: str) -> float", .function = cre_pkpy_api_audio_source_get_pitch},
            // Audio Manager
            {.signature = "audio_manager_play_sound(path: str, loops: bool) -> None", .function = cre_pkpy_api_audio_manager_play_sound},
            {.signature = "audio_manager_stop_sound(path: str) -> None", .function = cre_pkpy_api_audio_manager_stop_sound},
            // Game Config
            {.signature = "game_config_save(path: str, json_text: str, encryption_key: str) -> bool", .function = cre_pkpy_api_game_config_save},
            {.signature = "game_config_load(path, encryption_key) -> str", .function = cre_pkpy_api_game_config_load},
            // Packed Scene
            {.signature = "packed_scene_create_instance(scene_cache_id: int) -> \"Node\"", .function = cre_pkpy_api_packed_scene_create_instance},
            {.signature = "packed_scene_load(path: str) -> int", .function = cre_pkpy_api_packed_scene_load},
            // Collision Handler
            {.signature = "collision_handler_process_collisions(entity_id: float) -> Tuple[\"Node\", ...]", .function = cre_pkpy_api_collision_handler_process_collisions},
            {.signature = "collision_handler_process_mouse_collisions(pos_offset_x: float, pos_offset_y: float, collision_size_w: float, collision_size_h: float) -> Tuple[\"Node\", ...]", .function = cre_pkpy_api_collision_handler_process_mouse_collisions},
            // Network
            {.signature = "network_is_server() -> bool", .function = cre_pkpy_api_network_is_server},
            // Server
            {.signature = "server_start(port: int) -> None", .function = cre_pkpy_api_server_start},
            {.signature = "server_stop() -> None", .function = cre_pkpy_api_server_stop},
            {.signature = "server_send(message: str) -> None", .function = cre_pkpy_api_server_send},
            // Client
            {.signature = "client_start(host: str, port: int) -> None", .function = cre_pkpy_api_client_start},
            {.signature = "client_stop() -> None", .function = cre_pkpy_api_client_stop},
            {.signature = "client_send(message: str) -> None", .function = cre_pkpy_api_client_send},

            { NULL, NULL },
        }
    });

    // Now load the front facing crecent module
    cre_pkpy_util_create_from_string(CRE_PKPY_MODULE_NAME_CRESCENT, CRE_PKPY_CRESCENT_SOURCE);
}
