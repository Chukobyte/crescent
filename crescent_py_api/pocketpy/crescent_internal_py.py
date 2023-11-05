from typing import Dict

from crescent import Node

# Engine's python api used for internal functionality

CRE_ENTITY_TO_NODE_MAP: Dict[int, Node] = {}


def get_entity(entity_id: int) -> Node:
    return CRE_ENTITY_TO_NODE_MAP[entity_id]


def set_entity(node: Node) -> None:
    CRE_ENTITY_TO_NODE_MAP[node.entity_id] = node
