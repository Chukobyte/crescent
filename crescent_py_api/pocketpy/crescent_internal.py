from typing import Tuple, Optional


# Mock class for engine's internal c python module

# --- NODE --- #
def node_new(class_path: str, class_name: str, node_type_flag: int) -> "Node":
    return None


def node_get_name(entity_id: int) -> str:
    return "Name"


def node_add_child(parent_entity_id: int, child_entity_id: int) -> None:
    pass


def node_get_child(parent_entity_id: int, child_entity_name: str) -> Optional["Node"]:
    return None


def node_get_children(entity_id: int) -> Tuple["Node", ...]:
    return ()


def node_get_parent(entity_id: int) -> Optional["Node"]:
    return None


def node_queue_deletion(entity_id: int) -> None:
    pass


def node_is_queued_for_deletion(entity_id: int) -> bool:
    return False


def node_set_time_dilation(entity_id: int, dilation: float) -> None:
    pass


def node_get_time_dilation(entity_id: int) -> float:
    return 1.0


def node_get_total_time_dilation(entity_id: int) -> float:
    return 1.0


# --- NODE2D --- #

def node2d_set_position(entity_id: int, x: float, y: float) -> None:
    pass


def node2d_add_to_position(entity_id: int, x: float, y: float) -> None:
    pass


def node2d_get_position(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def node2d_get_global_position(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def node2d_set_scale(entity_id: int, x: float, y: float) -> None:
    pass


def node2d_add_to_scale(entity_id: int, x: float, y: float) -> None:
    pass


def node2d_get_scale(entity_id: int) -> Tuple[float, float]:
    return 1.0, 1.0


def node2d_set_rotation(entity_id: int, rotation: float) -> None:
    pass


def node2d_add_to_rotation(entity_id: int, rotation: float) -> None:
    pass


def node2d_get_rotation(entity_id: int) -> float:
    return 0.0


def node2d_set_z_index(entity_id: int, z_index: int) -> None:
    pass


def node2d_get_z_index(entity_id: int) -> int:
    return 0


def node2d_set_z_index_relative_to_parent(entity_id: int, is_relative: bool) -> None:
    pass


def node2d_get_z_index_relative_to_parent(entity_id: int) -> bool:
    return True


def node2d_set_ignore_camera(entity_id: int, ignore_camera: bool) -> None:
    pass


def node2d_get_ignore_camera(entity_id: int) -> bool:
    return False


# --- SPRITE --- #
def sprite_get_texture(entity_id: int) -> Tuple[str, str, str, bool]:
    return "assets/image.png", "linear", "linear", True


def sprite_set_texture(entity_id: int, file_path: str) -> None:
    pass


def sprite_get_draw_source(entity_id: int) -> Tuple[float, float, float, float]:
    return 0.0, 0.0, 8.0, 8.0


def sprite_set_draw_source(entity_id: int, x: float, y: float, w: float, h: float) -> None:
    pass


def sprite_get_flip_h(entity_id: int) -> bool:
    return False


def sprite_set_flip_h(entity_id: int, flip_h: bool) -> None:
    pass


def sprite_get_flip_v(entity_id: int) -> bool:
    return False


def sprite_set_flip_v(entity_id: int, flip_v: bool) -> None:
    pass


def sprite_get_modulate(entity_id: int) -> Tuple[int, int, int, int]:
    return 255, 255, 255, 255


def sprite_set_modulate(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


def sprite_get_origin(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def sprite_set_origin(entity_id: int, x: float, y: float) -> None:
    pass


def sprite_get_shader_instance(entity_id: int) -> int:
    return 0


def sprite_set_shader_instance(entity_id: int, shader_instance_id: int) -> None:
    pass


# --- ANIMATED SPRITE --- #

def animated_sprite_play(entity_id: int, animation_name: str) -> bool:
    return True


def animated_sprite_stop(entity_id: int) -> None:
    pass


def animated_sprite_set_current_animation_frame(entity_id: int, frame: int) -> None:
    pass


def animated_sprite_add_animation(entity_id: int, name: str, speed: float, does_loop: bool, frame_count: int, *args) -> None:
    pass


def animated_sprite_get_stagger_animation_start_times(entity_id: int) -> bool:
    return False


def animated_sprite_set_stagger_animation_start_times(entity_id: int, stagger: bool) -> None:
    pass


def animated_sprite_get_flip_h(entity_id: int) -> bool:
    return False


def animated_sprite_set_flip_h(entity_id: int, flip_h: bool) -> None:
    pass


def animated_sprite_get_flip_v(entity_id: int) -> bool:
    return False


def animated_sprite_set_flip_v(entity_id: int, flip_v: bool) -> None:
    pass


def animated_sprite_get_modulate(entity_id: int) -> Tuple[int, int, int, int]:
    return 255, 255, 255, 255


def animated_sprite_set_modulate(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


def animated_sprite_get_origin(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def animated_sprite_set_origin(entity_id: int, x: float, y: float) -> None:
    pass


def animated_sprite_get_shader_instance(entity_id: int) -> int:
    return 0


def animated_sprite_set_shader_instance(entity_id: int, shader_instance_id: int) -> None:
    pass


# --- TEXT LABEL --- #
def text_label_get_text(entity_id: int) -> str:
    return ""


def text_label_set_text(entity_id: int, text: str) -> None:
    pass


def text_label_get_color(entity_id: int) -> Tuple[int, int, int, int]:
    return 255, 255, 255, 255


def text_label_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


def text_label_set_font_uid(entity_id: int, uid: str) -> None:
    pass


# --- COLLIDER2D --- #
def collider2d_get_extents(entity_id: int) -> Tuple[float, float]:
    return 8.0, 8.0


def collider2d_set_extents(entity_id: int, w: float, h: float):
    pass


def collider2d_get_color(entity_id: int) -> Tuple[int, int, int, int]:
    return 255, 255, 255, 255


def collider2d_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


# --- COLOR RECT --- #

def color_rect_get_size(entity_id: int) -> Tuple[float, float]:
    return 8.0, 8.0


def color_rect_set_size(entity_id: int, w: float, h: float) -> None:
    pass


def color_rect_get_color(entity_id: int) -> Tuple[int, int, int, int]:
    return 255, 255, 255, 255


def color_rect_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


# --- PARALLAX --- #

def parallax_get_scroll_speed(entity_id: int) -> Tuple[float, float]:
    return 1.0, 1.0


def parallax_set_scroll_speed(entity_id: int, x: float, y: float) -> None:
    pass


# --- PARTICLES2D --- #

def particles2d_get_amount(entity_id: int) -> int:
    return 0


def particles2d_set_amount(entity_id: int, amount: int) -> None:
    pass


def particles2d_get_life_time(entity_id: int) -> float:
    return 0.0


def particles2d_set_life_time(entity_id: int, life_time: float) -> None:
    pass


def particles2d_get_damping(entity_id: int) -> float:
    return 0.999


def particles2d_set_damping(entity_id: int, damping: float) -> None:
    pass


def particles2d_get_explosiveness(entity_id: int) -> float:
    return 0.999


def particles2d_set_explosiveness(entity_id: int, explosiveness: float) -> None:
    pass


def particles2d_get_color(entity_id: int) -> Tuple[int, int, int, int]:
    return 0, 0, 0, 0


def particles2d_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


def particles2d_get_initial_velocity(entity_id: int) -> Tuple[float, float, float, float]:
    return 0.0, 0.0, 0.0, 0.0


def particles2d_set_initial_velocity(entity_id: int, min_x: float, min_y: float, max_x: float, max_y: float) -> None:
    pass


def particles2d_get_spread(entity_id: int) -> float:
    return 45.0


def particles2d_set_spread(entity_id: int, spread: float) -> None:
    pass


# --- SHADER INSTANCE --- #

def shader_instance_delete(shader_id: int) -> bool:
    return True


def shader_instance_create_bool_param(shader_id: int, name: str, initial_value: bool) -> None:
    pass


def shader_instance_set_bool_param(shader_id: int, name: str, value: bool) -> None:
    pass


def shader_instance_get_bool_param(shader_id: int, name: str) -> bool:
    return False


def shader_instance_create_int_param(shader_id: int, name: str, initial_value: int) -> None:
    pass


def shader_instance_set_int_param(shader_id: int, name: str, value: int) -> None:
    pass


def shader_instance_get_int_param(shader_id: int, name: str) -> int:
    return 0


def shader_instance_create_float_param(shader_id: int, name: str, initial_value: float) -> None:
    pass


def shader_instance_set_float_param(shader_id: int, name: str, value: float) -> None:
    pass


def shader_instance_get_float_param(shader_id: int, name: str) -> float:
    return 0.0


def shader_instance_create_float2_param(shader_id: int, name: str, initial_value_x: float, initial_value_y: float) -> None:
    pass


def shader_instance_set_float2_param(shader_id: int, name: str, value_x: float, value_y: float) -> None:
    pass


def shader_instance_get_float2_param(shader_id: int, name: str) -> Tuple[float, float]:
    pass


def shader_instance_create_float3_param(shader_id: int, name: str, initial_value_x: float, initial_value_y: float, initial_value_z: float) -> None:
    pass


def shader_instance_set_float3_param(shader_id: int, name: str, value_x: float, value_y: float, value_z: float) -> None:
    pass


def shader_instance_get_float3_param(shader_id: int, name) -> Tuple[float, float, float]:
    return 0.0, 0.0, 0.0


def shader_instance_create_float4_param(shader_id: int, name: str, initial_value_x: float, initial_value_y: float, initial_value_z: float, initial_value_w: float) -> None:
    pass


def shader_instance_set_float4_param(shader_id: int, name: str, value_x: float, value_y: float, value_z: float, value_w: float) -> None:
    pass


def shader_instance_get_float4_param(shader_id: int, name: str) -> Tuple[float, float, float, float]:
    return 0.0, 0.0, 0.0, 0.0


# --- SHADER UTIL --- #

def shader_util_compile_shader(shader_path: str) -> int:
    return 0


def shader_util_compile_shader_raw(vertex_path, fragment_path) -> int:
    return 0


def shader_util_set_screen_shader(shader_id: int) -> bool:
    return True


def shader_util_get_current_screen_shader() -> int:
    return 0


def shader_util_reset_screen_shader_to_default() -> None:
    pass


# --- ENGINE --- #

def engine_exit(code: int) -> None:
    pass


def engine_set_target_fps(fps: int) -> None:
    pass


def engine_get_target_fps() -> int:
    return 66


def engine_get_average_fps() -> int:
    return 66


def engine_set_fps_display_enabled(enabled: bool, font_uid: str, position_x: float, position_y: float) -> None:
    pass


def engine_get_global_physics_delta_time() -> float:
    return 0.1


# --- INPUT --- #

def input_is_key_pressed(key: int) -> bool:
    return False


def input_is_key_just_pressed(key: int) -> bool:
    return False


def input_is_key_just_released(key: int) -> bool:
    return False


def input_add_action(name: str, value_key: int, device_id: int) -> None:
    pass


def input_is_action_pressed(name: str) -> bool:
    return False


def input_is_action_just_pressed(name: str) -> bool:
    return False


def input_is_action_just_released(name: str) -> bool:
    return False


def input_start_gamepad_vibration(device_id: int, weak_magnitude: float, strong_magnitude: float, duration: float) -> None:
    pass


def input_stop_gamepad_vibration(device_id: int) -> None:
    pass


def input_mouse_get_position() -> Tuple[float, float]:
    return 0.0, 0.0


def input_mouse_get_world_position() -> Tuple[float, float]:
    return 0.0, 0.0


# --- SCENE TREE --- #
def scene_tree_change_scene(path: str) -> None:
    pass


def scene_tree_get_root() -> Optional["Node"]:
    return None

# Exposed internal functions that should only be used for testing


# --- SCENE MANAGER --- #
def _scene_manager_process_queued_creation_entities() -> None:
    pass


def _scene_manager_process_queued_scene_change() -> None:
    pass


# --- GAME PROPERTIES --- #

def game_properties_get() -> Tuple[str, int, int, int, int, int, str, bool]:
    return "Game", 640, 480, 640, 480, 66, "scene.cscn", False


# --- CAMERA2D --- #

def camera2d_set_position(x: float, y: float) -> None:
    pass


def camera2d_add_to_position(x: float, y: float) -> None:
    pass


def camera2d_get_position() -> Tuple[float, float]:
    return 0.0, 0.0


def camera2d_set_offset(x: float, y: float) -> None:
    pass


def camera2d_add_to_offset(x: float, y: float) -> None:
    pass


def camera2d_get_offset() -> Tuple[float, float]:
    return 0.0, 0.0


def camera2d_set_zoom(x: float, y: float) -> None:
    pass


def camera2d_add_to_zoom(x: float, y: float) -> None:
    pass


def camera2d_get_zoom() -> Tuple[float, float]:
    return 1.0, 1.0


def camera2d_set_boundary(x: float, y: float, w: float, h: float) -> None:
    pass


def camera2d_get_boundary() -> Tuple[float, float, float, float]:
    return 0.0, 0.0, 0.0, 0.0


def camera2d_follow_node(entity_id: int) -> None:
    pass


def camera2d_unfollow_node(entity_id: int) -> None:
    pass


# --- World --- #

def world_set_time_dilation(time_dilation: float) -> None:
    pass


def world_get_time_dilation() -> float:
    return 1.0


def world_get_delta_time() -> float:
    return 0.1


def world_get_variable_delta_time() -> float:
    return 0.1


# --- Audio Source --- #

def audio_source_get_pitch(path: str) -> float:
    return 1.0


def audio_source_set_pitch(path: str, pitch: float) -> None:
    pass


# --- Audio Manager --- #

def audio_manager_play_sound(path: str, loops: bool) -> None:
    pass


def audio_manager_stop_sound(path: str) -> None:
    pass


# --- Game Config --- #

def game_config_save(path: str, json_text: str, encryption_key: str) -> bool:
    return True


def game_config_load(path, encryption_key) -> str:
    return "{}"


# --- Packed Scene --- #

def packed_scene_create_instance(scene_cache_id: int) -> "Node":
    return None


def packed_scene_load(path: str) -> int:
    return 0


# --- Collision Handler --- #

def collision_handler_process_collisions(entity_id: float) -> Tuple["Node", ...]:
    return ()


def collision_handler_process_mouse_collisions(pos_offset_x: float, pos_offset_y: float, collision_size_w: float, collision_size_h: float) -> Tuple["Node", ...]:
    return ()


# --- Network --- #

def network_is_server() -> bool:
    return True


# --- Server --- #

def server_start(port: int) -> None:
    pass


def server_stop() -> None:
    pass


def server_send(message: str) -> None:
    pass

# --- Client --- #

def client_start(host: str, port: int) -> None:
    pass


def client_stop() -> None:
    pass


def client_send(message: str) -> None:
    pass
