from typing import Dict, List, Optional, Callable, Tuple

from crescent import Node

# Engine's python api used for internal functionality

CRE_ENTITY_TO_NODE_MAP: Dict[int, Node] = {}


def get_entity(entity_id: int) -> Node:
    return CRE_ENTITY_TO_NODE_MAP[entity_id]


def set_entity(node: Node) -> None:
    CRE_ENTITY_TO_NODE_MAP[node.entity_id] = node


class _NodeEventSubscriber:
    def __init__(self, entity_id: int, call_back: Callable[[Tuple], None], event_owner_entity_id: int, event_name: str) -> None:
        self.entity_id = entity_id
        self.call_back = call_back
        self.event_owner_entity_id = event_owner_entity_id
        self.event_name = event_name


class _NodeEvent:
    def __init__(self, entity_id: int, name: str, subscribers: List[_NodeEventSubscriber] = None) -> None:
        self.entity_id = entity_id
        self.name = name
        if not subscribers:
            subscribers = []
        self.subscribers = subscribers

    def add_or_update_subscriber(self, entity_id: int, call_back: Callable[[Tuple], None]) -> _NodeEventSubscriber:
        for sub in self.subscribers:
            if entity_id == sub.entity_id:
                sub.call_back = call_back
                return sub
        subscriber = _NodeEventSubscriber(
            entity_id, call_back, self.entity_id, self.name
        )
        self.subscribers.append(subscriber)
        return subscriber

    def remove_subscriber(self, subscribe_entity_id: int) -> None:
        for sub in self.subscribers:
            if subscribe_entity_id == sub.entity_id:
                self.subscribers.remove(sub)
                break


class _NodeEventManager:
    def __init__(self, events: Dict[int, Dict[str, _NodeEvent]] = None, entity_subscribers: Dict[int, List[_NodeEventSubscriber]] = None) -> None:
        if not events:
            events = {}
        if not entity_subscribers:
            entity_subscribers = {}
        self.events = events
        self.entity_subscribers = entity_subscribers

    def create_event(self, entity_id: int, event_name: str) -> _NodeEvent:
        if entity_id not in self.events:
            self.events[entity_id] = {}
        if event_name not in self.events[entity_id]:
            self.events[entity_id][event_name] = _NodeEvent(entity_id, event_name)
        return self.events[entity_id][event_name]

    def remove_event(self, entity_id: int, event_name: str) -> None:
        if self.has_event(entity_id, event_name):
            del self.events[entity_id][event_name]

    def remove_entity_and_connections(self, entity_id: int) -> None:
        if entity_id in self.events:
            del self.events[entity_id]
        if entity_id in self.entity_subscribers:
            for sub in self.entity_subscribers[entity_id]:
                event = self.get_event(sub.event_owner_entity_id, sub.event_name)
                if event:
                    event.remove_subscriber(entity_id)
            del self.entity_subscribers[entity_id]

    def get_event(self, entity_id: int, event_name: str) -> Optional[_NodeEvent]:
        if self.has_event(entity_id, event_name):
            return self.events[entity_id][event_name]
        return None

    def has_event(self, entity_id: int, event_name: str) -> bool:
        return entity_id in self.events and event_name in self.events[entity_id]

    def broadcast_event(self, entity_id: int, event_name: str, *args) -> None:
        event = self.create_event(entity_id, event_name)
        for sub in event.subscribers:
            sub.call_back(args)

    def subscribe_to_event(self, owner_entity: int, event_name: str, subscriber_entity: int, subscriber_call_back: Callable[[Tuple], None]) -> None:
        event = self.create_event(owner_entity, event_name)
        subscriber = event.add_or_update_subscriber(
            subscriber_entity, subscriber_call_back
        )
        if subscriber_entity not in self.entity_subscribers:
            self.entity_subscribers[subscriber_entity] = []
        sub_list = self.entity_subscribers.get(subscriber_entity, [])
        if subscriber not in sub_list:
            sub_list.append(subscriber)
