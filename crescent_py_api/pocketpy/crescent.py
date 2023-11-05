from typing import List

import crescent_internal


class Node:
    def __init__(self, entity_id: int) -> None:
        self.entity_id = entity_id

    # TODO: Replace with proper method once class method is added
    # @staticmethod
    # def new() -> "Node":
    #     return Node(0)

    def get_name(self) -> str:
        return crescent_internal.node_get_name(self.entity_id)

    @property
    def name(self) -> str:
        return crescent_internal.node_get_name(self.entity_id)

    def add_child(self, child_node: "Node") -> None:
        crescent_internal.node_add_child(self.entity_id, child_node.entity_id)

    def get_child(self, child_name: str) -> "Node":
        child_entity_id = crescent_internal.node_get_child(self.entity_id, child_name)
        return Node(child_entity_id)

    def get_children(self) -> List["Node"]:
        children_ids = crescent_internal.node_get_children(self.entity_id)
        # TODO: Get nodes from internal global dict
        children = []
        for child_id in children_ids:
            children.append(Node(child_id))
        return children

    def queue_deletion(self) -> None:
        crescent_internal.node_queue_deletion(self.entity_id)

    def is_queued_for_deletion(self) -> bool:
        return crescent_internal.node_is_queued_for_deletion(self.entity_id)

    def __eq__(self, other: "Node") -> bool:
        return self.entity_id == other.entity_id

    def __str__(self) -> str:
        return f"Node(entity_id: {self.entity_id})"

    def __repr__(self) -> str:
        return f"Node(entity_id: {self.entity_id})"
