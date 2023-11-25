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
