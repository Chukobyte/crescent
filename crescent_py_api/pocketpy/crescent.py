from typing import List, Callable, Tuple, Optional

import crescent_internal


class NodeType:
    Node = 1
    Node2D = 3


class Node:
    def __init__(self, entity_id: int) -> None:
        self.entity_id = entity_id

    # TODO: Replace with proper method once class method is added
    @staticmethod
    def new() -> "Node":
        return crescent_internal.node_new("crescent", "Node", NodeType.Node)

    def get_name(self) -> str:
        return crescent_internal.node_get_name(self.entity_id)

    @property
    def name(self) -> str:
        return crescent_internal.node_get_name(self.entity_id)

    def add_child(self, child_node: "Node") -> None:
        crescent_internal.node_add_child(self.entity_id, child_node.entity_id)

    def get_child(self, child_name: str) -> Optional["Node"]:
        return crescent_internal.node_get_child(self.entity_id, child_name)

    def get_children(self) -> List["Node"]:
        children = crescent_internal.node_get_children(self.entity_id)
        if children:
            if isinstance(children, tuple):
                return list(children)
            elif isinstance(children, Node):
                return [children]
        return []

    def get_parent(self) -> Optional["Node"]:
        return crescent_internal.node_get_parent(self.entity_id)

    def queue_deletion(self) -> None:
        crescent_internal.node_queue_deletion(self.entity_id)

    def is_queued_for_deletion(self) -> bool:
        return crescent_internal.node_is_queued_for_deletion(self.entity_id)

    def get_time_dilation(self) -> float:
        return crescent_internal.node_get_time_dilation(self.entity_id)

    def set_time_dilation(self, dilation: float) -> None:
        crescent_internal.node_set_time_dilation(self.entity_id, dilation)

    @property
    def time_dilation(self) -> float:
        return crescent_internal.node_get_time_dilation(self.entity_id)

    @time_dilation.setter
    def time_dilation(self, value: float) -> None:
        crescent_internal.node_set_time_dilation(self.entity_id, value)

    def get_total_time_dilation(self) -> float:
        return crescent_internal.node_get_total_time_dilation(self.entity_id)

    def create_event(self, event_name: str) -> None:
        pass

    def subscribe_to_event(self, event_name: str, subscriber: "Node", callback_func: Callable[[Tuple], None]) -> None:
        pass

    def broadcast_event(self, event_name: str, *args) -> None:
        pass

    def __eq__(self, other: "Node") -> bool:
        return self.entity_id == other.entity_id

    def __str__(self) -> str:
        return f"{self.__class__.__name__}(entity_id: {self.entity_id})"

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}(entity_id: {self.entity_id})"


class Node2D(Node):
    @staticmethod
    def new() -> "Node2D":
        return crescent_internal.node_new("crescent", "Node2D", NodeType.Node2D)

    def _start(self) -> None:
        print(f"Called start on {self}")


class SceneTree:
    @staticmethod
    def change_scene(path: str) -> None:
        crescent_internal.scene_tree_change_scene(path)

    @staticmethod
    def get_root() -> Optional[Node]:
        return crescent_internal.scene_tree_get_root()
