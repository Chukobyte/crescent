from typing import Tuple

# Mock class for engine's internal c python module


def node_new(class_path: str, class_name: str, node_type_flag: int) -> int:
    return 0


def node_get_name(entity_id: int) -> str:
    return "Name"


def node_add_child(parent_entity_id: int, child_entity_id: int) -> None:
    pass


def node_get_child(parent_entity_id: int, child_entity_name: str) -> int:
    return 0


def node_get_children(entity_id) -> Tuple[int, ...]:
    return ()


def node_queue_deletion(entity_id: int) -> None:
    pass


def node_is_queued_for_deletion(entity_id: int) -> bool:
    return False
