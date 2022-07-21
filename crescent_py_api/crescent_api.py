from enum import Enum
from typing import Optional

import crescent_api_internal

import math


# COLOR
class Color:
    # 255 is the limit...
    def __init__(self, r=0, g=0, b=0, a=255):
        self.r = r
        self.g = g
        self.b = b
        self.a = a

    def __str__(self):
        return f"({self.r}, {self.g}, {self.b}, {self.a})"

    def __repr__(self):
        return f"({self.r}, {self.g}, {self.b}, {self.a})"


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

    def normalized(self):
        mag = self.magnitude()
        self.x = self.x / mag
        self.y = self.y / mag
        return self

    def direction_to(self, target):
        return (target - self).normalized()

    @staticmethod
    def lerp(source, destination, amount: float) -> float:
        return source + (destination - source) * Vector2(amount, amount)

    @staticmethod
    def ZERO():
        return Vector2(0.0, 0.0)

    @staticmethod
    def LEFT():
        return Vector2(-1.0, 0.0)

    @staticmethod
    def RIGHT():
        return Vector2(1.0, 0.0)

    @staticmethod
    def UP():
        return Vector2(0.0, -1.0)

    @staticmethod
    def DOWN():
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


class Rect2:
    def __init__(self, x=0.0, y=0.0, w=0.0, h=0.0):
        self.x = x
        self.y = y
        self.w = w
        self.h = h

    def __eq__(self, other) -> bool:
        if (
            self.x == other.x
            and self.y == other.y
            and self.w == other.w
            and self.h == other.h
        ):
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
    def size(self) -> Vector2:
        return Vector2(self.w, self.h)

    @size.setter
    def size(self, value: Vector2) -> None:
        self.w = value.x
        self.h = value.y

    def total_length(self) -> float:
        return self.x + self.y + self.w + self.h


# ASSETS
class AudioSource:
    def __init__(self, file_path: str):
        self.file_path = file_path


class Texture:
    def __init__(
        self,
        file_path: str,
        wrap_s="clamp_to_border",
        wrap_t="clamp_to_border",
        filter_min="nearest",
        filter_mag="nearest",
    ):
        self.file_path = file_path
        self.wrap_s = wrap_s
        self.wrap_t = wrap_t
        self.filter_min = filter_min
        self.filter_mag = filter_mag

    def __str__(self):
        return f"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, filter_min: {self.filter_min}, filter_mag: {self.filter_mag})"

    def __repr__(self):
        return f"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, filter_min: {self.filter_min}, filter_mag: {self.filter_mag})"


class Font:
    def __init__(self, file_path: str, uid: str, size: int):
        self.file_path = file_path
        self.uid = uid
        self.size = size


class InputAction:
    def __init__(self, name: str, values: list, device_id=0):
        self.name = name
        self.values = values
        self.device_id = device_id


class AnimationFrame:
    def __init__(self, frame: int, texture_path: str, draw_source: Rect2):
        self.frame = frame
        self.texture_path = texture_path
        self.draw_source = draw_source


class Animation:
    def __init__(self, name: str, speed: int, loops: bool, frames: list):
        self.name = name
        self.speed = speed
        self.loops = loops
        self.frames = frames


# ENGINE
class Engine:
    @staticmethod
    def exit(code=0) -> None:
        crescent_api_internal.engine_exit(code=code)

    @staticmethod
    def set_target_fps(fps: int) -> None:
        crescent_api_internal.engine_set_target_fps(fps=fps)

    @staticmethod
    def get_target_fps() -> int:
        return crescent_api_internal.engine_get_target_fps()

    @staticmethod
    def get_average_fps() -> float:
        return crescent_api_internal.engine_get_average_fps()

    @staticmethod
    def set_fps_display_enabled(enabled: bool) -> None:
        crescent_api_internal.engine_set_fps_display_enabled(enabled=enabled)


# INPUT
class Input:
    @staticmethod
    def add_action(name: str, value: str, device_id=0) -> None:
        crescent_api_internal.input_add_action(
            name=name, value=value, device_id=device_id
        )

    @staticmethod
    def is_action_pressed(name: str) -> bool:
        return crescent_api_internal.input_is_action_pressed(name=name)

    @staticmethod
    def is_action_just_pressed(name: str) -> bool:
        return crescent_api_internal.input_is_action_just_pressed(name=name)

    @staticmethod
    def is_action_just_released(name: str) -> bool:
        return crescent_api_internal.input_is_action_just_released(name=name)

    class Mouse:
        LEFT_BUTTON = "mb_left"
        RIGHT_BUTTON = "mb_right"

        @staticmethod
        def get_position() -> Vector2:
            mouse_x, mouse_y = crescent_api_internal.mouse_get_position()
            return Vector2(mouse_x, mouse_y)

    class Keyboard:
        NUM_0 = "0"
        NUM_1 = "1"
        NUM_2 = "2"
        NUM_3 = "3"
        NUM_4 = "4"
        NUM_5 = "5"
        NUM_6 = "6"
        NUM_7 = "7"
        NUM_8 = "8"
        NUM_9 = "9"

        A = "a"
        B = "b"
        C = "c"
        D = "d"
        E = "e"
        F = "f"
        G = "g"
        H = "h"
        I = "i"
        J = "j"
        K = "k"
        L = "l"
        M = "m"
        N = "n"
        O = "o"
        P = "p"
        Q = "q"
        R = "r"
        S = "s"
        T = "t"
        U = "u"
        V = "v"
        W = "w"
        X = "x"
        Y = "y"
        Z = "z"

        UP = "up"
        DOWN = "down"
        LEFT = "left"
        RIGHT = "right"
        SPACE = "space"
        ESC = "esc"
        RETURN = "return"

        F1 = "f1"
        F2 = "f2"
        F3 = "f3"
        F4 = "f4"
        F5 = "f5"
        F6 = "f6"
        F7 = "f7"
        F8 = "f8"
        F9 = "f9"
        F10 = "f10"
        F11 = "f11"
        F12 = "f12"

    class Gamepad:
        BUTTON_A = "joystick_button_a"
        BUTTON_B = "joystick_button_b"
        BUTTON_X = "joystick_button_x"
        BUTTON_Y = "joystick_button_y"
        BUTTON_START = "joystick_start"
        BUTTON_BACK = "joystick_back"
        BUTTON_DPAD_LEFT = "joystick_dpad_left"
        BUTTON_DPAD_RIGHT = "joystick_dpad_right"
        BUTTON_DPAD_UP = "joystick_dpad_up"
        BUTTON_DPAD_DOWN = "joystick_dpad_down"
        BUTTON_LEFT_SHOULDER = "joystick_left_shoulder"
        BUTTON_RIGHT_SHOULDER = "joystick_right_shoulder"
        BUTTON_LEFT_ANALOG = "joystick_left_analog"
        BUTTON_RIGHT_ANALOG = "joystick_right_analog"

        AXIS_LEFT_TRIGGER = "joystick_left_trigger"
        AXIS_RIGHT_TRIGGER = "joystick_right_trigger"
        AXIS_LEFT_ANALOG_LEFT = "joystick_left_analog_left"
        AXIS_LEFT_ANALOG_RIGHT = "joystick_left_analog_right"
        AXIS_LEFT_ANALOG_UP = "joystick_left_analog_up"
        AXIS_LEFT_ANALOG_DOWN = "joystick_left_analog_down"
        AXIS_RIGHT_ANALOG_LEFT = "joystick_right_analog_left"
        AXIS_RIGHT_ANALOG_RIGHT = "joystick_right_analog_right"
        AXIS_RIGHT_ANALOG_UP = "joystick_right_analog_up"
        AXIS_RIGHT_ANALOG_DOWN = "joystick_right_analog_down"


# CAMERA
class Camera2D:
    @staticmethod
    def set_position(position: Vector2) -> None:
        crescent_api_internal.camera2D_set_position(x=position.x, y=position.y)

    @staticmethod
    def add_to_position(position: Vector2) -> None:
        crescent_api_internal.camera2D_add_to_position(x=position.x, y=position.y)

    @staticmethod
    def get_position() -> Vector2:
        x, y = crescent_api_internal.camera2D_get_position()

    @staticmethod
    def set_offset(offset: Vector2) -> None:
        crescent_api_internal.camera2D_set_offset(x=offset.x, y=offset.y)

    @staticmethod
    def add_to_offset(offset: Vector2) -> None:
        crescent_api_internal.camera2D_add_to_offset(x=offset.x, y=offset.y)

    @staticmethod
    def get_offset() -> Vector2:
        x, y = crescent_api_internal.camera2D_get_offset()

    @staticmethod
    def set_zoom(zoom: Vector2) -> None:
        crescent_api_internal.camera2D_set_zoom(x=zoom.x, y=zoom.y)

    @staticmethod
    def add_to_zoom(zoom: Vector2) -> None:
        crescent_api_internal.camera2D_add_to_zoom(x=zoom.x, y=zoom.y)

    @staticmethod
    def get_zoom() -> Vector2:
        x, y = crescent_api_internal.camera2D_get_zoom()

    @staticmethod
    def set_boundary(boundary: Rect2) -> None:
        crescent_api_internal.camera2D_set_boundary(
            x=boundary.x, y=boundary.y, w=boundary.w, h=boundary.h
        )

    @staticmethod
    def get_boundary() -> Rect2:
        x, y, w, h = crescent_api_internal.camera2D_get_boundary()
        return Rect2(x=x, y=y, w=w, h=h)


# STAGE SETUP
class StageNode:
    def __init__(
        self, name: str, type: str, tags, external_node_source, components, children
    ):
        self.name = name
        self.type = type
        self.tags = tags
        self.external_node_source = external_node_source
        self.components = components
        self.children = children


# COMPONENTS
class Transform2DComponent:
    def __init__(
        self,
        position: Vector2,
        scale: Vector2,
        rotation: float,
        z_index: int,
        z_index_relative_to_parent: bool,
        ignore_camera: bool,
    ):
        self.position = position
        self.scale = scale
        self.rotation = rotation
        self.z_index = z_index
        self.z_index_relative_to_parent = z_index_relative_to_parent
        self.ignore_camera = ignore_camera


class SpriteComponent:
    def __init__(
        self,
        texture_path: str,
        draw_source: Rect2,
        origin=Vector2.ZERO(),
        flip_x=False,
        flip_y=False,
        modulate=Color(255, 255, 255),
    ):
        self.texture_path = texture_path
        self.draw_source = draw_source
        self.origin = origin
        self.flip_x = flip_x
        self.flip_y = flip_y
        self.modulate = modulate


class AnimatedSpriteComponent:
    def __init__(
        self,
        current_animation_name: str,
        is_playing: bool,
        animations: list,
        origin=Vector2.ZERO(),
        flip_x=False,
        flip_y=False,
    ):
        self.current_animation_name = current_animation_name
        self.is_playing = is_playing
        self.animations = animations
        self.origin = origin
        self.flip_x = flip_x
        self.flip_y = flip_y


class TextLabelComponent:
    def __init__(
        self,
        uid: str,
        text: str,
        color: Color,
    ):
        self.uid = uid
        self.text = text
        self.color = color


class ScriptComponent:
    def __init__(self, class_path: str, class_name: str):
        self.class_path = class_path
        self.class_name = class_name


class Collider2DComponent:
    def __init__(self, extents: Size2D, color: Color):
        self.extents = extents
        self.color = color


# NODE
class NodeType(str, Enum):
    NODE = "Node"
    TIMER = "Timer"
    NODE2D = "Node2D"
    SPRITE = "Sprite"
    ANIMATED_SPRITE = "AnimatedSprite"
    TEXT_LABEL = "TextLabel"
    COLLIDER2D = "Collider2D"


class Node:
    def __init__(self, entity_id: int) -> None:
        self.entity_id = entity_id

    def __eq__(self, other) -> bool:
        if self.entity_id == other.entity_id:
            return True
        else:
            return False

    def __str__(self):
        return f"Node(entity_id: {self.entity_id}, type: {type(self).__name__})"

    def __repr__(self):
        return f"Node(entity_id: {self.entity_id}, type: {type(self).__name__})"

    # New API
    def get_child(self, name: str):
        node = crescent_api_internal.node_get_child(
            entity_id=self.entity_id, child_name=name
        )
        return self.parse_scene_node_from_engine(scene_node=node)

    def get_children(self) -> list:
        children_nodes = []
        children = crescent_api_internal.node_get_children(entity_id=self.entity_id)
        for child_node in children:
            children_nodes.append(
                self.parse_scene_node_from_engine(scene_node=child_node)
            )
        return children_nodes

    def get_parent(self):
        parent_node = crescent_api_internal.node_get_parent(entity_id=self.entity_id)
        return self.parse_scene_node_from_engine(scene_node=parent_node)

    # Old API

    @classmethod
    def extract_valid_inheritance_node(cls) -> str:
        valid_node_type_list = [e.value for e in NodeType]
        for class_path in cls.__mro__:
            if class_path.__name__ in valid_node_type_list:
                return class_path.__name__
        return ""

    @classmethod
    def new(cls):
        return crescent_api_internal.node_new(
            class_path=f"{cls.__module__}",
            class_name=f"{cls.__name__}",
            node_type=f"{cls.extract_valid_inheritance_node()}",
        )

    @staticmethod
    def parse_scene_node_from_engine(scene_node):
        if not isinstance(scene_node, tuple):
            return scene_node
        elif scene_node:
            entity_id = scene_node[0]
            node_type = scene_node[1]
            node_class = globals()[node_type]
            instance = node_class(entity_id=entity_id)
            return instance
        # Is not a valid node, return 'None'
        return None

    def add_child(self, child_node) -> None:
        crescent_api_internal.node_add_child(
            parent_entity_id=self.entity_id, child_entity_id=child_node.entity_id
        )

    def get_node(self, name: str):
        node = crescent_api_internal.node_get_node(name=name)
        if not node:
            return None
        return self.parse_scene_node_from_engine(scene_node=node)

    def queue_deletion(self) -> None:
        crescent_api_internal.node_queue_deletion(entity_id=self.entity_id)

    def create_signal(self, signal_id: str) -> None:
        crescent_api_internal.node_signal_create(
            entity_id=self.entity_id, signal_id=signal_id
        )

    def connect_signal(self, signal_id: str, listener_node, function_name: str) -> None:
        crescent_api_internal.node_signal_connect(
            entity_id=self.entity_id,
            signal_id=signal_id,
            listener_entity_id=listener_node.entity_id,
            function_name=function_name,
        )

    def emit_signal(self, signal_id: str, args=[]) -> None:
        crescent_api_internal.node_signal_emit(
            entity_id=self.entity_id, signal_id=signal_id, args=args
        )

    @property
    def name(self) -> str:
        return crescent_api_internal.node_get_name(entity_id=self.entity_id)

    @property
    def tags(self) -> list:
        return crescent_api_internal.node_get_tags(entity_id=self.entity_id)

    @tags.setter
    def tags(self, value: list) -> None:
        crescent_api_internal.node_set_tags(entity_id=self.entity_id, tags=value)

    def show(self) -> None:
        crescent_api_internal.node_show(entity_id=self.entity_id)

    def hide(self) -> None:
        crescent_api_internal.node_hide(entity_id=self.entity_id)

    @property
    def visibility(self) -> bool:
        return crescent_api_internal.node_is_visible(entity_id=self.entity_id)

    @visibility.setter
    def visibility(self, value: bool) -> None:
        if value:
            self.show()
        else:
            self.hide()


# 2D
class Node2D(Node):
    def set_position(self, value: Vector2) -> None:
        crescent_api_internal.node2D_set_position(
            entity_id=self.entity_id, x=value.x, y=value.y
        )

    def add_to_position(self, value: Vector2) -> None:
        crescent_api_internal.node2D_add_to_position(
            entity_id=self.entity_id, x=value.x, y=value.y
        )

    def get_position(self) -> Vector2:
        px, py = crescent_api_internal.node2D_get_position(entity_id=self.entity_id)
        return Vector2(px, py)

    @property
    def position(self) -> Vector2:
        px, py = crescent_api_internal.node2D_get_position(entity_id=self.entity_id)
        return Vector2(px, py)

    @position.setter
    def position(self, value: Vector2) -> None:
        crescent_api_internal.node2D_set_position(
            entity_id=self.entity_id, x=value.x, y=value.y
        )

    def get_global_position(self) -> Vector2:
        px, py = crescent_api_internal.node2D_get_global_position(
            entity_id=self.entity_id
        )
        return Vector2(px, py)

    @property
    def global_position(self) -> Vector2:
        px, py = crescent_api_internal.node2D_get_global_position(
            entity_id=self.entity_id
        )
        return Vector2(px, py)

    def set_scale(self, value: Vector2) -> None:
        crescent_api_internal.node2D_set_scale(
            entity_id=self.entity_id, x=value.x, y=value.y
        )

    def add_to_scale(self, value: Vector2) -> None:
        crescent_api_internal.node2D_add_to_scale(
            entity_id=self.entity_id, x=value.x, y=value.y
        )

    def get_scale(self) -> Vector2:
        sx, sy = crescent_api_internal.node2D_get_scale(entity_id=self.entity_id)
        return Vector2(sx, sy)

    @property
    def scale(self) -> Vector2:
        sx, sy = crescent_api_internal.node2D_get_scale(entity_id=self.entity_id)
        return Vector2(sx, sy)

    @scale.setter
    def scale(self, value: Vector2) -> None:
        crescent_api_internal.node2D_set_scale(
            entity_id=self.entity_id, x=value.x, y=value.y
        )

    def set_rotation(self, value: float) -> None:
        crescent_api_internal.node2D_set_rotation(
            entity_id=self.entity_id, rotation=value
        )

    def add_to_rotation(self, value: float) -> None:
        crescent_api_internal.node2D_add_to_rotation(
            entity_id=self.entity_id, rotation=value
        )

    def get_rotation(self) -> float:
        return crescent_api_internal.node2D_get_rotation(entity_id=self.entity_id)

    @property
    def rotation(self) -> float:
        return crescent_api_internal.node2D_get_rotation(entity_id=self.entity_id)

    @rotation.setter
    def rotation(self, value: float) -> None:
        crescent_api_internal.node2D_set_rotation(
            entity_id=self.entity_id, rotation=value
        )

    @property
    def z_index(self) -> int:
        return crescent_api_internal.node2D_get_z_index(entity_id=self.entity_id)

    @z_index.setter
    def z_index(self, value: int) -> None:
        crescent_api_internal.node2D_set_z_index(
            entity_id=self.entity_id, z_index=value
        )


class Sprite(Node2D):
    @property
    def flip_h(self) -> bool:
        return crescent_api_internal.sprite_get_flip_h(entity_id=self.entity_id)

    @flip_h.setter
    def flip_h(self, value: bool) -> None:
        crescent_api_internal.sprite_set_flip_h(entity_id=self.entity_id, flip_h=value)

    @property
    def flip_v(self) -> bool:
        return crescent_api_internal.sprite_get_flip_v(entity_id=self.entity_id)

    @flip_v.setter
    def flip_v(self, value: bool) -> None:
        crescent_api_internal.sprite_set_flip_v(entity_id=self.entity_id, flip_h=value)

    @property
    def modulate(self) -> Color:
        red, green, blue, alpha = crescent_api_internal.sprite_get_modulate(
            entity_id=self.entity_id
        )
        return Color(r=red, g=green, b=blue, a=alpha)

    @modulate.setter
    def modulate(self, color: Color) -> None:
        crescent_api_internal.sprite_set_modulate(
            entity_id=self.entity_id,
            red=color.r,
            green=color.g,
            blue=color.b,
            alpha=color.a,
        )

    @property
    def texture(self) -> Texture:
        (
            file_path,
            wrap_s,
            wrap_t,
            filter_min,
            filter_mag,
        ) = crescent_api_internal.sprite_get_texture(entity_id=self.entity_id)
        return Texture(
            file_path=file_path,
            wrap_s=wrap_s,
            wrap_t=wrap_t,
            filter_min=filter_min,
            filter_mag=filter_mag,
        )

    @texture.setter
    def texture(self, value: Texture) -> None:
        crescent_api_internal.sprite_set_texture(
            entity_id=self.entity_id, file_path=value.file_path
        )

    @property
    def draw_source(self) -> Rect2:
        (
            x,
            y,
            w,
            h,
        ) = crescent_api_internal.sprite_get_draw_source(entity_id=self.entity_id)
        return Rect2(x=x, y=y, w=w, h=h)

    @draw_source.setter
    def draw_source(self, value: Rect2) -> None:
        crescent_api_internal.sprite_set_draw_source(
            entity_id=self.entity_id, x=value.x, y=value.y, w=value.w, h=value.h
        )


class AnimatedSprite(Node2D):
    pass


class TextLabel(Node2D):
    @property
    def text(self) -> str:
        return crescent_api_internal.text_label_get_text(entity_id=self.entity_id)

    @text.setter
    def text(self, value: str) -> None:
        crescent_api_internal.text_label_set_text(entity_id=self.entity_id, text=value)

    def get_text(self) -> str:
        return crescent_api_internal.text_label_get_text(entity_id=self.entity_id)

    def set_text(self, text: str) -> None:
        crescent_api_internal.text_label_set_text(entity_id=self.entity_id, text=text)

    @property
    def color(self) -> Color:
        r, g, b, a = crescent_api_internal.text_label_get_color(
            entity_id=self.entity_id
        )
        return Color(r, g, b, a)

    @color.setter
    def color(self, value: Color) -> None:
        crescent_api_internal.text_label_set_color(
            entity_id=self.entity_id, r=value.r, g=value.g, b=value.b, a=value.a
        )

    def get_color(self) -> Color:
        r, g, b, a = crescent_api_internal.text_label_get_color(
            entity_id=self.entity_id
        )
        return Color(r, g, b, a)

    def set_color(self, color: Color) -> None:
        crescent_api_internal.text_label_set_color(
            entity_id=self.entity_id, r=color.r, g=color.g, b=color.b, a=color.a
        )


class Collider2D(Node2D):
    def get_extents(self) -> Size2D:
        w, h = crescent_api_internal.collider2D_get_extents(entity_id=self.entity_id)
        return Size2D(w=w, h=h)

    def set_extents(self, extents: Size2D) -> None:
        crescent_api_internal.collider2D_set_extents(
            entity_id=self.entity_id, w=extents.w, h=extents.h
        )

    @property
    def extents(self) -> Size2D:
        w, h = crescent_api_internal.collider2D_get_extents(entity_id=self.entity_id)
        return Size2D(w=w, h=h)

    @extents.setter
    def extents(self, value: Size2D) -> None:
        crescent_api_internal.collider2D_set_extents(
            entity_id=self.entity_id, w=value.w, h=value.h
        )

    def get_color(self) -> Color:
        r, g, b, a = crescent_api_internal.collider2D_get_color(
            entity_id=self.entity_id
        )
        return Color(r=r, g=g, b=b, a=a)

    def set_color(self, color: Color) -> None:
        crescent_api_internal.collider2D_set_color(
            entity_id=self.entity_id, r=color.r, g=color.g, b=color.b, a=color.a
        )

    @property
    def color(self) -> Color:
        r, g, b, a = crescent_api_internal.collider2D_get_color(
            entity_id=self.entity_id
        )
        return Color(r=r, g=g, b=b, a=a)

    @color.setter
    def color(self, value: Color) -> None:
        crescent_api_internal.collider2D_set_color(
            entity_id=self.entity_id, r=value.r, g=value.g, b=value.b, a=value.a
        )


# SCENE TREE
class SceneTree:
    @staticmethod
    def change_scene(path: str) -> None:
        crescent_api_internal.scene_tree_change_scene(path=path)


# AUDIO MANAGER
class AudioManager:
    @staticmethod
    def play_sound(path: str, loops=False):
        crescent_api_internal.audio_manager_play_sound(path=path, loops=loops)

    @staticmethod
    def stop_sound(path: str):
        crescent_api_internal.audio_manager_stop_sound(path=path)


# PHYSICS
class CollisionHandler:
    # A temp (ish) class used to brute force collision checks for now

    @staticmethod
    def process_collisions(collider: Collider2D):
        collided_entities = crescent_api_internal.collision_handler_process_collisions(
            entity_id=collider.entity_id
        )
        for index, node in enumerate(collided_entities):
            yield Node.parse_scene_node_from_engine(scene_node=node)


# NETWORK
class Network:
    @staticmethod
    def is_server() -> bool:
        return crescent_api_internal.network_is_server()


class Server:
    @staticmethod
    def start(port: int) -> None:
        crescent_api_internal.server_start(port=port)

    @staticmethod
    def stop() -> None:
        crescent_api_internal.server_stop()

    @staticmethod
    def send(message: str) -> None:
        crescent_api_internal.server_send(message=message)

    @staticmethod
    def subscribe(signal_id: str, listener_node: Node, listener_func) -> None:
        crescent_api_internal.server_subscribe(
            signal_id=signal_id,
            listener_node=listener_node.entity_id,
            listener_func=listener_func,
        )


class Client:
    @staticmethod
    def start(host: str, port: int) -> None:
        crescent_api_internal.client_start(host=host, port=port)

    @staticmethod
    def stop() -> None:
        crescent_api_internal.client_stop()

    @staticmethod
    def send(message: str) -> None:
        crescent_api_internal.client_send(message=message)

    @staticmethod
    def subscribe(signal_id: str, listener_node: Node, listener_func) -> None:
        crescent_api_internal.client_subscribe(
            signal_id=signal_id,
            listener_node=listener_node.entity_id,
            listener_func=listener_func,
        )


# CONFIGURATION
def configure_game(
    game_tile="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=600,
    target_fps=66,
    initial_node_path="",
    colliders_visible=False,
) -> None:
    crescent_api_internal.configure_game(
        game_tile=game_tile,
        window_width=window_width,
        window_height=window_height,
        resolution_width=resolution_width,
        resolution_height=resolution_height,
        target_fps=target_fps,
        initial_node_path=initial_node_path,
        colliders_visible=colliders_visible,
    )


def configure_assets(audio_sources=None, textures=None, fonts=None) -> None:
    if fonts is None:
        fonts = []
    if textures is None:
        textures = []
    if audio_sources is None:
        audio_sources = []
    crescent_api_internal.configure_assets(
        audio_sources=audio_sources, textures=textures, fonts=fonts
    )


def configure_inputs(input_actions=None) -> None:
    if input_actions is None:
        input_actions = []
    crescent_api_internal.configure_inputs(input_actions=input_actions)


# STAGE
def create_stage_nodes(stage_nodes: list) -> None:
    crescent_api_internal.create_stage_nodes(stage_nodes=stage_nodes)
