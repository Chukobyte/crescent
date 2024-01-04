#include "cre_pkpy_api.h"

#include <string.h>

#include <seika/rendering/frame_buffer.h>
#include <seika/rendering/render_context.h>
#include <seika/rendering/renderer.h>
#include <seika/rendering/shader/shader_cache.h>
#include <seika/input/input.h>
#include <seika/input/mouse.h>
#include <seika/audio/audio.h>
#include <seika/audio/audio_manager.h>
#include <seika/asset/asset_manager.h>
#include <seika/asset/asset_file_loader.h>
#include <seika/networking/se_network.h>
#include <seika/utils/se_assert.h>
#include <seika/utils/se_string_util.h>
#include <seika/utils/se_file_system_utils.h>

#include "cre_pkpy_api_node.h"
#include "../cre_pkpy.h"
#include "../cre_pkpy_util.h"
#include "../cre_pkpy_entity_instance_cache.h"
#include "../cre_pkpy_api_source.h"
#include "../cre_pkpy_script_context.h"
#include "../../../../engine_context.h"
#include "../../../../world.h"
#include "../../../../game_properties.h"
#include "../../../../scene/scene_utils.h"
#include "../../../../scene/scene_template_cache.h"
#include "../../../../ecs/ecs_manager.h"
#include "../../../../ecs/system/ec_system.h"
#include "../../../../camera/camera.h"
#include "../../../../camera/camera_manager.h"
#include "../../../../physics/collision/collision.h"

// Shader Instance
int cre_pkpy_api_shader_instance_delete(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_create_bool_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_set_bool_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_get_bool_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_create_int_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_set_int_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_get_int_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_create_float_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_set_float_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_get_float_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_create_float2_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_set_float2_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_get_float2_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_create_float3_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_set_float3_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_get_float3_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_create_float4_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_set_float4_param(pkpy_vm* vm);
int cre_pkpy_api_shader_instance_get_float4_param(pkpy_vm* vm);

// Shader Util
int cre_pkpy_api_shader_util_compile_shader(pkpy_vm* vm);
int cre_pkpy_api_shader_util_compile_shader_raw(pkpy_vm* vm);
int cre_pkpy_api_shader_util_set_screen_shader(pkpy_vm* vm);
int cre_pkpy_api_shader_util_get_current_screen_shader(pkpy_vm* vm);
int cre_pkpy_api_shader_util_reset_screen_shader_to_default(pkpy_vm* vm);

// Engine
int cre_pkpy_api_engine_exit(pkpy_vm* vm);
int cre_pkpy_api_engine_set_target_fps(pkpy_vm* vm);
int cre_pkpy_api_engine_get_target_fps(pkpy_vm* vm);
int cre_pkpy_api_engine_get_average_fps(pkpy_vm* vm);
int cre_pkpy_api_engine_set_fps_display_enabled(pkpy_vm* vm);
int cre_pkpy_api_engine_get_global_physics_delta_time(pkpy_vm* vm);

// Input
int cre_pkpy_api_input_add_action(pkpy_vm* vm);
int cre_pkpy_api_input_is_action_pressed(pkpy_vm* vm);
int cre_pkpy_api_input_is_action_just_pressed(pkpy_vm* vm);
int cre_pkpy_api_input_is_action_just_released(pkpy_vm* vm);
int cre_pkpy_api_input_start_gamepad_vibration(pkpy_vm* vm);
int cre_pkpy_api_input_stop_gamepad_vibration(pkpy_vm* vm);
int cre_pkpy_api_input_mouse_get_position(pkpy_vm* vm);
int cre_pkpy_api_input_mouse_get_world_position(pkpy_vm* vm);

// Scene Tree
int cre_pkpy_api_scene_tree_change_scene(pkpy_vm* vm);
int cre_pkpy_api_scene_tree_get_root(pkpy_vm* vm);

// Scene Manager
int cre_pkpy_api_scene_manager_process_queued_creation_entities(pkpy_vm* vm);
int cre_pkpy_api_scene_manager_process_queued_scene_change(pkpy_vm* vm);

// Game Properties
int cre_pkpy_api_game_properties_get(pkpy_vm* vm);

// Camera2D
int cre_pkpy_api_camera2d_set_position(pkpy_vm* vm);
int cre_pkpy_api_camera2d_add_to_position(pkpy_vm* vm);
int cre_pkpy_api_camera2d_get_position(pkpy_vm* vm);
int cre_pkpy_api_camera2d_set_offset(pkpy_vm* vm);
int cre_pkpy_api_camera2d_add_to_offset(pkpy_vm* vm);
int cre_pkpy_api_camera2d_get_offset(pkpy_vm* vm);
int cre_pkpy_api_camera2d_set_zoom(pkpy_vm* vm);
int cre_pkpy_api_camera2d_add_to_zoom(pkpy_vm* vm);
int cre_pkpy_api_camera2d_get_zoom(pkpy_vm* vm);
int cre_pkpy_api_camera2d_set_boundary(pkpy_vm* vm);
int cre_pkpy_api_camera2d_get_boundary(pkpy_vm* vm);
int cre_pkpy_api_camera2d_follow_node(pkpy_vm* vm);
int cre_pkpy_api_camera2d_unfollow_node(pkpy_vm* vm);

// World
int cre_pkpy_api_world_set_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_world_get_time_dilation(pkpy_vm* vm);
int cre_pkpy_api_world_get_delta_time(pkpy_vm* vm);

// Audio Source
int cre_pkpy_api_audio_source_set_pitch(pkpy_vm* vm);
int cre_pkpy_api_audio_source_get_pitch(pkpy_vm* vm);

// Audio Manager
int cre_pkpy_api_audio_manager_play_sound(pkpy_vm* vm);
int cre_pkpy_api_audio_manager_stop_sound(pkpy_vm* vm);

// Game Config
int cre_pkpy_api_game_config_save(pkpy_vm* vm);
int cre_pkpy_api_game_config_load(pkpy_vm* vm);

// Packed Scene
int cre_pkpy_api_packed_scene_create_instance(pkpy_vm* vm);
int cre_pkpy_api_packed_scene_load(pkpy_vm* vm);

// Collision Handler
int cre_pkpy_api_collision_handler_process_collisions(pkpy_vm* vm);
int cre_pkpy_api_collision_handler_process_mouse_collisions(pkpy_vm* vm);

// Network
int cre_pkpy_api_network_is_server(pkpy_vm* vm);
// Server
int cre_pkpy_api_server_start(pkpy_vm* vm);
int cre_pkpy_api_server_stop(pkpy_vm* vm);
int cre_pkpy_api_server_send(pkpy_vm* vm);
// Client
int cre_pkpy_api_client_start(pkpy_vm* vm);
int cre_pkpy_api_client_stop(pkpy_vm* vm);
int cre_pkpy_api_client_send(pkpy_vm* vm);

void cre_pkpy_api_load_internal_modules(pkpy_vm* vm) {
    // Load internal first
    cre_pkpy_util_create_module(vm, &(CrePPModule) {
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
            {.signature = "input_add_action(name: str, value: str, device_id: int) -> None", .function = cre_pkpy_api_input_add_action},
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
            {.signature = "particles2d_get_color(entity_id: int) -> Tuple[int, int, int, int]", .function = cre_pkpy_api_particles2d_get_color},
            {.signature = "particles2d_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None", .function = cre_pkpy_api_particles2d_set_color},
            {.signature = "particles2d_get_initial_velocity(entity_id: int) -> Tuple[float, float]", .function = cre_pkpy_api_particles2d_get_initial_velocity},
            {.signature = "particles2d_set_initial_velocity(entity_id: int, x: float, y: float) -> None", .function = cre_pkpy_api_particles2d_set_initial_velocity},
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

    // Now load front facing api
    cre_pkpy_util_create_from_string(vm, CRE_PKPY_MODULE_NAME_CRESCENT, CRE_PKPY_CRESCENT_SOURCE);
}

// Helper functions
static inline SKAVector2 cre_pkpy_api_helper_mouse_get_global_position(const SKAVector2* offset) {
    SEMouse* globalMouse = se_mouse_get();
    const CRECamera2D* camera = cre_camera_manager_get_current_camera();
    CREGameProperties* gameProps = cre_game_props_get();
    SERenderContext* renderContext = se_render_context_get();
    const SKAVector2 mouse_pixel_coord = {
            ska_math_map_to_range(globalMouse->position.x, 0.0f, (float) renderContext->windowWidth, 0.0f, (float) gameProps->resolutionWidth),
            ska_math_map_to_range(globalMouse->position.y, 0.0f, (float) renderContext->windowHeight, 0.0f, (float) gameProps->resolutionHeight)
    };
    const SKAVector2 mouseWorldPos = {
            (camera->viewport.x + camera->offset.x + mouse_pixel_coord.x + offset->x) * camera->zoom.x,
            (camera->viewport.y + camera->offset.y + mouse_pixel_coord.y + offset->y) * camera->zoom.y
    };
    return mouseWorldPos;
}

static inline const char* cre_pkpy_api_helper_convert_pkpy_CString(const pkpy_CString* pkpyString) {
#define CONVERTED_STRING_BUFFER_SIZE 2048
    SE_ASSERT_FMT(pkpyString->size <= CONVERTED_STRING_BUFFER_SIZE, "pkpy string is larger than converted string buffer size, consider raising 'CONVERTED_STRING_BUFFER_SIZE'!");
    static char convertedStringBuffer[CONVERTED_STRING_BUFFER_SIZE];
    se_str_trim_by_size(pkpyString->data, convertedStringBuffer, pkpyString->size);
    return convertedStringBuffer;
#undef CONVERTED_STRING_BUFFER_SIZE
}

//--- SHADER INSTANCE ---//

int cre_pkpy_api_shader_instance_delete(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_to_int(vm, 0, &pyShaderId);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    bool hasDeletedInstance = false;
    if (shaderInstance != NULL) {
        se_shader_cache_remove_instance(shaderId);
        se_shader_instance_destroy(shaderInstance);
        hasDeletedInstance = true;
    }
    pkpy_push_bool(vm, hasDeletedInstance);
    return 1;
}

int cre_pkpy_api_shader_instance_create_bool_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    bool value;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_bool(vm, 2, &value);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_create_bool(shaderInstance, paramName, value);
    return 0;
}

int cre_pkpy_api_shader_instance_set_bool_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    bool value;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_bool(vm, 2, &value);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_update_bool(shaderInstance, paramName, value);
    return 0;
}

int cre_pkpy_api_shader_instance_get_bool_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    const bool paramValue = se_shader_instance_param_get_bool(shaderInstance, paramName);
    pkpy_push_bool(vm, paramValue);
    return 1;
}

int cre_pkpy_api_shader_instance_create_int_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    int value;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_int(vm, 2, &value);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_create_int(shaderInstance, paramName, value);
    return 0;
}

int cre_pkpy_api_shader_instance_set_int_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    int value;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_int(vm, 2, &value);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_update_int(shaderInstance, paramName, value);
    return 0;
}

int cre_pkpy_api_shader_instance_get_int_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    const int paramValue = se_shader_instance_param_get_int(shaderInstance, paramName);
    pkpy_push_int(vm, paramValue);
    return 1;
}

int cre_pkpy_api_shader_instance_create_float_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double value;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &value);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_create_float(shaderInstance, paramName, (float)value);
    return 0;
}

int cre_pkpy_api_shader_instance_set_float_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double value;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &value);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_update_float(shaderInstance, paramName, (float)value);
    return 0;
}

int cre_pkpy_api_shader_instance_get_float_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    const float paramValue = se_shader_instance_param_get_float(shaderInstance, paramName);
    pkpy_push_float(vm, paramValue);
    return 1;
}

int cre_pkpy_api_shader_instance_create_float2_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double valueX;
    double valueY;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &valueX);
    pkpy_to_float(vm, 3, &valueY);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_create_float2(shaderInstance, paramName, (SKAVector2){ (float)valueX, (float)valueY });
    return 0;
}

int cre_pkpy_api_shader_instance_set_float2_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double valueX;
    double valueY;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &valueX);
    pkpy_to_float(vm, 3, &valueY);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_update_float2(shaderInstance, paramName, (SKAVector2){ (float)valueX, (float)valueY });
    return 0;
}

int cre_pkpy_api_shader_instance_get_float2_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    const SKAVector2 paramValue = se_shader_instance_param_get_float2(shaderInstance, paramName);
    pkpy_push_float(vm, paramValue.x);
    pkpy_push_float(vm, paramValue.y);
    return 2;
}

int cre_pkpy_api_shader_instance_create_float3_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double valueX;
    double valueY;
    double valueZ;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &valueX);
    pkpy_to_float(vm, 3, &valueY);
    pkpy_to_float(vm, 4, &valueZ);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_create_float3(shaderInstance, paramName, (SKAVector3){ (float)valueX, (float)valueY, (float)valueZ });
    return 0;
}

int cre_pkpy_api_shader_instance_set_float3_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double valueX;
    double valueY;
    double valueZ;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &valueX);
    pkpy_to_float(vm, 3, &valueY);
    pkpy_to_float(vm, 4, &valueZ);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_update_float3(shaderInstance, paramName, (SKAVector3){ (float)valueX, (float)valueY, (float)valueZ });
    return 0;
}

int cre_pkpy_api_shader_instance_get_float3_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    const SKAVector3 paramValue = se_shader_instance_param_get_float3(shaderInstance, paramName);
    pkpy_push_float(vm, paramValue.x);
    pkpy_push_float(vm, paramValue.y);
    pkpy_push_float(vm, paramValue.z);
    return 3;
}

int cre_pkpy_api_shader_instance_create_float4_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double valueX;
    double valueY;
    double valueZ;
    double valueW;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &valueX);
    pkpy_to_float(vm, 3, &valueY);
    pkpy_to_float(vm, 4, &valueZ);
    pkpy_to_float(vm, 5, &valueW);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_create_float4(shaderInstance, paramName, (SKAVector4){ (float)valueX, (float)valueY, (float)valueZ, (float)valueW });
    return 0;
}

int cre_pkpy_api_shader_instance_set_float4_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    double valueX;
    double valueY;
    double valueZ;
    double valueW;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);
    pkpy_to_float(vm, 2, &valueX);
    pkpy_to_float(vm, 3, &valueY);
    pkpy_to_float(vm, 4, &valueZ);
    pkpy_to_float(vm, 5, &valueW);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    se_shader_instance_param_update_float4(shaderInstance, paramName, (SKAVector4){ (float)valueX, (float)valueY, (float)valueZ, (float)valueW });
    return 0;
}

int cre_pkpy_api_shader_instance_get_float4_param(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_CString pyParamName;
    pkpy_to_int(vm, 0, &pyShaderId);
    pkpy_to_string(vm, 1, &pyParamName);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    const char* paramName = cre_pkpy_api_helper_convert_pkpy_CString(&pyParamName);
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    const SKAVector4 paramValue = se_shader_instance_param_get_float4(shaderInstance, paramName);
    pkpy_push_float(vm, paramValue.x);
    pkpy_push_float(vm, paramValue.y);
    pkpy_push_float(vm, paramValue.z);
    pkpy_push_float(vm, paramValue.w);
    return 4;
}

//--- SHADER UTIL ---//

int cre_pkpy_api_shader_util_compile_shader(pkpy_vm* vm) {
    pkpy_CString pyShaderPath;
    pkpy_to_string(vm, 0, &pyShaderPath);

    const char* shaderPath = cre_pkpy_api_helper_convert_pkpy_CString(&pyShaderPath);
    const SEShaderInstanceId newId = se_shader_cache_create_instance_and_add(shaderPath);
    SE_ASSERT_FMT(newId != SE_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from path '%s'", shaderPath);
    pkpy_push_int(vm, (int)newId);
    return 1;
}

int cre_pkpy_api_shader_util_compile_shader_raw(pkpy_vm* vm) {
    pkpy_CString pyVertexPath;
    pkpy_CString pyFragmentPath;
    pkpy_to_string(vm, 0, &pyVertexPath);
    pkpy_to_string(vm, 1, &pyFragmentPath);

    const char* vertexPath = cre_pkpy_api_helper_convert_pkpy_CString(&pyVertexPath);
    const char* fragmentPath = cre_pkpy_api_helper_convert_pkpy_CString(&pyFragmentPath);
    const SEShaderInstanceId newId = se_shader_cache_create_instance_and_add_from_raw(vertexPath, fragmentPath);
    SE_ASSERT_FMT(newId != SE_SHADER_INSTANCE_INVALID_ID, "Invalid shader id reading from paths: vertex = '%s', fragment = '%s'", vertexPath, fragmentPath);
    pkpy_push_int(vm, (int)newId);
    return 1;
}

int cre_pkpy_api_shader_util_set_screen_shader(pkpy_vm* vm) {
    int pyShaderId;
    pkpy_to_int(vm, 0, &pyShaderId);

    const SEShaderInstanceId shaderId = (SEShaderInstanceId)pyShaderId;
    SEShaderInstance* shaderInstance = se_shader_cache_get_instance(shaderId);
    bool hasSetShaderInstance = false;
    if (shaderInstance) {
        se_frame_buffer_set_screen_shader(shaderInstance);
        hasSetShaderInstance = true;
    }
    pkpy_push_bool(vm, hasSetShaderInstance);
    return 1;
}

int cre_pkpy_api_shader_util_get_current_screen_shader(pkpy_vm* vm) {
    const SEShaderInstanceId currentScreenShaderId = 0; // TODO: Keep track of current screen shader somewhere
    pkpy_push_int(vm, (int)currentScreenShaderId);
    return 1;
}

int cre_pkpy_api_shader_util_reset_screen_shader_to_default(pkpy_vm* vm) {
    se_frame_buffer_reset_to_default_screen_shader();
    return 0;
}

//--- ENGINE ---//

int cre_pkpy_api_engine_exit(pkpy_vm* vm) {
    int pyExitCode;
    pkpy_to_int(vm, 0, &pyExitCode);

    CREEngineContext* engineContext = cre_engine_context_get();
    engineContext->isRunning = false;
    engineContext->exitCode = pyExitCode;
    return 0;
}

int cre_pkpy_api_engine_set_target_fps(pkpy_vm* vm) {
    int pyTargetFPS;
    pkpy_to_int(vm, 0, &pyTargetFPS);

    CREEngineContext* engineContext = cre_engine_context_get();
    engineContext->targetFPS = pyTargetFPS;
    return 0;
}

int cre_pkpy_api_engine_get_target_fps(pkpy_vm* vm) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    pkpy_push_int(vm, engineContext->targetFPS);
    return 1;
}

int cre_pkpy_api_engine_get_average_fps(pkpy_vm* vm) {
    const CREEngineContext* engineContext = cre_engine_context_get();
    pkpy_push_int(vm, (int)engineContext->stats.averageFPS);
    return 1;
}

int cre_pkpy_api_engine_set_fps_display_enabled(pkpy_vm* vm) {
    bool pyIsEnabled;
    pkpy_CString pyFontUID;
    double pyPositionX, pyPositionY;
    pkpy_to_bool(vm, 0, &pyIsEnabled);
    pkpy_to_string(vm, 1, &pyFontUID);
    pkpy_to_float(vm, 2, &pyPositionX);
    pkpy_to_float(vm, 3, &pyPositionY);

    const char* potentialFontUID = cre_pkpy_api_helper_convert_pkpy_CString(&pyFontUID);
    cre_ecs_manager_enable_fps_display_entity(
            pyIsEnabled,
            strcmp(potentialFontUID, "") != 0 ? potentialFontUID : NULL,
            (float)pyPositionX,
            (float)pyPositionY
    );
    return 0;
}

int cre_pkpy_api_engine_get_global_physics_delta_time(pkpy_vm* vm) {
    pkpy_push_float(vm, (double)CRE_GLOBAL_PHYSICS_DELTA_TIME);
    return 1;
}

//--- INPUT ---//

int cre_pkpy_api_input_add_action(pkpy_vm* vm) {
    pkpy_CString pyActionName;
    pkpy_CString pyActionValue;
    int pyDeviceId;
    pkpy_to_string(vm, 0, &pyActionName);
    pkpy_to_string(vm, 1, &pyActionValue);
    pkpy_to_int(vm, 2, &pyDeviceId);

    const char* actionName = cre_pkpy_api_helper_convert_pkpy_CString(&pyActionName);
    const char* actionValue = cre_pkpy_api_helper_convert_pkpy_CString(&pyActionValue);
    se_input_add_action_value(actionName, actionValue, pyDeviceId);
    return 0;
}

int cre_pkpy_api_input_is_action_pressed(pkpy_vm* vm) {
    pkpy_CString pyActionName;
    pkpy_to_string(vm, 0, &pyActionName);

    const char* actionName = cre_pkpy_api_helper_convert_pkpy_CString(&pyActionName);
    pkpy_push_bool(vm, se_input_is_action_pressed(actionName));
    return 1;
}

int cre_pkpy_api_input_is_action_just_pressed(pkpy_vm* vm) {
    pkpy_CString pyActionName;
    pkpy_to_string(vm, 0, &pyActionName);

    const char* actionName = cre_pkpy_api_helper_convert_pkpy_CString(&pyActionName);
    pkpy_push_bool(vm, se_input_is_action_just_pressed(actionName));
    return 1;
}

int cre_pkpy_api_input_is_action_just_released(pkpy_vm* vm) {
    pkpy_CString pyActionName;
    pkpy_to_string(vm, 0, &pyActionName);

    const char* actionName = cre_pkpy_api_helper_convert_pkpy_CString(&pyActionName);
    pkpy_push_bool(vm, se_input_is_action_just_released(actionName));
    return 1;
}

int cre_pkpy_api_input_start_gamepad_vibration(pkpy_vm* vm) {
    int pyDeviceId;
    double pyWeakMagnitude;
    double pyStrongMagnitude;
    double pyDurationSeconds;
    pkpy_to_int(vm, 0, &pyDeviceId);
    pkpy_to_float(vm, 1, &pyWeakMagnitude);
    pkpy_to_float(vm, 2, &pyStrongMagnitude);
    pkpy_to_float(vm, 3, &pyDurationSeconds);

    se_input_gamepad_start_vibration(pyDeviceId, (float)pyWeakMagnitude, (float)pyStrongMagnitude, (float)pyDurationSeconds);
    return 0;
}

int cre_pkpy_api_input_stop_gamepad_vibration(pkpy_vm* vm) {
    int pyDeviceId;
    pkpy_to_int(vm, 0, &pyDeviceId);

    se_input_gamepad_stop_vibration(pyDeviceId);
    return 0;
}

int cre_pkpy_api_input_mouse_get_position(pkpy_vm* vm) {
    const SEMouse* globalMouse = se_mouse_get();
    pkpy_push_float(vm, (double)globalMouse->position.x);
    pkpy_push_float(vm, (double)globalMouse->position.y);
    return 2;
}

int cre_pkpy_api_input_mouse_get_world_position(pkpy_vm* vm) {
    const SKAVector2 mouseWorldPosition = cre_pkpy_api_helper_mouse_get_global_position(&(SKAVector2){0.0f, 0.0f });
    pkpy_push_float(vm, (double)mouseWorldPosition.x);
    pkpy_push_float(vm, (double)mouseWorldPosition.y);
    return 2;
}

//--- SCENE TREE ---//

int cre_pkpy_api_scene_tree_change_scene(pkpy_vm* vm) {
    pkpy_CString pyScenePath;
    pkpy_to_string(vm, 0, &pyScenePath);

    const char* scenePath = cre_pkpy_api_helper_convert_pkpy_CString(&pyScenePath);
    cre_scene_manager_queue_scene_change(scenePath);
    return 0;
}

int cre_pkpy_api_scene_tree_get_root(pkpy_vm* vm) {
    SceneTreeNode* rootNode = cre_scene_manager_get_active_scene_root();
    SE_ASSERT(rootNode != NULL);
    cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(rootNode->entity);
    return 1;
}

//--- SCENE MANAGER ---//

int cre_pkpy_api_scene_manager_process_queued_creation_entities(pkpy_vm* vm) {
    cre_scene_manager_process_queued_creation_entities();
    return 0;
}

int cre_pkpy_api_scene_manager_process_queued_scene_change(pkpy_vm* vm) {
    cre_scene_manager_process_queued_scene_change();
    return 0;
}

//--- GAME PROPERTIES ---//

int cre_pkpy_api_game_properties_get(pkpy_vm* vm) {
    const CREGameProperties* gameProps = cre_game_props_get();
    SE_ASSERT(gameProps->gameTitle);
    SE_ASSERT(gameProps->initialScenePath);
    pkpy_push_string(vm, pkpy_string(gameProps->gameTitle));
    pkpy_push_int(vm, gameProps->resolutionWidth);
    pkpy_push_int(vm, gameProps->resolutionHeight);
    pkpy_push_int(vm, gameProps->windowWidth);
    pkpy_push_int(vm, gameProps->windowHeight);
    pkpy_push_int(vm, gameProps->targetFPS);
    pkpy_push_string(vm, pkpy_string(gameProps->initialScenePath));
    pkpy_push_bool(vm, gameProps->areCollidersVisible);
    return 8;
}

//--- CAMERA2D ---//

int cre_pkpy_api_camera2d_set_position(pkpy_vm* vm) {
    double pyPositionX, pyPositionY;
    pkpy_to_float(vm, 0, &pyPositionX);
    pkpy_to_float(vm, 1, &pyPositionY);

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->viewport = (SKAVector2){ (float)pyPositionX, (float)pyPositionY };
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
    return 0;
}

int cre_pkpy_api_camera2d_add_to_position(pkpy_vm* vm) {
    double pyPositionX, pyPositionY;
    pkpy_to_float(vm, 0, &pyPositionX);
    pkpy_to_float(vm, 1, &pyPositionY);

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->viewport = (SKAVector2){  camera2D->viewport.x + (float)pyPositionX, camera2D->viewport.y + (float)pyPositionY };
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
    return 0;
}

int cre_pkpy_api_camera2d_get_position(pkpy_vm* vm) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    pkpy_push_float(vm, (double)camera2D->viewport.x);
    pkpy_push_float(vm, (double)camera2D->viewport.y);
    return 2;
}

int cre_pkpy_api_camera2d_set_offset(pkpy_vm* vm) {
    double pyOffsetX, pyOffsetY;
    pkpy_to_float(vm, 0, &pyOffsetX);
    pkpy_to_float(vm, 1, &pyOffsetY);

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->offset = (SKAVector2){(float)pyOffsetX, (float)pyOffsetY };
    return 0;
}

int cre_pkpy_api_camera2d_add_to_offset(pkpy_vm* vm) {
    double pyOffsetX, pyOffsetY;
    pkpy_to_float(vm, 0, &pyOffsetX);
    pkpy_to_float(vm, 1, &pyOffsetY);

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->offset = (SKAVector2){camera2D->offset.x + (float)pyOffsetX, camera2D->offset.y + (float)pyOffsetY };
    return 0;
}

int cre_pkpy_api_camera2d_get_offset(pkpy_vm* vm) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    pkpy_push_float(vm, (double)camera2D->offset.x);
    pkpy_push_float(vm, (double)camera2D->offset.y);
    return 2;
}

int cre_pkpy_api_camera2d_set_zoom(pkpy_vm* vm) {
    double pyZoomX, pyZoomY;
    pkpy_to_float(vm, 0, &pyZoomX);
    pkpy_to_float(vm, 1, &pyZoomY);

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->zoom = (SKAVector2){(float)pyZoomX, (float)pyZoomY };
    return 0;
}

int cre_pkpy_api_camera2d_add_to_zoom(pkpy_vm* vm) {
    double pyZoomX, pyZoomY;
    pkpy_to_float(vm, 0, &pyZoomX);
    pkpy_to_float(vm, 1, &pyZoomY);

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->zoom = (SKAVector2){camera2D->zoom.x + (float)pyZoomX, camera2D->zoom.y + (float)pyZoomY };
    return 0;
}

int cre_pkpy_api_camera2d_get_zoom(pkpy_vm* vm) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    pkpy_push_float(vm, (double)camera2D->zoom.x);
    pkpy_push_float(vm, (double)camera2D->zoom.y);
    return 2;
}

int cre_pkpy_api_camera2d_set_boundary(pkpy_vm* vm) {
    double pyBoundaryX, pyBoundaryY, pyBoundaryW, pyBoundaryH;
    pkpy_to_float(vm, 0, &pyBoundaryX);
    pkpy_to_float(vm, 1, &pyBoundaryY);
    pkpy_to_float(vm, 2, &pyBoundaryW);
    pkpy_to_float(vm, 3, &pyBoundaryH);

    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    camera2D->boundary = (SKARect2){ (float)pyBoundaryX, (float)pyBoundaryY, (float)pyBoundaryW, (float)pyBoundaryH };
    cre_camera2d_clamp_viewport_to_boundary(camera2D);
    return 0;
}

int cre_pkpy_api_camera2d_get_boundary(pkpy_vm* vm) {
    const CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    pkpy_push_float(vm, (double)camera2D->boundary.x);
    pkpy_push_float(vm, (double)camera2D->boundary.y);
    pkpy_push_float(vm, (double)camera2D->boundary.w);
    pkpy_push_float(vm, (double)camera2D->boundary.h);
    return 4;
}

int cre_pkpy_api_camera2d_follow_node(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    cre_camera2d_follow_entity(camera2D, entity);
    return 0;
}

int cre_pkpy_api_camera2d_unfollow_node(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    cre_camera2d_unfollow_entity(camera2D, entity);
    return 0;
}

//--- WORLD ---//
void cre_pkpy_mark_scene_nodes_time_dilation_flag_dirty(SceneTreeNode* node) {
    NodeComponent* nodeComponent = (NodeComponent*) cre_component_manager_get_component_unchecked(node->entity, CreComponentDataIndex_NODE);
    SE_ASSERT(nodeComponent != NULL);
    nodeComponent->timeDilation.cacheInvalid = true;
}

int cre_pkpy_api_world_set_time_dilation(pkpy_vm* vm) {
    double pyTimeDilation;
    pkpy_to_float(vm, 0, &pyTimeDilation);

    cre_world_set_time_dilation((float)pyTimeDilation);
    cre_scene_manager_execute_on_root_and_child_nodes(cre_pkpy_mark_scene_nodes_time_dilation_flag_dirty);
    return 0;
}

int cre_pkpy_api_world_get_time_dilation(pkpy_vm* vm) {
    pkpy_push_float(vm, (double)cre_world_get_time_dilation());
    return 1;
}

int cre_pkpy_api_world_get_delta_time(pkpy_vm* vm) {
    pkpy_push_float(vm, (double)(cre_world_get_time_dilation() * CRE_GLOBAL_PHYSICS_DELTA_TIME));
    return 1;
}

//--- AUDIO SOURCE ---//

int cre_pkpy_api_audio_source_set_pitch(pkpy_vm* vm) {
    pkpy_CString pyPath;
    double pyPitch;
    pkpy_to_string(vm, 0, &pyPath);
    pkpy_to_float(vm, 1, &pyPitch);

    const char* path = cre_pkpy_api_helper_convert_pkpy_CString(&pyPath);
    if (se_asset_manager_has_audio_source(path)) {
        SEAudioSource* audioSource = se_asset_manager_get_audio_source(path);
        audioSource->pitch = pyPitch;
    } else {
        se_logger_warn("Tried to set non-existent audio source's pitch at '%s'", path);
    }
    return 0;
}
int cre_pkpy_api_audio_source_get_pitch(pkpy_vm* vm) {
    pkpy_CString pyPath;
    pkpy_to_string(vm, 0, &pyPath);

    const char* path = cre_pkpy_api_helper_convert_pkpy_CString(&pyPath);
    if (se_asset_manager_has_audio_source(path)) {
        SEAudioSource* audioSource = se_asset_manager_get_audio_source(path);
        pkpy_push_float(vm, audioSource->pitch);
    } else {
        se_logger_warn("Tried to get non-existent audio source's pitch at '%s'", path);
        pkpy_push_float(vm, 1.0);
    }
    return 1;
}

//--- AUDIO MANAGER ---//

int cre_pkpy_api_audio_manager_play_sound(pkpy_vm* vm) {
    pkpy_CString pyPath;
    bool pyLoops;
    pkpy_to_string(vm, 0, &pyPath);
    pkpy_to_bool(vm, 1, &pyLoops);

    const char* path = cre_pkpy_api_helper_convert_pkpy_CString(&pyPath);
    se_audio_manager_play_sound(path, pyLoops);
    return 0;
}

int cre_pkpy_api_audio_manager_stop_sound(pkpy_vm* vm) {
    pkpy_CString pyPath;
    pkpy_to_string(vm, 0, &pyPath);

    const char* path = cre_pkpy_api_helper_convert_pkpy_CString(&pyPath);
    se_audio_manager_stop_sound(path);
    return 0;
}

//--- GAME CONFIG ---//

int cre_pkpy_api_game_config_save(pkpy_vm* vm) {
    pkpy_CString pyPath;
    pkpy_CString pyDataJson;
    pkpy_CString pyEncryptionKey;
    pkpy_to_string(vm, 0, &pyPath);
    pkpy_to_string(vm, 1, &pyDataJson);
    pkpy_to_string(vm, 2, &pyEncryptionKey);

    const char* path = cre_pkpy_api_helper_convert_pkpy_CString(&pyPath);
    const char* dataJson = cre_pkpy_api_helper_convert_pkpy_CString(&pyDataJson);
    const char* encryptionKey = cre_pkpy_api_helper_convert_pkpy_CString(&pyEncryptionKey); // TODO: Use
    const CREGameProperties* gameProps = cre_game_props_get();
    char* validGameTitle = se_strdup(gameProps->gameTitle);
    se_str_to_lower_and_underscore_whitespace(validGameTitle);
    char* fullSavePath = se_fs_get_user_save_path("crescent", validGameTitle, path);
    const bool success = se_fs_write_to_file(fullSavePath, dataJson);
    SE_MEM_FREE(fullSavePath);
    SE_MEM_FREE(validGameTitle);
    pkpy_push_bool(vm, success);
    return 1;
}

int cre_pkpy_api_game_config_load(pkpy_vm* vm) {
    pkpy_CString pyPath;
    pkpy_CString pyEncryptionKey;
    pkpy_to_string(vm, 0, &pyPath);
    pkpy_to_string(vm, 1, &pyEncryptionKey);

    const char* path = cre_pkpy_api_helper_convert_pkpy_CString(&pyPath);
    const char* encryptionKey = cre_pkpy_api_helper_convert_pkpy_CString(&pyEncryptionKey); // TODO: Use
    const CREGameProperties* gameProps = cre_game_props_get();
    char* validGameTitle = se_strdup(gameProps->gameTitle);
    se_str_to_lower_and_underscore_whitespace(validGameTitle);
    char* fullSavePath = se_fs_get_user_save_path("crescent", validGameTitle, path);
    char* fileContents = sf_asset_file_loader_read_file_contents_as_string(fullSavePath, NULL);
    pkpy_push_string(vm, pkpy_string(fileContents));
    SE_MEM_FREE(validGameTitle);
    SE_MEM_FREE(fullSavePath);
    SE_MEM_FREE(fileContents);
    return 1;
}

//--- Packed Scene ---//

int cre_pkpy_api_packed_scene_create_instance(pkpy_vm* vm) {
    int pyCacheId;
    pkpy_to_int(vm, 0, &pyCacheId);

    const CreSceneCacheId cacheId = (CreSceneCacheId)pyCacheId;
    JsonSceneNode* sceneNode = cre_scene_template_cache_get_scene(cacheId);
    SceneTreeNode* rootNode = cre_scene_manager_stage_scene_nodes_from_json(sceneNode);

    cre_pkpy_script_context_create_instance(rootNode->entity, CRE_PKPY_MODULE_NAME_CRESCENT, node_get_base_type_string(sceneNode->type));
    cre_pkpy_entity_instance_cache_push_entity_instance(vm, rootNode->entity);
    return 1;
}

int cre_pkpy_api_packed_scene_load(pkpy_vm* vm) {
    pkpy_CString pyScenePath;
    pkpy_to_string(vm, 0, &pyScenePath);

    const char* scenePath = cre_pkpy_api_helper_convert_pkpy_CString(&pyScenePath);
    const CreSceneCacheId cacheId = cre_scene_template_cache_load_scene(scenePath);
    pkpy_push_int(vm, (int)cacheId);
    return 1;
}

//--- Collision Handler ---//

int cre_pkpy_api_collision_handler_process_collisions(pkpy_vm* vm) {
    int pyEntityId;
    pkpy_to_int(vm, 0, &pyEntityId);

    const CreEntity entity = (CreEntity)pyEntityId;
    const CollisionResult collisionResult = cre_collision_process_entity_collisions(entity);
    for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
        const CreEntity collidedEntity = collisionResult.collidedEntities[i];
        cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(collidedEntity);
    }
    return (int)collisionResult.collidedEntityCount;
}

int cre_pkpy_api_collision_handler_process_mouse_collisions(pkpy_vm* vm) {
    double pyPosOffsetX, pyPosOffsetY;
    double pyCollisionSizeW, pyCollisionSizeH;
    pkpy_to_float(vm, 0, &pyPosOffsetX);
    pkpy_to_float(vm, 1, &pyPosOffsetY);
    pkpy_to_float(vm, 2, &pyCollisionSizeW);
    pkpy_to_float(vm, 3, &pyCollisionSizeH);

    const SKAVector2 positionOffset = { (float)pyPosOffsetX, (float)pyPosOffsetY };
    const SKAVector2 mouseWorldPos = cre_pkpy_api_helper_mouse_get_global_position(&positionOffset);
    const SKARect2 collisionRect = { mouseWorldPos.x, mouseWorldPos.y, (float)pyCollisionSizeW, (float)pyCollisionSizeH };
    const CollisionResult collisionResult = cre_collision_process_mouse_collisions(&collisionRect);
    for (size_t i = 0; i < collisionResult.collidedEntityCount; i++) {
        const CreEntity collidedEntity = collisionResult.collidedEntities[i];
        cre_pkpy_script_context_create_instance_if_nonexistent_and_push_entity_instance(collidedEntity);
    }
    return (int)collisionResult.collidedEntityCount;
}

//--- Network ---//
int cre_pkpy_api_network_is_server(pkpy_vm* vm) {
    pkpy_push_bool(vm, se_network_is_server());
    return 1;
}

//--- Server ---//
int cre_pkpy_api_server_start(pkpy_vm* vm) {
    int pyPort;
    pkpy_to_int(vm, 0, &pyPort);

    se_udp_server_initialize(pyPort, cre_ec_system_network_callback);
    return 0;
}

int cre_pkpy_api_server_stop(pkpy_vm* vm) {
    se_udp_server_finalize();
    return 0;
}

int cre_pkpy_api_server_send(pkpy_vm* vm) {
    pkpy_CString pyMessage;
    pkpy_to_string(vm, 0, &pyMessage);

    const char* message = cre_pkpy_api_helper_convert_pkpy_CString(&pyMessage);
    se_udp_server_send_message(message);
    return 0;
}

//--- Client ---//
int cre_pkpy_api_client_start(pkpy_vm* vm) {
    pkpy_CString pyHost;
    int pyPort;
    pkpy_to_string(vm, 0, &pyHost);
    pkpy_to_int(vm, 1, &pyPort);

    const char* host = cre_pkpy_api_helper_convert_pkpy_CString(&pyHost);
    se_udp_client_initialize(host, pyPort, cre_ec_system_network_callback);
    return 0;
}
int cre_pkpy_api_client_stop(pkpy_vm* vm) {
    se_udp_client_finalize();
    return 0;
}
int cre_pkpy_api_client_send(pkpy_vm* vm) {
    pkpy_CString pyMessage;
    pkpy_to_string(vm, 0, &pyMessage);

    const char* message = cre_pkpy_api_helper_convert_pkpy_CString(&pyMessage);
    se_udp_client_send_message(message);
    return 0;
}
