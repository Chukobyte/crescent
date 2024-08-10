import json
import math
from typing import List, Callable, Tuple, Optional, Dict, Union

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


class MinMaxVector2:
    def __init__(self, value_min: Vector2, value_max: Vector2):
        self.min = value_min
        self.max = value_max

    def contain(self, value: Vector2) -> bool:
        return self.min <= value <= self.max

    def is_below(self, value: Vector2) -> bool:
        return value < self.min

    def is_above(self, value: Vector2) -> bool:
        return value > self.max

    def __str__(self):
        return f"( {self.min}, {self.max} )"

    def __repr__(self):
        return f"( {self.min}, {self.max} )"


class AudioSource:
    def __init__(self, path: str):
        self.path = path

    @property
    def pitch(self) -> float:
        return crescent_internal.audio_source_get_pitch(self.path)

    @pitch.setter
    def pitch(self, value: float) -> None:
        crescent_internal.audio_source_set_pitch(self.path, value)


class AudioManager:
    @staticmethod
    def get_audio_source(path: str) -> AudioSource:
        return AudioSource(path)

    @staticmethod
    def play_sound(source: Union[AudioSource, str], loops=False):
        if isinstance(source, AudioSource):
            source = source.path
        crescent_internal.audio_manager_play_sound(source, loops)

    @staticmethod
    def stop_sound(source: Union[AudioSource, str]):
        if isinstance(source, AudioSource):
            source = source.path
        crescent_internal.audio_manager_stop_sound(source)


class Texture:
    def __init__(self, file_path: str, wrap_s="clamp_to_border", wrap_t="clamp_to_border", nearest_neighbor=True):
        self.file_path = file_path
        self.wrap_s = wrap_s
        self.wrap_t = wrap_t
        self.nearest_neighbor = nearest_neighbor

    def __str__(self):
        return f"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, nearest_neighbor: {self.nearest_neighbor})"

    def __repr__(self):
        return f"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, nearest_neighbor: {self.nearest_neighbor})"


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
    def __init__(self, name: str, speed: int, loops: bool, frames: List[AnimationFrame]):
        self.name = name
        self.speed = speed
        self.loops = loops
        self.frames = frames


class ShaderInstance:
    def __init__(self, shader_id: int):
        self.shader_id = shader_id
        self._is_active = True

    def delete(self) -> bool:
        if self._is_active:
            has_deleted = crescent_internal.shader_instance_delete(self.shader_id)
            if has_deleted:
                self._is_active = False
            return has_deleted
        return False

    def create_bool_param(self, name: str, initial_value: bool) -> None:
        crescent_internal.shader_instance_create_bool_param(self.shader_id, name, initial_value)

    def set_bool_param(self, name: str, value: bool) -> None:
        crescent_internal.shader_instance_set_bool_param(self.shader_id, name, value)

    def get_bool_param(self, name: str) -> bool:
        return crescent_internal.shader_instance_get_bool_param(self.shader_id, name)

    def create_int_param(self, name: str, initial_value: int) -> None:
        crescent_internal.shader_instance_create_int_param(self.shader_id, name, initial_value)

    def set_int_param(self, name: str, value: int) -> None:
        crescent_internal.shader_instance_set_int_param(self.shader_id, name, value)

    def get_int_param(self, name: str) -> int:
        return crescent_internal.shader_instance_get_int_param(self.shader_id, name)

    def create_float_param(self, name: str, initial_value: float) -> None:
        crescent_internal.shader_instance_create_float_param(self.shader_id, name, initial_value)

    def set_float_param(self, name: str, value: float) -> None:
        crescent_internal.shader_instance_set_float_param(self.shader_id, name, value)

    def get_float_param(self, name: str) -> float:
        return crescent_internal.shader_instance_get_float_param(self.shader_id, name)

    def create_float2_param(self, name: str, initial_value: Vector2) -> None:
        crescent_internal.shader_instance_create_float2_param(self.shader_id, name, initial_value.x, initial_value.y)

    def set_float2_param(self, name: str, value: Vector2) -> None:
        crescent_internal.shader_instance_set_float2_param(self.shader_id, name, value.x, value.y)

    def get_float2_param(self, name: str) -> Vector2:
        x, y = crescent_internal.shader_instance_get_float2_param(self.shader_id, name)
        return Vector2(x, y)

    def create_float3_param(self, name: str, initial_value: Vector3) -> None:
        crescent_internal.shader_instance_create_float3_param(self.shader_id, name, initial_value.x, initial_value.y, initial_value.z)

    def set_float3_param(self, name: str, value: Vector3) -> None:
        crescent_internal.shader_instance_set_float3_param(self.shader_id, name, value.x, value.y, value.z)

    def get_float3_param(self, name: str) -> Vector3:
        x, y, z = crescent_internal.shader_instance_get_float3_param(self.shader_id, name)
        return Vector3(x, y, z)

    def create_float4_param(self, name: str, initial_value: Vector4) -> None:
        crescent_internal.shader_instance_create_float4_param(self.shader_id, name, initial_value.x, initial_value.y, initial_value.z, initial_value.w)

    def set_float4_param(self, name: str, value: Vector4) -> None:
        crescent_internal.shader_instance_set_float4_param(self.shader_id, name, value.x, value.y, value.z, value.w)

    def get_float4_param(self, name: str) -> Vector4:
        x, y, z, w = crescent_internal.shader_instance_get_float4_param(self.shader_id, name)
        return Vector4(x, y, z, w)


class ShaderUtil:
    @staticmethod
    def compile_shader(shader_path: str) -> ShaderInstance:
        shader_id = crescent_internal.shader_util_compile_shader(shader_path)
        return ShaderInstance(shader_id)

    @staticmethod
    def compile_shader_raw(vertex_path: str, fragment_path: str) -> ShaderInstance:
        shader_id = crescent_internal.shader_util_compile_shader_raw(vertex_path, fragment_path)
        return ShaderInstance(shader_id)

    @staticmethod
    def set_screen_shader(shader_instance: ShaderInstance) -> bool:
        return crescent_internal.shader_util_set_screen_shader(shader_instance.shader_id)

    @staticmethod
    def get_current_screen_shader() -> Optional[ShaderInstance]:
        shader_instance_id = (crescent_internal.shader_util_get_current_screen_shader())
        if shader_instance_id >= 0:
            return ShaderInstance(shader_instance_id)
        return None

    @staticmethod
    def reset_screen_shader_to_default() -> None:
        crescent_internal.shader_util_reset_screen_shader_to_default()


class Engine:
    @staticmethod
    def exit(code=0) -> None:
        crescent_internal.engine_exit(code)

    @staticmethod
    def set_target_fps(fps: int) -> None:
        crescent_internal.engine_set_target_fps(fps)

    @staticmethod
    def get_target_fps() -> int:
        return crescent_internal.engine_get_target_fps()

    @staticmethod
    def get_average_fps() -> float:
        return crescent_internal.engine_get_average_fps()

    @staticmethod
    def set_fps_display_enabled(enabled: bool, font_uid="", position: Optional[Vector2] = None) -> None:
        if not position:
            position = Vector2(20, 30)
        crescent_internal.engine_set_fps_display_enabled(enabled, font_uid, position.x, position.y)

    @staticmethod
    def get_global_physics_delta_time() -> float:
        return crescent_internal.engine_get_global_physics_delta_time()


class Input:
    @staticmethod
    def is_key_pressed(key: int) -> bool:
        return crescent_internal.input_is_key_pressed(key)

    @staticmethod
    def is_key_just_pressed(key: int) -> bool:
        return crescent_internal.input_is_key_just_pressed(key)

    @staticmethod
    def is_key_just_released(key: int) -> bool:
        return crescent_internal.input_is_key_just_released(key)

    @staticmethod
    def add_action(name: str, value_key: int, device_id=0) -> None:
        crescent_internal.input_add_action(name, value_key, device_id)

    @staticmethod
    def is_action_pressed(name: str) -> bool:
        return crescent_internal.input_is_action_pressed(name)

    @staticmethod
    def is_action_just_pressed(name: str) -> bool:
        return crescent_internal.input_is_action_just_pressed(name)

    @staticmethod
    def is_action_just_released(name: str) -> bool:
        return crescent_internal.input_is_action_just_released(name)

    @staticmethod
    def start_gamepad_vibration(device_id: int, weak_magnitude: float, strong_magnitude: float, duration: float) -> None:
        crescent_internal.input_start_gamepad_vibration(device_id, weak_magnitude, strong_magnitude, duration)

    @staticmethod
    def stop_gamepad_vibration(device_id: int) -> None:
        crescent_internal.input_stop_gamepad_vibration(device_id)


class Mouse:
    LEFT_BUTTON = 148
    RIGHT_BUTTON = 149
    MIDDLE_BUTTON = 150

    @staticmethod
    def get_position() -> Vector2:
        mouse_x, mouse_y = crescent_internal.input_mouse_get_position()
        return Vector2(mouse_x, mouse_y)

    @staticmethod
    def get_world_position() -> Vector2:
        mouse_x, mouse_y = crescent_internal.input_mouse_get_world_position()
        return Vector2(mouse_x, mouse_y)


class Keyboard:
    TAB = 29

    A = 96
    B = 97
    C = 98
    D = 99
    E = 100
    F = 101
    G = 102
    H = 103
    I = 104
    J = 105
    K = 106
    L = 107
    M = 108
    N = 109
    O = 110
    P = 111
    Q = 112
    R = 113
    S = 114
    T = 115
    U = 116
    V = 117
    W = 118
    X = 119
    Y = 120
    Z = 121

    LEFT = 30
    RIGHT = 31
    UP = 32
    DOWN = 33
    PAGE_DOWN = 34
    PAGE_UP = 35
    HOME = 36
    END = 37
    INSERT = 38
    DELETE = 39
    BACKSPACE = 40
    SPACE = 41
    RETURN = 42
    ESC = 43
    QUOTE = 44
    COMMA = 45
    MINUS = 46
    PERIOD = 47
    SLASH = 48
    SEMICOLON = 49
    EQUALS = 50
    LEFT_BRACKET = 51
    RIGHT_BRACKET = 52
    BACKSLASH = 53
    BACKQUOTE = 54
    CAPS_LOCK = 55
    SCROLL_LOCK = 56
    NUM_LOCK_CLEAR = 57
    PRINT_SCREEN = 58
    PAUSE = 59
    KEYPAD_0 = 60
    KEYPAD_1 = 61
    KEYPAD_2 = 62
    KEYPAD_3 = 63
    KEYPAD_4 = 64
    KEYPAD_5 = 65
    KEYPAD_6 = 66
    KEYPAD_7 = 67
    KEYPAD_8 = 68
    KEYPAD_9 = 69
    KEYPAD_PERIOD = 70
    KEYPAD_DIVIDE = 71
    KEYPAD_MULTIPLY = 72
    KEYPAD_MINUS = 73
    KEYPAD_PLUS = 74
    KEYPAD_ENTER = 75
    KEYPAD_EQUALS = 76
    LEFT_CONTROL = 77
    LEFT_SHIFT = 78
    LEFT_ALT = 79
    LEFT_GUI = 80
    RIGHT_CONTROL = 81
    RIGHT_SHIFT = 82
    RIGHT_ALT = 83
    RIGHT_GUI = 84
    APPLICATION = 85
    NUM_0 = 86
    NUM_1 = 87
    NUM_2 = 88
    NUM_3 = 89
    NUM_4 = 90
    NUM_5 = 91
    NUM_6 = 92
    NUM_7 = 93
    NUM_8 = 95
    NUM_9 = 96

    F1 = 122
    F2 = 123
    F3 = 124
    F4 = 125
    F5 = 126
    F6 = 127
    F7 = 128
    F8 = 129
    F9 = 130
    F10 = 131
    F11 = 132
    F12 = 133
    F13 = 134
    F14 = 135
    F15 = 136
    F16 = 137
    F17 = 138
    F18 = 139
    F19 = 140
    F20 = 141
    F21 = 142
    F22 = 143
    F23 = 144
    F24 = 145
    APP_FORWARD = 146
    APP_BACK = 147


class Gamepad:
    BUTTON_DPAD_DOWN = 1
    BUTTON_DPAD_UP = 2
    BUTTON_DPAD_LEFT = 3
    BUTTON_DPAD_RIGHT = 4
    BUTTON_NORTH = 5
    BUTTON_SOUTH = 6
    BUTTON_EAST = 7
    BUTTON_WEST = 8
    BUTTON_START = 9
    BUTTON_BACK = 10
    BUTTON_LEFT_SHOULDER = 11
    AXIS_LEFT_TRIGGER = 12
    BUTTON_LEFT_ANALOG = 13
    LEFT_ANALOG_2D_AXIS_X = 14
    LEFT_ANALOG_2D_AXIS_Y = 15
    BUTTON_RIGHT_SHOULDER = 16
    AXIS_RIGHT_TRIGGER = 17
    BUTTON_RIGHT_ANALOG = 18
    RIGHT_ANALOG_2D_AXIS_X = 19
    RIGHT_ANALOG_2D_AXIS_Y = 20
    AXIS_LEFT_ANALOG_LEFT = 21
    AXIS_LEFT_ANALOG_RIGHT = 22
    AXIS_LEFT_ANALOG_UP = 23
    AXIS_LEFT_ANALOG_DOWN = 24
    AXIS_RIGHT_ANALOG_LEFT = 25
    AXIS_RIGHT_ANALOG_RIGHT = 26
    AXIS_RIGHT_ANALOG_UP = 27
    AXIS_RIGHT_ANALOG_DOWN = 28


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
        subscriber = _NodeEventSubscriber(entity_id, call_back, self.entity_id, self.name)
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
    Node2D = 2
    Sprite = 4
    AnimatedSprite = 8
    TextLabel = 16
    Collider2D = 32
    ColorRect = 64
    Parallax = 128
    Particles2D = 256
    Tilemap = 512


class Node:
    def __init__(self, entity_id: int) -> None:
        self.entity_id = entity_id

    @classmethod
    def new(cls) -> "Node":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.Node)

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
    @classmethod
    def new(cls) -> "Node2D":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.Node2D)

    def set_position(self, value: Vector2) -> None:
        crescent_internal.node2d_set_position(self.entity_id, value.x, value.y)

    def add_to_position(self, value: Vector2) -> None:
        crescent_internal.node2d_add_to_position(self.entity_id, value.x, value.y)

    def get_position(self) -> Vector2:
        px, py = crescent_internal.node2d_get_position(self.entity_id)
        return Vector2(px, py)

    @property
    def position(self) -> Vector2:
        px, py = crescent_internal.node2d_get_position(self.entity_id)
        return Vector2(px, py)

    @position.setter
    def position(self, value: Vector2) -> None:
        crescent_internal.node2d_set_position(self.entity_id, value.x, value.y)

    def get_global_position(self) -> Vector2:
        px, py = crescent_internal.node2d_get_global_position(self.entity_id)
        return Vector2(px, py)

    @property
    def global_position(self) -> Vector2:
        px, py = crescent_internal.node2d_get_global_position(self.entity_id)
        return Vector2(px, py)

    def set_scale(self, value: Vector2) -> None:
        crescent_internal.node2d_set_scale(self.entity_id, value.x, value.y)

    def add_to_scale(self, value: Vector2) -> None:
        crescent_internal.node2d_add_to_scale(self.entity_id, value.x, value.y)

    def get_scale(self) -> Vector2:
        sx, sy = crescent_internal.node2d_get_scale(self.entity_id)
        return Vector2(sx, sy)

    @property
    def scale(self) -> Vector2:
        sx, sy = crescent_internal.node2d_get_scale(self.entity_id)
        return Vector2(sx, sy)

    @scale.setter
    def scale(self, value: Vector2) -> None:
        crescent_internal.node2d_set_scale(self.entity_id, value.x, value.y)

    def set_rotation(self, value: float) -> None:
        crescent_internal.node2d_set_rotation(self.entity_id, value)

    def add_to_rotation(self, value: float) -> None:
        crescent_internal.node2d_add_to_rotation(self.entity_id, value)

    def get_rotation(self) -> float:
        return crescent_internal.node2d_get_rotation(self.entity_id)

    @property
    def rotation(self) -> float:
        return crescent_internal.node2d_get_rotation(self.entity_id)

    @rotation.setter
    def rotation(self, value: float) -> None:
        crescent_internal.node2d_set_rotation(self.entity_id, value)

    @property
    def z_index(self) -> int:
        return crescent_internal.node2d_get_z_index(self.entity_id)

    @z_index.setter
    def z_index(self, value: int) -> None:
        crescent_internal.node2d_set_z_index(self.entity_id, value)

    @property
    def z_index_relative_to_parent(self) -> bool:
        return crescent_internal.node2d_get_z_index_relative_to_parent(self.entity_id)

    @z_index_relative_to_parent.setter
    def z_index_relative_to_parent(self, value: bool) -> None:
        crescent_internal.node2d_set_z_index_relative_to_parent(self.entity_id, value)

    @property
    def ignore_camera(self) -> bool:
        return crescent_internal.node2d_get_ignore_camera(self.entity_id)

    @ignore_camera.setter
    def ignore_camera(self, value: bool) -> None:
        crescent_internal.node2d_set_ignore_camera(self.entity_id, value)


class Sprite(Node2D):
    @classmethod
    def new(cls) -> "Sprite":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.Sprite)

    @property
    def texture(self) -> Texture:
        file_path, wrap_s, wrap_t, nearest_neighbor = crescent_internal.sprite_get_texture(self.entity_id)
        return Texture(file_path, wrap_s, wrap_t, nearest_neighbor)

    @texture.setter
    def texture(self, value: Texture) -> None:
        crescent_internal.sprite_set_texture(self.entity_id, value.file_path)

    @property
    def draw_source(self) -> Rect2:
        x, y, w, h = crescent_internal.sprite_get_draw_source(self.entity_id)
        return Rect2(x=x, y=y, w=w, h=h)

    @draw_source.setter
    def draw_source(self, value: Rect2) -> None:
        crescent_internal.sprite_set_draw_source(self.entity_id, value.x, value.y, value.w, value.h)

    @property
    def flip_h(self) -> bool:
        return crescent_internal.sprite_get_flip_h(self.entity_id)

    @flip_h.setter
    def flip_h(self, value: bool) -> None:
        crescent_internal.sprite_set_flip_h(self.entity_id, value)

    @property
    def flip_v(self) -> bool:
        return crescent_internal.sprite_get_flip_v(self.entity_id)

    @flip_v.setter
    def flip_v(self, value: bool) -> None:
        crescent_internal.sprite_set_flip_v(self.entity_id, value)

    @property
    def modulate(self) -> Color:
        red, green, blue, alpha = crescent_internal.sprite_get_modulate(self.entity_id)
        return Color(red, green, blue, alpha)

    @modulate.setter
    def modulate(self, color: Color) -> None:
        crescent_internal.sprite_set_modulate(self.entity_id, color.r, color.g, color.b, color.a)

    @property
    def origin(self) -> Vector2:
        x, y = crescent_internal.sprite_get_origin(self.entity_id)
        return Vector2(x, y)

    @origin.setter
    def origin(self, value: Vector2) -> None:
        crescent_internal.sprite_set_origin(self.entity_id, value.x, value.y)

    @property
    def shader_instance(self) -> Optional[ShaderInstance]:
        shader_instance_id = crescent_internal.sprite_get_shader_instance(self.entity_id)
        if shader_instance_id >= 0:
            return ShaderInstance(shader_instance_id)
        return None

    @shader_instance.setter
    def shader_instance(self, value: ShaderInstance) -> None:
        crescent_internal.sprite_set_shader_instance(self.entity_id, value.shader_id)


class AnimatedSprite(Node2D):
    @classmethod
    def new(cls) -> "AnimatedSprite":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.AnimatedSprite)

    def play(self, name: str) -> bool:
        return crescent_internal.animated_sprite_play(self.entity_id, name)

    def stop(self) -> None:
        crescent_internal.animated_sprite_stop(self.entity_id)

    def set_current_animation_frame(self, frame: int) -> None:
        crescent_internal.animated_sprite_set_current_animation_frame(self.entity_id, frame)

    def add_animation(self, animation: Animation) -> None:
        crescent_internal.animated_sprite_add_animation(self.entity_id, animation.name, animation.speed, animation.loops, len(animation.frames), *animation.frames)

    @property
    def flip_h(self) -> bool:
        return crescent_internal.animated_sprite_get_flip_h(self.entity_id)

    @flip_h.setter
    def flip_h(self, value: bool) -> None:
        crescent_internal.animated_sprite_set_flip_h(self.entity_id, value)

    @property
    def flip_v(self) -> bool:
        return crescent_internal.animated_sprite_get_flip_v(self.entity_id)

    @flip_v.setter
    def flip_v(self, value: bool) -> None:
        crescent_internal.animated_sprite_set_flip_v(self.entity_id, value)

    @property
    def modulate(self) -> Color:
        red, green, blue, alpha = crescent_internal.animated_sprite_get_modulate(self.entity_id)
        return Color(r=red, g=green, b=blue, a=alpha)

    @modulate.setter
    def modulate(self, color: Color) -> None:
        crescent_internal.animated_sprite_set_modulate(self.entity_id, color.r, color.g, color.b, color.a)

    @property
    def origin(self) -> Vector2:
        x, y = crescent_internal.animated_sprite_get_origin(self.entity_id)
        return Vector2(x, y)

    @origin.setter
    def origin(self, value: Vector2) -> None:
        crescent_internal.animated_sprite_set_origin(self.entity_id, value.x, value.y)

    @property
    def stagger_animation_start_times(self) -> bool:
        return crescent_internal.animated_sprite_get_stagger_animation_start_times(self.entity_id)

    @stagger_animation_start_times.setter
    def stagger_animation_start_times(self, value: bool) -> None:
        crescent_internal.animated_sprite_set_stagger_animation_start_times(self.entity_id, value)

    @property
    def shader_instance(self) -> Optional[ShaderInstance]:
        shader_instance_id = crescent_internal.animated_sprite_get_shader_instance(self.entity_id)
        if shader_instance_id >= 0:
            return ShaderInstance(shader_instance_id)
        return None

    @shader_instance.setter
    def shader_instance(self, value: ShaderInstance) -> None:
        crescent_internal.animated_sprite_set_shader_instance(self.entity_id, value.shader_id)


class TextLabel(Node2D):
    @classmethod
    def new(cls) -> "TextLabel":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.TextLabel)

    @property
    def text(self) -> str:
        return crescent_internal.text_label_get_text(self.entity_id)

    @text.setter
    def text(self, value: str) -> None:
        crescent_internal.text_label_set_text(self.entity_id, value)

    def get_text(self) -> str:
        return crescent_internal.text_label_get_text(self.entity_id)

    def set_text(self, text: str) -> None:
        crescent_internal.text_label_set_text(self.entity_id, text)

    @property
    def color(self) -> Color:
        r, g, b, a = crescent_internal.text_label_get_color(self.entity_id)
        return Color(r, g, b, a)

    @color.setter
    def color(self, value: Color) -> None:
        crescent_internal.text_label_set_color(self.entity_id, value.r, value.g, value.b, value.a)

    def get_color(self) -> Color:
        r, g, b, a = crescent_internal.text_label_get_color(self.entity_id)
        return Color(r, g, b, a)

    def set_color(self, color: Color) -> None:
        crescent_internal.text_label_set_color(self.entity_id, color.r, color.g, color.b, color.a)

    def set_font(self, uid: str) -> None:
        crescent_internal.text_label_set_font_uid(self.entity_id, uid)


class Collider2D(Node2D):
    @classmethod
    def new(cls) -> "Collider2D":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.Collider2D)

    def get_extents(self) -> Size2D:
        w, h = crescent_internal.collider2d_get_extents(self.entity_id)
        return Size2D(w, h)

    def set_extents(self, extents: Size2D) -> None:
        crescent_internal.collider2d_set_extents(self.entity_id, extents.w, extents.h)

    @property
    def extents(self) -> Size2D:
        w, h = crescent_internal.collider2d_get_extents(self.entity_id)
        return Size2D(w, h)

    @extents.setter
    def extents(self, value: Size2D) -> None:
        crescent_internal.collider2d_set_extents(self.entity_id, value.w, value.h)

    def get_color(self) -> Color:
        r, g, b, a = crescent_internal.collider2d_get_color(self.entity_id)
        return Color(r, g, b, a)

    def set_color(self, color: Color) -> None:
        crescent_internal.collider2d_set_color(self.entity_id, color.r, color.g, color.b, color.a)

    @property
    def color(self) -> Color:
        r, g, b, a = crescent_internal.collider2d_get_color(self.entity_id)
        return Color(r, g, b, a)

    @color.setter
    def color(self, value: Color) -> None:
        crescent_internal.collider2d_set_color(self.entity_id, value.r, value.g, value.b, value.a)


class ColorRect(Node2D):
    @classmethod
    def new(cls) -> "ColorRect":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.ColorRect)

    def get_size(self) -> Size2D:
        w, h = crescent_internal.color_rect_get_size(self.entity_id)
        return Size2D(w, h)

    def set_size(self, extents: Size2D) -> None:
        crescent_internal.color_rect_set_size(self.entity_id, extents.w, extents.h)

    @property
    def size(self) -> Size2D:
        w, h = crescent_internal.color_rect_get_size(self.entity_id)
        return Size2D(w, h)

    @size.setter
    def size(self, value: Size2D) -> None:
        crescent_internal.color_rect_set_size(self.entity_id, value.w, value.h)

    def get_color(self) -> Color:
        r, g, b, a = crescent_internal.color_rect_get_color(self.entity_id)
        return Color(r, g, b, a)

    def set_color(self, color: Color) -> None:
        crescent_internal.color_rect_set_color(self.entity_id, color.r, color.g, color.b, color.a)

    @property
    def color(self) -> Color:
        r, g, b, a = crescent_internal.color_rect_get_color(self.entity_id)
        return Color(r, g, b, a)

    @color.setter
    def color(self, value: Color) -> None:
        crescent_internal.color_rect_set_color(self.entity_id, value.r, value.g, value.b, value.a)


class Parallax(Node2D):
    @classmethod
    def new(cls) -> "Parallax":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.Parallax)

    @property
    def scroll_speed(self) -> Vector2:
        x, y = crescent_internal.parallax_get_scroll_speed(self.entity_id)
        return Vector2(x, y)

    @scroll_speed.setter
    def scroll_speed(self, value: Vector2) -> None:
        crescent_internal.parallax_set_scroll_speed(self.entity_id, value.x, value.y)


class Particles2D(Node2D):
    @classmethod
    def new(cls) -> "Particles2D":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.Particles2D)

    @property
    def amount(self) -> int:
        return crescent_internal.particles2d_get_amount(self.entity_id)

    @amount.setter
    def amount(self, value: int) -> None:
        crescent_internal.particles2d_set_amount(self.entity_id, value)

    @property
    def life_time(self) -> float:
        return crescent_internal.particles2d_get_life_time(self.entity_id)

    @life_time.setter
    def life_time(self, value: float) -> None:
        crescent_internal.particles2d_set_life_time(self.entity_id, value)

    @property
    def damping(self) -> float:
        return crescent_internal.particles2d_get_damping(self.entity_id)

    @damping.setter
    def damping(self, value: float) -> None:
        crescent_internal.particles2d_set_damping(self.entity_id, value)

    @property
    def explosiveness(self) -> float:
        return crescent_internal.particles2d_get_explosiveness(self.entity_id)

    @explosiveness.setter
    def explosiveness(self, value: float) -> None:
        crescent_internal.particles2d_set_explosiveness(self.entity_id, value)

    @property
    def color(self) -> Color:
        r, g, b, a = crescent_internal.particles2d_get_color(self.entity_id)
        return Color(r, g, b, a)

    @color.setter
    def color(self, value: Color) -> None:
        crescent_internal.particles2d_set_color(self.entity_id, value.r, value.g, value.b, value.a)

    @property
    def initial_velocity(self) -> MinMaxVector2:
        min_x, min_y, max_x, max_y = crescent_internal.particles2d_get_initial_velocity(self.entity_id)
        return MinMaxVector2(
            Vector2(min_x, min_y),
            Vector2(max_x, max_y)
        )

    @initial_velocity.setter
    def initial_velocity(self, value: MinMaxVector2) -> None:
        crescent_internal.particles2d_set_initial_velocity(self.entity_id, value.min.x, value.min.y, value.max.x, value.max.y)

    @property
    def spread(self) -> float:
        return crescent_internal.particles2d_get_spread(self.entity_id)

    @spread.setter
    def spread(self, value: float) -> None:
        crescent_internal.particles2d_set_spread(self.entity_id, value)


class Tilemap(Node2D):
    @classmethod
    def new(cls) -> "Tilemap":
        return crescent_internal.node_new(str(cls.__module__.__name__), cls.__name__, NodeType.Tilemap)


class SceneTree:
    @staticmethod
    def change_scene(path: str) -> None:
        crescent_internal.scene_tree_change_scene(path)

    @staticmethod
    def get_root() -> Optional[Node]:
        return crescent_internal.scene_tree_get_root()


class World:
    @staticmethod
    def set_time_dilation(time_dilation: float) -> None:
        crescent_internal.world_set_time_dilation(time_dilation)

    @staticmethod
    def get_time_dilation() -> float:
        return crescent_internal.world_get_time_dilation()

    @staticmethod
    def get_delta_time() -> float:
        return crescent_internal.world_get_delta_time()


class CollisionHandler:
    """
    Will most likely be replaced once a physics system is in place
    """
    @staticmethod
    def process_collisions(collider: Collider2D) -> List[Node]:
        collided_entities = crescent_internal.collision_handler_process_collisions(collider.entity_id)
        if collided_entities:
            if isinstance(collided_entities, tuple):
                return list(collided_entities)
            elif isinstance(collided_entities, Node):
                return [collided_entities]
        return []

    @staticmethod
    def process_mouse_collisions(pos_offset: Optional[Vector2] = None, collision_size: Optional[Size2D] = None) -> List[Node]:
        if not pos_offset:
            pos_offset = Vector2.ZERO()
        if not collision_size:
            collision_size = Size2D(1.0, 1.0)
        collided_entities = crescent_internal.collision_handler_process_mouse_collisions(pos_offset.x, pos_offset.y, collision_size.w, collision_size.h)
        if collided_entities:
            if isinstance(collided_entities, tuple):
                return list(collided_entities)
            elif isinstance(collided_entities, Node):
                return [collided_entities]
        return []


# Work around for singleton until class methods are in
class _InternalGameProperties:
    def __init__(self, game_title: str, game_resolution: Size2D, default_window_size: Size2D, target_fps: int, initial_scene_path: str, are_colliders_visible: bool):
        self.game_title = game_title
        self.game_resolution = game_resolution
        self.default_window_size = default_window_size
        self.target_fps = target_fps
        self.initial_scene_path = initial_scene_path
        self.are_colliders_visible = are_colliders_visible


_game_properties_instance = None


class GameProperties:
    def __init__(self) -> None:
        global _game_properties_instance
        if not _game_properties_instance:
            game_title, res_w, res_h, window_w, window_h, target_fps, initial_scene_path, are_colliders_visible = crescent_internal.game_properties_get()
            _game_properties_instance = _InternalGameProperties(game_title, Size2D(res_w, res_h), Size2D(window_w, window_h), target_fps, initial_scene_path, are_colliders_visible)

    def __str__(self):
        return f"GameProperties(game_title: {self.game_title}, game_resolution: {self.game_resolution}), default_window_size: {self.default_window_size}, target_fps: {self.target_fps}, initial_scene_path: {self.initial_scene_path}, are_colliders_visible: {self.are_colliders_visible})"

    def __repr__(self):
        return f"GameProperties(game_title: {self.game_title}, game_resolution: {self.game_resolution}), default_window_size: {self.default_window_size}, target_fps: {self.target_fps}, initial_scene_path: {self.initial_scene_path}, are_colliders_visible: {self.are_colliders_visible})"

    @property
    def game_title(self) -> str:
        return _game_properties_instance.game_title

    @property
    def game_resolution(self) -> Size2D:
        return _game_properties_instance.game_resolution

    @property
    def default_window_size(self) -> Size2D:
        return _game_properties_instance.default_window_size

    @property
    def target_fps(self) -> int:
        return _game_properties_instance.target_fps

    @property
    def initial_scene_path(self) -> str:
        return _game_properties_instance.initial_scene_path

    @property
    def are_colliders_visible(self) -> bool:
        return _game_properties_instance.are_colliders_visible


class Camera2D:
    @staticmethod
    def set_position(position: Vector2) -> None:
        crescent_internal.camera2d_set_position(position.x, position.y)

    @staticmethod
    def add_to_position(position: Vector2) -> None:
        crescent_internal.camera2d_add_to_position(position.x, position.y)

    @staticmethod
    def get_position() -> Vector2:
        x, y = crescent_internal.camera2d_get_position()
        return Vector2(x, y)

    @staticmethod
    def set_offset(offset: Vector2) -> None:
        crescent_internal.camera2d_set_offset(offset.x, offset.y)

    @staticmethod
    def add_to_offset(offset: Vector2) -> None:
        crescent_internal.camera2d_add_to_offset(offset.x, offset.y)

    @staticmethod
    def get_offset() -> Vector2:
        x, y = crescent_internal.camera2d_get_offset()
        return Vector2(x, y)

    @staticmethod
    def set_zoom(zoom: Vector2) -> None:
        crescent_internal.camera2d_set_zoom(zoom.x, zoom.y)

    @staticmethod
    def add_to_zoom(zoom: Vector2) -> None:
        crescent_internal.camera2d_add_to_zoom(zoom.x, zoom.y)

    @staticmethod
    def get_zoom() -> Vector2:
        x, y = crescent_internal.camera2d_get_zoom()
        return Vector2(x, y)

    @staticmethod
    def set_boundary(boundary: Rect2) -> None:
        crescent_internal.camera2d_set_boundary(
            boundary.x, boundary.y, boundary.w, boundary.h
        )

    @staticmethod
    def get_boundary() -> Rect2:
        x, y, w, h = crescent_internal.camera2d_get_boundary()
        return Rect2(x, y, w, h)

    @staticmethod
    def follow_node(node: Node2D) -> None:
        crescent_internal.camera2d_follow_node(node.entity_id)

    @staticmethod
    def unfollow_node(node: Node2D) -> None:
        crescent_internal.camera2d_unfollow_node(node.entity_id)


class GameConfig:
    @staticmethod
    def save(path: str, data: dict, encryption_key="") -> bool:
        try:
            json_text = json.dumps(data, indent=4)
        except Exception:
            print(f"Game Config Save Json Decode Error")
            return False
        return crescent_internal.game_config_save(path, json_text, encryption_key)

    @staticmethod
    def load(path: str, encryption_key="") -> dict:
        try:
            json_dict = json.loads(
                crescent_internal.game_config_load(path, encryption_key)
            )
        except Exception:
            print(f"Game Config Load Json Decode Error")
            return {}
        return json_dict


class PackedScene:
    def __init__(self, scene_cache_id: int, path: str):
        self.scene_cache_id = scene_cache_id
        self.path = path

    def create_instance(self) -> Node:
        return crescent_internal.packed_scene_create_instance(self.scene_cache_id)

    @staticmethod
    def load(path: str) -> Optional["PackedScene"]:
        scene_cache_id = crescent_internal.packed_scene_load(path)
        if scene_cache_id < 0:
            return None
        return PackedScene(scene_cache_id, path)


class Network:
    @staticmethod
    def is_server() -> bool:
        return crescent_internal.network_is_server()


class Server:
    @staticmethod
    def start(port: int) -> None:
        crescent_internal.server_start(port=port)

    @staticmethod
    def stop() -> None:
        crescent_internal.server_stop()

    @staticmethod
    def send(message: str) -> None:
        crescent_internal.server_send(message=message)

    @staticmethod
    def subscribe(event_name: str, listener_node: Node, listener_func: Callable) -> None:
        _node_event_manager.subscribe_to_event(-100, event_name, listener_node.entity_id, listener_func)


class Client:
    @staticmethod
    def start(host: str, port: int) -> None:
        crescent_internal.client_start(host=host, port=port)

    @staticmethod
    def stop() -> None:
        crescent_internal.client_stop()

    @staticmethod
    def send(message: str) -> None:
        crescent_internal.client_send(message=message)

    @staticmethod
    def subscribe(event_name: str, listener_node: Node, listener_func: Callable) -> None:
        _node_event_manager.subscribe_to_event(-100, event_name, listener_node.entity_id, listener_func)
