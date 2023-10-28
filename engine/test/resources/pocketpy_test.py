from typing import List

import crescent_api_internal


class Node:
    def __init__(self, entity_id: int):
        self.entity_id = entity_id

    def get_name(self) -> str:
        return crescent_api_internal.node_get_name(self.entity_id)

    @property
    def name(self) -> str:
        return crescent_api_internal.node_get_name(self.entity_id)

    def get_children(self) -> List["Node"]:
        children_ids = crescent_api_internal.node_get_children(self.entity_id)
        # TODO: Get nodes from internal global dict
        children = []
        for child_id in children_ids:
            children.append(Node(child_id))
        return children

    def __eq__(self, other) -> bool:
        return self.entity_id == other.entity_id

    def __str__(self):
        return f"Node(entity_id: {self.entity_id})"

    def __repr__(self):
        return f"Node(entity_id: {self.entity_id})"


class NodeManager:
    def __init__(self):
        self.test_node = Node(45)

    def test_add(self, x: int, y: int, z: int) -> int:
        return x + y + z
