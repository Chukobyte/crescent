from typing import Callable, Tuple, List


def engine_exit(code=0) -> None:
    pass


def engine_set_target_fps(fps: float) -> None:
    pass


def engine_get_target_fps() -> int:
    return 66


def engine_get_average_fps() -> float:
    return 66.0


def engine_set_fps_display_enabled(enabled: bool) -> None:
    pass


def engine_get_global_physics_delta_time() -> float:
    return 0.1


def configure_game(
    game_title="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=800,
    target_fps=66,
    initial_node_path="",
    colliders_visible=False,
) -> None:
    pass


def configure_assets(audio_sources=None, textures=None, fonts=None) -> None:
    if fonts is None:
        fonts = []
    if textures is None:
        textures = []
    if audio_sources is None:
        audio_sources = []


def configure_inputs(input_actions=None) -> None:
    if input_actions is None:
        input_actions = []


def create_stage_nodes(stage_nodes: list) -> None:
    pass


def curve_float_create_new() -> int:
    return 0


def curve_float_delete(curve_id: int) -> None:
    pass


def curve_float_load_from_file(curve_id: int, file_path: str) -> bool:
    return True


def curve_float_add_point(
    curve_id: int, x: float, y: float, tangent_in: float, tangent_out: float
) -> None:
    pass


def curve_float_remove_point(curve_id: int, x: float, y: float) -> bool:
    return True


def curve_float_eval(curve_id: int, t: float) -> float:
    return 0.0


def audio_source_set_pitch(path: str, pitch: float) -> None:
    pass


def audio_source_get_pitch(path: str) -> float:
    return 1.0


def shader_instance_delete(shader_id: int) -> bool:
    return True


def shader_instance_create_bool_param(
    shader_id: int, name: str, initial_value: bool
) -> None:
    pass


def shader_instance_set_bool_param(shader_id: int, name: str, value: bool) -> None:
    pass


def shader_instance_get_bool_param(shader_id: int, name: str) -> bool:
    return False


def shader_instance_create_int_param(
    shader_id: int, name: str, initial_value: int
) -> None:
    pass


def shader_instance_set_int_param(shader_id: int, name: str, value: int) -> None:
    pass


def shader_instance_get_int_param(shader_id: int, name: str) -> int:
    return 0


def shader_instance_create_float_param(
    shader_id: int, name: str, initial_value: float
) -> None:
    pass


def shader_instance_set_float_param(shader_id: int, name: str, value: float) -> None:
    pass


def shader_instance_get_float_param(shader_id: int, name: str) -> float:
    return 0.0


def shader_instance_create_float2_param(
    shader_id: int, name: str, initial_value_x: float, initial_value_y: float
) -> None:
    pass


def shader_instance_set_float2_param(
    shader_id: int, name: str, value_x: float, value_y: float
) -> None:
    pass


def shader_instance_get_float2_param(shader_id: int, name: str) -> Tuple[float, float]:
    return 0.0, 0.0


def shader_instance_create_float3_param(
    shader_id: int,
    name: str,
    initial_value_x: float,
    initial_value_y: float,
    initial_value_z: float,
) -> None:
    pass


def shader_instance_set_float3_param(
    shader_id: int, name: str, value_x: float, value_y: float, value_z: float
) -> None:
    pass


def shader_instance_get_float3_param(
    shader_id: int, name: str
) -> Tuple[float, float, float]:
    return 0.0, 0.0, 0.0


def shader_instance_create_float4_param(
    shader_id: int,
    name: str,
    initial_value_x: float,
    initial_value_y: float,
    initial_value_z: float,
    initial_value_w: float,
) -> None:
    pass


def shader_instance_set_float4_param(
    shader_id: int,
    name: str,
    value_x: float,
    value_y: float,
    value_z: float,
    value_w: float,
) -> None:
    pass


def shader_instance_get_float4_param(
    shader_id: int, name: str
) -> Tuple[float, float, float, float]:
    return 0.0, 0.0, 0.0, 0.0


def shader_util_compile_shader(shader_path: str) -> int:
    return 0


def shader_util_compile_shader_raw(vertex_path: str, fragment_path: str) -> int:
    return 0


def shader_util_set_screen_shader(shader_id: int) -> bool:
    return True


def shader_util_reset_screen_shader_to_default() -> None:
    pass


def input_add_action(name: str, value: str, device_id: int) -> None:
    pass


def input_is_action_pressed(name: str) -> bool:
    return False


def input_is_action_just_pressed(name: str) -> bool:
    return False


def input_is_action_just_released(name: str) -> bool:
    return False


def mouse_get_position() -> Tuple[float, float]:
    return 0.0, 0.0


def mouse_get_world_position() -> Tuple[float, float]:
    return 0.0, 0.0


def scene_tree_change_scene(path: str) -> None:
    pass


def scene_tree_get_root() -> Tuple[int, str]:
    return 0, "Node"


def world_set_time_dilation(time_dilation: float) -> None:
    pass


def world_get_time_dilation() -> float:
    return 1.0


def audio_manager_play_sound(path: str, loops: bool) -> None:
    pass


def audio_manager_stop_sound(path: str) -> None:
    pass


def camera2D_set_position(x: float, y: float) -> None:
    pass


def camera2D_add_to_position(x: float, y: float) -> None:
    pass


def camera2D_get_position() -> Tuple[float, float]:
    return 0.0, 0.0


def camera2D_set_offset(x: float, y: float) -> None:
    pass


def camera2D_add_to_offset(x: float, y: float) -> None:
    pass


def camera2D_get_offset() -> Tuple[float, float]:
    return 0.0, 0.0


def camera2D_set_zoom(x: float, y: float) -> None:
    pass


def camera2D_add_to_zoom(x: float, y: float) -> None:
    pass


def camera2D_get_zoom() -> Tuple[float, float]:
    return 0.0, 0.0


def camera2D_set_boundary(x: float, y: float, w: float, h: float) -> None:
    pass


def camera2D_get_boundary() -> Tuple[float, float, float, float]:
    return -1000000.0, -1000000.0, 1000000.0, 1000000.0


def camera2D_follow_node(entity_id: int) -> None:
    pass


def camera2D_unfollow_node(entity_id: int) -> None:
    pass


def game_properties_get() -> Tuple[str, float, float, float, float, int, str, bool]:
    """returns GameProperties(game_title, res_w, res_h, window_w, window_h, target_fps, initial_scene_path, are_colliders_visible)."""
    return "test", 800.0, 450.0, 800.0, 450.0, 66, "scenes/test.cscn", False


def node_new(class_path: str, class_name: str, node_type: str):
    return None


def node_queue_deletion(entity_id: int) -> None:
    pass


def node_add_child(parent_entity_id: int, child_entity_id: int) -> None:
    pass


def node_get_child(entity_id: int, child_name: str) -> Tuple[int, str]:
    return 2, "Node"


def node_get_children(entity_id: int) -> list:
    return [(2, "Node"), (3, "Node")]


def node_get_parent(entity_id: int) -> Tuple[int, str]:
    return 1, "Node"


def node_create_event(entity_id: int, event_id: str) -> None:
    pass


def node_subscribe_to_event(
    entity_id: int, event_id: str, scoped_entity_id: int, callback_func: Callable
) -> None:
    pass


def node_broadcast_event(entity_id: int, event_id: str, args) -> None:
    pass


def node_get_name(entity_id: int) -> str:
    return "Node"


def node_set_time_dilation(entity_id: int, time_dilation: float) -> None:
    pass


def node_get_time_dilation(entity_id: int) -> float:
    return 1.0


def node_get_full_time_dilation(entity_id: int) -> float:
    return 1.0


def node2D_set_position(entity_id: int, x: float, y: float) -> None:
    pass


def node2D_add_to_position(entity_id: int, x: float, y: float) -> None:
    pass


def node2D_get_position(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def node2D_get_global_position(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def node2D_set_scale(entity_id: int, x: float, y: float) -> None:
    pass


def node2D_add_to_scale(entity_id: int, x: float, y: float) -> None:
    pass


def node2D_get_scale(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def node2D_set_rotation(entity_id: int, rotation: float) -> None:
    pass


def node2D_add_to_rotation(entity_id: int, rotation: float) -> None:
    pass


def node2D_get_rotation(entity_id: int) -> float:
    return 0.0


def node2D_get_z_index(entity_id: int) -> int:
    return 0


def node2D_set_z_index(entity_id: int, z_index: int) -> None:
    pass


def node2D_get_ignore_camera(entity_id: int) -> bool:
    return False


def node2D_set_ignore_camera(entity_id: int, ignore_camera: bool) -> bool:
    pass


def sprite_set_texture(entity_id: int, file_path: str) -> None:
    pass


def sprite_get_texture(entity_id: int) -> Tuple[str, str, str, bool]:
    return (
        "assets/texture.png",
        "clamp_to_border",
        "clamp_to_border",
        True,
    )


def sprite_set_draw_source(
    entity_id: int, x: float, y: float, w: float, h: float
) -> None:
    pass


def sprite_get_draw_source(entity_id: int) -> Tuple[float, float, float, float]:
    return 0.0, 0.0, 32.0, 32.0


def sprite_set_modulate(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


def sprite_get_modulate(entity_id: int) -> Tuple[int, int, int, int]:
    return 0, 0, 0, 255


def sprite_set_flip_h(entity_id: int, flip_h: bool) -> None:
    pass


def sprite_get_flip_h(entity_id: int) -> bool:
    return False


def sprite_set_flip_v(entity_id: int, flip_v: bool) -> None:
    pass


def sprite_get_flip_v(entity_id: int) -> bool:
    return False


def sprite_get_shader_instance(entity_id: int) -> int:
    return 0


def sprite_set_shader_instance(entity_id: int, shader_instance_id: int) -> None:
    pass


def animated_sprite_play(entity_id: int, name: str) -> bool:
    return True


def animated_sprite_stop(entity_id: int) -> None:
    pass


def animated_sprite_add_animation(
    entity_id: int,
    name: str,
    speed: int,
    loops: bool,
    frames: List[Tuple[int, str, float, float, float, float]],
) -> None:
    pass


def animated_sprite_set_modulate(
    entity_id: int, r: int, g: int, b: int, a: int
) -> None:
    pass


def animated_sprite_get_modulate(entity_id: int) -> Tuple[int, int, int, int]:
    return 0, 0, 0, 255


def animated_sprite_set_flip_h(entity_id: int, flip_h: bool) -> None:
    pass


def animated_sprite_get_flip_h(entity_id: int) -> bool:
    return False


def animated_sprite_set_flip_v(entity_id: int, flip_v: bool) -> None:
    pass


def animated_sprite_get_flip_v(entity_id: int) -> bool:
    return False


def animated_sprite_get_shader_instance(entity_id: int) -> int:
    return 0


def animated_sprite_set_shader_instance(
    entity_id: int, shader_instance_id: int
) -> None:
    pass


def text_label_set_text(entity_id: int, text: str) -> None:
    pass


def text_label_get_text(entity_id: int) -> str:
    return ""


def text_label_set_color(entity_id: int, r: int, g: int, b: int, a: int) -> None:
    pass


def text_label_get_color(entity_id: int) -> Tuple[int, int, int, int]:
    return 255, 255, 255, 255


def text_label_set_font_uid(entity_id: int, uid: str) -> None:
    pass


def text_label_get_font_uid(entity_id: int) -> str:
    return "default"


def collider2D_set_extents(entity_id: int, w: float, h: float) -> None:
    pass


def collider2D_get_extents(entity_id: int) -> Tuple[float, float]:
    return 0.0, 0.0


def collider2D_set_color(
    entity_id: int, r: float, g: float, b: float, a: float
) -> None:
    pass


def collider2D_get_color(entity_id: int) -> Tuple[float, float, float, float]:
    return 0.2, 0.2, 0.2, 1.0


def color_rect_set_size(entity_id: int, w: float, h: float) -> None:
    pass


def color_rect_get_size(entity_id: int) -> Tuple[float, float]:
    return 32.0, 32.0


def color_rect_set_color(
    entity_id: int, r: float, g: float, b: float, a: float
) -> None:
    pass


def color_rect_get_color(entity_id: int) -> Tuple[float, float, float, float]:
    return 0.2, 0.2, 0.2, 1.0


def network_is_server() -> bool:
    return True


def server_start(port: int) -> None:
    pass


def server_stop() -> None:
    pass


def server_send(message: str) -> None:
    pass


def server_subscribe(signal_id: str, listener_node: int, listener_func) -> None:
    pass


def client_start(host: str, port: int) -> None:
    pass


def client_stop() -> None:
    pass


def client_send(message: str) -> None:
    pass


def client_subscribe(signal_id: str, listener_node: int, listener_func) -> None:
    pass


def collision_handler_process_collisions(entity_id: int) -> list:
    return []


def collision_handler_process_mouse_collisions(
    pos_offset_x: float,
    pos_offset_y: float,
    collision_size_w: float,
    collision_size_h: float,
) -> list:
    return []


def game_config_save(path: str, data_json: str, encryption_key="") -> bool:
    return True


def game_config_load(path: str, encryption_key="") -> str:
    return '{ "level": 0 }'


def packed_scene_create_instance(scene_cache_id: int):
    pass


def scene_util_load_scene(path: str) -> int:
    return 0
