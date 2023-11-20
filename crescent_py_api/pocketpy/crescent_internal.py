from typing import Tuple, Optional


# Mock class for engine's internal c python module


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


# --- SCENE TREE --- #
def scene_tree_change_scene(path: str) -> None:
    pass


def scene_tree_get_root() -> Optional["Node"]:
    return None
