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
