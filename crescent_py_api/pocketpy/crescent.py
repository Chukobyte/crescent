from typing import List, Callable, Tuple, Optional, Dict

import crescent_internal


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
    """
    Used to manage events between nodes
    """
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

    def clear_all_data(self) -> None:
        self.events.clear()
        self.entity_subscribers.clear()


_node_event_manager = _NodeEventManager()


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
        _node_event_manager.create_event(self.entity_id, event_name)

    def subscribe_to_event(self, event_name: str, subscriber: "Node", callback_func: Callable[[Tuple], None]) -> None:
        _node_event_manager.subscribe_to_event(self.entity_id, event_name, subscriber.entity_id, callback_func)

    def broadcast_event(self, event_name: str, *args) -> None:
        _node_event_manager.broadcast_event(self.entity_id, event_name)

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


class SceneTree:
    @staticmethod
    def change_scene(path: str) -> None:
        crescent_internal.scene_tree_change_scene(path)

    @staticmethod
    def get_root() -> Optional[Node]:
        return crescent_internal.scene_tree_get_root()
