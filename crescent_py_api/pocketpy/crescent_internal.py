from typing import Tuple


def node_new(class_path: str, class_name: str, node_type_flag: int) -> int:
    return 0


def node_queue_deletion(entity_id: int) -> None:
    pass


def node_is_queued_for_deletion(entity_id: int) -> bool:
    return False


def node_get_name(entity_id: int) -> str:
    return "Name"


def node_get_children(entity_id) -> Tuple[int, ...]:
    return ()
