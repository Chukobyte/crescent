from typing import Dict, List

from crescent import Node

# Engine's python api used for internal functionality

CRE_ENTITY_TO_NODE_MAP: Dict[int, Node] = {}


def get_entity(entity_id: int) -> Node:
    return CRE_ENTITY_TO_NODE_MAP[entity_id]


def set_entity(node: Node) -> None:
    CRE_ENTITY_TO_NODE_MAP[node.entity_id] = node


class _NodeEventSubscriber:
    def __init__(self, entity_id: int):
        self.entity_id = entity_id


class _NodeEvent:
    def __init__(self, name: str) -> None:
        self.name = name
        self.subscribers: List[_NodeEventSubscriber] = []


class _NodeEventManager:
    def __init__(self) -> None:
        self.events: Dict[int, List[_NodeEvent]] = {}

    def create_event(self, entity_id: int, event_name: str) -> None:
        entity_events = self.events.get(entity_id, [])
        entity_events.append(_NodeEvent(event_name))
