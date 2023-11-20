#pragma once

#define CRE_PKPY_CRESCENT_SOURCE ""\
"from typing import List, Callable, Tuple, Optional\n"\
"\n"\
"import crescent_internal\n"\
"\n"\
"\n"\
"class Node:\n"\
"    def __init__(self, entity_id: int) -> None:\n"\
"        self.entity_id = entity_id\n"\
"\n"\
"    # TODO: Replace with proper method once class method is added\n"\
"    # @staticmethod\n"\
"    # def new() -> \"Node\":\n"\
"    #     return Node(0)\n"\
"\n"\
"    @staticmethod\n"\
"    def new(class_path: str, class_name: str) -> \"Node\":\n"\
"        return crescent_internal.node_new(class_path, class_name, 1)\n"\
"\n"\
"    def get_name(self) -> str:\n"\
"        return crescent_internal.node_get_name(self.entity_id)\n"\
"\n"\
"    @property\n"\
"    def name(self) -> str:\n"\
"        return crescent_internal.node_get_name(self.entity_id)\n"\
"\n"\
"    def add_child(self, child_node: \"Node\") -> None:\n"\
"        crescent_internal.node_add_child(self.entity_id, child_node.entity_id)\n"\
"\n"\
"    def get_child(self, child_name: str) -> Optional[\"Node\"]:\n"\
"        return crescent_internal.node_get_child(self.entity_id, child_name)\n"\
"\n"\
"    def get_children(self) -> List[\"Node\"]:\n"\
"        children = crescent_internal.node_get_children(self.entity_id)\n"\
"        if children:\n"\
"            return list(children)\n"\
"        return []\n"\
"\n"\
"    def get_parent(self) -> Optional[\"Node\"]:\n"\
"        return crescent_internal.node_get_parent(self.entity_id)\n"\
"\n"\
"    def queue_deletion(self) -> None:\n"\
"        crescent_internal.node_queue_deletion(self.entity_id)\n"\
"\n"\
"    def is_queued_for_deletion(self) -> bool:\n"\
"        return crescent_internal.node_is_queued_for_deletion(self.entity_id)\n"\
"\n"\
"    def get_time_dilation(self) -> float:\n"\
"        return crescent_internal.node_get_time_dilation(self.entity_id)\n"\
"\n"\
"    def set_time_dilation(self, dilation: float) -> None:\n"\
"        crescent_internal.node_set_time_dilation(self.entity_id, dilation)\n"\
"\n"\
"    @property\n"\
"    def time_dilation(self) -> float:\n"\
"        return crescent_internal.node_get_time_dilation(self.entity_id)\n"\
"\n"\
"    @time_dilation.setter\n"\
"    def time_dilation(self, value: float) -> None:\n"\
"        crescent_internal.node_set_time_dilation(self.entity_id, value)\n"\
"\n"\
"    def get_total_time_dilation(self) -> float:\n"\
"        return crescent_internal.node_get_total_time_dilation(self.entity_id)\n"\
"\n"\
"    def create_event(self, event_name: str) -> None:\n"\
"        pass\n"\
"\n"\
"    def subscribe_to_event(self, event_name: str, subscriber: \"Node\", callback_func: Callable[[Tuple], None]) -> None:\n"\
"        pass\n"\
"\n"\
"    def broadcast_event(self, event_name: str, *args) -> None:\n"\
"        pass\n"\
"\n"\
"    def __eq__(self, other: \"Node\") -> bool:\n"\
"        return self.entity_id == other.entity_id\n"\
"\n"\
"    def __str__(self) -> str:\n"\
"        return f\"Node(entity_id: {self.entity_id})\"\n"\
"\n"\
"    def __repr__(self) -> str:\n"\
"        return f\"Node(entity_id: {self.entity_id})\"\n"\
"\n"\
"\n"\
"class SceneTree:\n"\
"    @staticmethod\n"\
"    def change_scene(path: str) -> None:\n"\
"        crescent_internal.scene_tree_change_scene(path)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_root() -> Optional[Node]:\n"\
"        return crescent_internal.scene_tree_get_root()\n"\
"\n"

#define CRE_PKPY_CRESCENT_INTERNAL_PY_SOURCE ""\
"from typing import Dict, List, Optional, Callable, Tuple\n"\
"\n"\
"from crescent import Node\n"\
"\n"\
"# Engine's python api used for internal functionality\n"\
"\n"\
"CRE_ENTITY_TO_NODE_MAP: Dict[int, Node] = {}\n"\
"\n"\
"\n"\
"def get_entity(entity_id: int) -> Node:\n"\
"    return CRE_ENTITY_TO_NODE_MAP[entity_id]\n"\
"\n"\
"\n"\
"def set_entity(node: Node) -> None:\n"\
"    CRE_ENTITY_TO_NODE_MAP[node.entity_id] = node\n"\
"\n"\
"\n"\
"class _NodeEventSubscriber:\n"\
"    def __init__(self, entity_id: int, call_back: Callable[[Tuple], None], event_owner_entity_id: int, event_name: str) -> None:\n"\
"        self.entity_id = entity_id\n"\
"        self.call_back = call_back\n"\
"        self.event_owner_entity_id = event_owner_entity_id\n"\
"        self.event_name = event_name\n"\
"\n"\
"\n"\
"class _NodeEvent:\n"\
"    def __init__(self, entity_id: int, name: str, subscribers: List[_NodeEventSubscriber] = None) -> None:\n"\
"        self.entity_id = entity_id\n"\
"        self.name = name\n"\
"        if not subscribers:\n"\
"            subscribers = []\n"\
"        self.subscribers = subscribers\n"\
"\n"\
"    def add_or_update_subscriber(self, entity_id: int, call_back: Callable[[Tuple], None]) -> _NodeEventSubscriber:\n"\
"        for sub in self.subscribers:\n"\
"            if entity_id == sub.entity_id:\n"\
"                sub.call_back = call_back\n"\
"                return sub\n"\
"        subscriber = _NodeEventSubscriber(\n"\
"            entity_id, call_back, self.entity_id, self.name\n"\
"        )\n"\
"        self.subscribers.append(subscriber)\n"\
"        return subscriber\n"\
"\n"\
"    def remove_subscriber(self, subscribe_entity_id: int) -> None:\n"\
"        for sub in self.subscribers:\n"\
"            if subscribe_entity_id == sub.entity_id:\n"\
"                self.subscribers.remove(sub)\n"\
"                break\n"\
"\n"\
"\n"\
"class _NodeEventManager:\n"\
"    def __init__(self, events: Dict[int, Dict[str, _NodeEvent]] = None, entity_subscribers: Dict[int, List[_NodeEventSubscriber]] = None) -> None:\n"\
"        if not events:\n"\
"            events = {}\n"\
"        if not entity_subscribers:\n"\
"            entity_subscribers = {}\n"\
"        self.events = events\n"\
"        self.entity_subscribers = entity_subscribers\n"\
"\n"\
"    def _get_entity_events(self, entity_id: int) -> Dict[str, _NodeEvent]:\n"\
"        entity_events = self.events.get(entity_id, {})\n"\
"        return entity_events\n"\
"\n"\
"    def create_event(self, entity_id: int, event_name: str) -> _NodeEvent:\n"\
"        if not self.has_event(entity_id, event_name):\n"\
"            entity_events = self._get_entity_events(entity_id)\n"\
"            entity_events[event_name] = _NodeEvent(entity_id, event_name)\n"\
"            return entity_events[event_name]\n"\
"        return self.events[entity_id][event_name]\n"\
"\n"\
"    def remove_event(self, entity_id: int, event_name: str) -> None:\n"\
"        if self.has_event(entity_id, event_name):\n"\
"            entity_events = self._get_entity_events(entity_id)\n"\
"            del entity_events[event_name]\n"\
"\n"\
"    def remove_entity_and_connections(self, entity_id: int) -> None:\n"\
"        if entity_id in self.events:\n"\
"            del self.events[entity_id]\n"\
"        if entity_id in self.entity_subscribers:\n"\
"            for sub in self.entity_subscribers[entity_id]:\n"\
"                event = self.get_event(sub.event_owner_entity_id, sub.event_name)\n"\
"                if event:\n"\
"                    event.remove_subscriber(entity_id)\n"\
"            del self.entity_subscribers[entity_id]\n"\
"\n"\
"    def get_event(self, entity_id: int, event_name: str) -> Optional[_NodeEvent]:\n"\
"        if self.has_event(entity_id, event_name):\n"\
"            return self.events[entity_id][event_name]\n"\
"        return None\n"\
"\n"\
"    def has_event(self, entity_id: int, event_name: str) -> bool:\n"\
"        return entity_id in self.events and event_name in self.events[entity_id]\n"\
"\n"\
"    def broadcast_event(self, entity_id: int, event_name: str, *args) -> None:\n"\
"        event = self.create_event(entity_id, event_name)\n"\
"        for sub in event.subscribers:\n"\
"            sub.call_back(args)\n"\
"\n"\
"    def subscribe_to_event(self, owner_entity: int, event_name: str, subscriber_entity: int, subscriber_call_back: Callable[[Tuple], None]) -> None:\n"\
"        event = self.create_event(owner_entity, event_name)\n"\
"        subscriber = event.add_or_update_subscriber(\n"\
"            subscriber_entity, subscriber_call_back\n"\
"        )\n"\
"        sub_list = self.entity_subscribers.get(subscriber_entity, [])\n"\
"        if subscriber not in sub_list:\n"\
"            sub_list.append(subscriber)\n"\
"\n"

