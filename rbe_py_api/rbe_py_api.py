import rbe_py_api_internal

import math

# COLOR
class Color:
    def __init__(self, r=0.0, g=0.0, b=0.0, a=1.0):
        self.r = r
        self.g = g
        self.b = b
        self.a = a

    def __str__(self):
        return f"({self.r}, {self.g}, {self.b}, {self.a})"

    def __repr__(self):
        return f"({self.r}, {self.g}, {self.b}, {self.a})"

    @staticmethod
    def normalize(r: int, g: int, b: int, a=255):
        return Color(r / 255.0, g / 255.0, b / 255.0, a / 255.0)


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
        self, file_path: str, wrap_s: str, wrap_t: str, filter_min: str, filter_mag: str
    ):
        self.file_path = file_path
        self.wrap_s = wrap_s
        self.wrap_t = wrap_t
        self.filter_min = filter_min
        self.filter_mag = filter_mag


class Font:
    def __init__(self, file_path: str, uid: str, size: int):
        self.file_path = file_path
        self.uid = uid
        self.size = size


class InputAction:
    def __init__(self, name: str, values: list):
        self.name = name
        self.values = values


# INPUT
class Input:
    @staticmethod
    def add_action(action_name: str, value: str) -> None:
        rbe_py_api_internal.input_add_action(action_name=action_name, value=value)

    @staticmethod
    def is_action_pressed(action_name: str) -> bool:
        return rbe_py_api_internal.input_is_action_pressed(action_name=action_name)

    @staticmethod
    def is_action_just_pressed(action_name: str) -> bool:
        return rbe_py_api_internal.input_is_action_just_pressed(action_name=action_name)

    @staticmethod
    def is_action_just_released(action_name: str) -> bool:
        return rbe_py_api_internal.input_is_action_just_released(
            action_name=action_name
        )

    class Mouse:
        LEFT_BUTTON = "mb_left"
        RIGHT_BUTTON = "mb_right"

        @staticmethod
        def get_position() -> Vector2:
            mouse_x, mouse_y = rbe_py_api_internal.mouse_get_position()
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
        flip_x: bool,
        flip_y: bool,
        modulate: Color,
    ):
        self.texture_path = texture_path
        self.draw_source = draw_source
        self.flip_x = flip_x
        self.flip_y = flip_y
        self.modulate = modulate


class ScriptComponent:
    def __init__(self, class_path: str, class_name: str):
        self.class_path = class_path
        self.class_name = class_name


# CONFIGURATION
def configure_game(
    game_tile="Test Game",
    window_width=800,
    window_height=600,
    resolution_width=800,
    resolution_height=600,
    target_fps=66,
    initial_node_path="",
) -> None:
    rbe_py_api_internal.configure_game(
        game_tile=game_tile,
        window_width=window_width,
        window_height=window_height,
        resolution_width=resolution_width,
        resolution_height=resolution_height,
        target_fps=target_fps,
        initial_node_path=initial_node_path,
    )


def configure_assets(audio_sources=None, textures=None, fonts=None) -> None:
    if fonts is None:
        fonts = []
    if textures is None:
        textures = []
    if audio_sources is None:
        audio_sources = []
    rbe_py_api_internal.configure_assets(
        audio_sources=audio_sources, textures=textures, fonts=fonts
    )


def configure_inputs(input_actions=None) -> None:
    if input_actions is None:
        input_actions = []
    rbe_py_api_internal.configure_inputs(input_actions=input_actions)


# STAGE
def create_stage_nodes(stage_nodes: list) -> None:
    rbe_py_api_internal.create_stage_nodes(stage_nodes=stage_nodes)
