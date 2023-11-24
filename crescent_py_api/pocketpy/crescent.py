import math
from typing import List, Callable, Tuple, Optional, Dict

import crescent_internal


# COLOR
class Color:
    def __init__(self, r=0, g=0, b=0, a=255):
        self.r = r
        self.g = g
        self.b = b
        self.a = a

    def __str__(self):
        return f"({self.r}, {self.g}, {self.b}, {self.a})"

    def __repr__(self):
        return f"({self.r}, {self.g}, {self.b}, {self.a})"

    def copy(self) -> "Color":
        return Color(self.r, self.g, self.b, self.a)

    @staticmethod
    def linear_color(r: float, g: float, b: float, a=1.0) -> "Color":
        return Color(int(r * 255), int(g * 255), int(b * 255), int(a * 255))

    @staticmethod
    def BLACK() -> "Color":
        return Color(0, 0, 0)

    @staticmethod
    def WHITE() -> "Color":
        return Color(255, 255, 255)

    @staticmethod
    def RED() -> "Color":
        return Color(255, 0, 0)

    @staticmethod
    def GREEN() -> "Color":
        return Color(0, 255, 0)

    @staticmethod
    def BLUE() -> "Color":
        return Color(0, 0, 255)


# MATH
class Math:
    PI = 3.14159

    @staticmethod
    def lerp(source: float, destination: float, amount: float) -> float:
        return source + (destination - source) * amount


class Vector2:
    def __init__(self, x=0.0, y=0.0):
        self.x = x
        self.y = y

    def dot_product(self, other) -> float:
        return (self.x * other.x) + (self.y * other.y)

    def __eq__(self, other) -> bool:
        if self.x == other.x and self.y == other.y:
            return True
        else:
            return False

    # TODO: Make a not terrible hash function...
    def __hash__(self):
        return hash(self.x + (self.y * 0.2))

    def __gt__(self, other) -> bool:
        if self.x + self.y > other.x + other.y:
            return True
        else:
            return False

    def __lt__(self, other) -> bool:
        if self.x + self.y < other.x + other.y:
            return True
        else:
            return False

    def __sub__(self, other):
        return Vector2(self.x - other.x, self.y - other.y)

    def __add__(self, other):
        return Vector2(self.x + other.x, self.y + other.y)

    def __mul__(self, other):
        return Vector2(self.x * other.x, self.y * other.y)

    def __truediv__(self, other):
        return Vector2(self.x / other.x, self.y / other.y)

    def __str__(self):
        return f"({self.x}, {self.y})"

    def __repr__(self):
        return f"({self.x}, {self.y})"

    def magnitude(self) -> float:
        return math.sqrt(self.x * self.x + self.y * self.y)

    def normalized(self) -> "Vector2":
        mag = self.magnitude()
        self.x = self.x / mag
        self.y = self.y / mag
        return self

    def direction_to(self, target: "Vector2") -> "Vector2":
        return (target - self).normalized()

    def distance_to(self, target: "Vector2") -> float:
        return math.sqrt(
            (self.x - target.x) * (self.x - target.x) + (self.y - target.y) * (self.y - target.y)
        )

    def copy(self) -> "Vector2":
        return Vector2(self.x, self.y)

    @staticmethod
    def lerp(source, destination, amount: float) -> float:
        return source + (destination - source) * Vector2(amount, amount)

    @staticmethod
    def ZERO() -> "Vector2":
        return Vector2(0.0, 0.0)

    @staticmethod
    def LEFT() -> "Vector2":
        return Vector2(-1.0, 0.0)

    @staticmethod
    def RIGHT() -> "Vector2":
        return Vector2(1.0, 0.0)

    @staticmethod
    def UP() -> "Vector2":
        return Vector2(0.0, -1.0)

    @staticmethod
    def DOWN() -> "Vector2":
        return Vector2(0.0, 1.0)


class Vector3:
    def __init__(self, x=0.0, y=0.0, z=0.0):
        self.x = x
        self.y = y
        self.z = z

    def __eq__(self, other) -> bool:
        if self.x == other.x and self.y == other.y and self.z == other.z:
            return True
        else:
            return False

    def __gt__(self, other) -> bool:
        if self.x + self.y + self.z > other.x + other.y + other.z:
            return True
        else:
            return False

    def __lt__(self, other) -> bool:
        if self.x + self.y + self.z < other.x + other.y + other.z:
            return True
        else:
            return False

    def __sub__(self, other):
        return Vector3(self.x - other.x, self.y - other.y, self.z - other.z)

    def __add__(self, other):
        return Vector3(self.x + other.x, self.y + other.y, self.z + other.z)

    def __mul__(self, other):
        return Vector3(self.x * other.x, self.y * other.y, self.z * other.z)

    def __truediv__(self, other):
        return Vector3(self.x / other.x, self.y / other.y, self.z / other.z)

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z})"

    def __repr__(self):
        return f"({self.x}, {self.y}, {self.z})"


class Vector4:
    def __init__(self, x=0.0, y=0.0, z=0.0, w=0.0):
        self.x = x
        self.y = y
        self.z = z
        self.w = w

    def __eq__(self, other) -> bool:
        if self.x == other.x and self.y == other.y and self.z == other.z and self.w == other.w:
            return True
        else:
            return False

    def __gt__(self, other) -> bool:
        if self.x + self.y + self.z + self.w > other.x + other.y + other.z + other.w:
            return True
        else:
            return False

    def __lt__(self, other) -> bool:
        if self.x + self.y + self.z + self.w < other.x + other.y + other.z + other.w:
            return True
        else:
            return False

    def __sub__(self, other):
        return Vector4(
            self.x - other.x, self.y - other.y, self.z - other.z, self.w - other.w
        )

    def __add__(self, other):
        return Vector4(
            self.x + other.x, self.y + other.y, self.z + other.z, self.w + other.w
        )

    def __mul__(self, other):
        return Vector4(
            self.x * other.x, self.y * other.y, self.z * other.z, self.w * other.w
        )

    def __truediv__(self, other):
        return Vector4(
            self.x / other.x, self.y / other.y, self.z / other.z, self.w / other.w
        )

    def __str__(self):
        return f"({self.x}, {self.y}, {self.z}, {self.w})"

    def __repr__(self):
        return f"({self.x}, {self.y}, {self.z}, {self.w})"


class Size2D:
    def __init__(self, w=0.0, h=0.0):
        self.w = w
        self.h = h

    def __eq__(self, other) -> bool:
        if self.w == other.w and self.h == other.h:
            return True
        else:
            return False

    def __gt__(self, other) -> bool:
        if self.total_length() > other.total_length():
            return True
        else:
            return False

    def __lt__(self, other) -> bool:
        if self.total_length() < other.total_length():
            return True
        else:
            return False

    def __sub__(self, other):
        return Size2D(self.w - other.w, self.h - other.h)

    def __add__(self, other):
        return Size2D(self.w + other.w, self.h + other.h)

    def __mul__(self, other):
        return Size2D(self.w * other.w, self.h * other.h)

    def __truediv__(self, other):
        return Size2D(self.w / other.w, self.h / other.h)

    def __str__(self):
        return f"({self.w}, {self.h})"

    def __repr__(self):
        return f"({self.w}, {self.h})"

    def total_length(self) -> float:
        return self.w + self.h

    def to_vec2(self) -> Vector2:
        return Vector2(self.w, self.h)


class Rect2:
    def __init__(self, x=0.0, y=0.0, w=0.0, h=0.0):
        self.x = x
        self.y = y
        self.w = w
        self.h = h

    def __eq__(self, other) -> bool:
        if self.x == other.x and self.y == other.y and self.w == other.w and self.h == other.h:
            return True
        else:
            return False

    def __gt__(self, other) -> bool:
        if self.total_length() > other.total_length():
            return True
        else:
            return False

    def __lt__(self, other) -> bool:
        if self.total_length() < other.total_length():
            return True
        else:
            return False

    def __sub__(self, other):
        return Rect2(
            self.x - other.x, self.y - other.y, self.w - other.w, self.h - other.h
        )

    def __add__(self, other):
        return Rect2(
            self.x + other.x, self.y + other.y, self.w + other.w, self.h + other.h
        )

    def __mul__(self, other):
        return Rect2(
            self.x * other.x, self.y * other.y, self.w * other.w, self.h * other.h
        )

    def __truediv__(self, other):
        return Rect2(
            self.x / other.x, self.y / other.y, self.w / other.w, self.h / other.h
        )

    def __str__(self):
        return f"({self.x}, {self.y}, {self.w}, {self.h})"

    def __repr__(self):
        return f"({self.x}, {self.y}, {self.w}, {self.h})"

    @property
    def position(self) -> Vector2:
        return Vector2(self.x, self.y)

    @position.setter
    def position(self, value: Vector2) -> None:
        self.x = value.x
        self.y = value.y

    @property
    def size(self) -> Size2D:
        return Size2D(self.w, self.h)

    @size.setter
    def size(self, value: Size2D) -> None:
        self.w = value.w
        self.h = value.h

    def total_length(self) -> float:
        return self.x + self.y + self.w + self.h

    def contains_point(self, point: Vector2) -> bool:
        return self.x <= point.x <= self.w and self.y <= point.y <= self.h


class MinMax:
    def __init__(self, value_min: float, value_max: float):
        self.min = value_min
        self.max = value_max

    def contain(self, value: float) -> bool:
        return self.min <= value <= self.max

    def is_below(self, value: float) -> bool:
        return value < self.min

    def is_above(self, value: float) -> bool:
        return value > self.max

    def __str__(self):
        return f"({self.min}, {self.max})"

    def __repr__(self):
        return f"({self.min}, {self.max})"


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
            sub.call_back(*args)

    def subscribe_to_event(self, owner_entity: int, event_name: str, subscriber_entity: int, subscriber_call_back: Callable[[...], None]) -> None:
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

    def subscribe_to_event(self, event_name: str, subscriber: "Node", callback_func: Callable[[...], None]) -> None:
        _node_event_manager.subscribe_to_event(self.entity_id, event_name, subscriber.entity_id, callback_func)

    def broadcast_event(self, event_name: str, *args) -> None:
        _node_event_manager.broadcast_event(self.entity_id, event_name, *args)

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
