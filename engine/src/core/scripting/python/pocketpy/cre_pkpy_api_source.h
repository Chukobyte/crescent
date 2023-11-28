#pragma once

#define CRE_PKPY_CRESCENT_SOURCE ""\
"import math\n"\
"from typing import List, Callable, Tuple, Optional, Dict\n"\
"\n"\
"import crescent_internal\n"\
"\n"\
"\n"\
"# COLOR\n"\
"class Color:\n"\
"    def __init__(self, r=0, g=0, b=0, a=255):\n"\
"        self.r = r\n"\
"        self.g = g\n"\
"        self.b = b\n"\
"        self.a = a\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"({self.r}, {self.g}, {self.b}, {self.a})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"({self.r}, {self.g}, {self.b}, {self.a})\"\n"\
"\n"\
"    def copy(self) -> \"Color\":\n"\
"        return Color(self.r, self.g, self.b, self.a)\n"\
"\n"\
"    @staticmethod\n"\
"    def linear_color(r: float, g: float, b: float, a=1.0) -> \"Color\":\n"\
"        return Color(int(r * 255), int(g * 255), int(b * 255), int(a * 255))\n"\
"\n"\
"    @staticmethod\n"\
"    def BLACK() -> \"Color\":\n"\
"        return Color(0, 0, 0)\n"\
"\n"\
"    @staticmethod\n"\
"    def WHITE() -> \"Color\":\n"\
"        return Color(255, 255, 255)\n"\
"\n"\
"    @staticmethod\n"\
"    def RED() -> \"Color\":\n"\
"        return Color(255, 0, 0)\n"\
"\n"\
"    @staticmethod\n"\
"    def GREEN() -> \"Color\":\n"\
"        return Color(0, 255, 0)\n"\
"\n"\
"    @staticmethod\n"\
"    def BLUE() -> \"Color\":\n"\
"        return Color(0, 0, 255)\n"\
"\n"\
"\n"\
"# MATH\n"\
"class Math:\n"\
"    PI = 3.14159\n"\
"\n"\
"    @staticmethod\n"\
"    def lerp(source: float, destination: float, amount: float) -> float:\n"\
"        return source + (destination - source) * amount\n"\
"\n"\
"\n"\
"class Vector2:\n"\
"    def __init__(self, x=0.0, y=0.0):\n"\
"        self.x = x\n"\
"        self.y = y\n"\
"\n"\
"    def dot_product(self, other) -> float:\n"\
"        return (self.x * other.x) + (self.y * other.y)\n"\
"\n"\
"    def __eq__(self, other) -> bool:\n"\
"        if self.x == other.x and self.y == other.y:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    # TODO: Make a not terrible hash function...\n"\
"    def __hash__(self):\n"\
"        return hash(self.x + (self.y * 0.2))\n"\
"\n"\
"    def __gt__(self, other) -> bool:\n"\
"        if self.x + self.y > other.x + other.y:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __lt__(self, other) -> bool:\n"\
"        if self.x + self.y < other.x + other.y:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __sub__(self, other):\n"\
"        return Vector2(self.x - other.x, self.y - other.y)\n"\
"\n"\
"    def __add__(self, other):\n"\
"        return Vector2(self.x + other.x, self.y + other.y)\n"\
"\n"\
"    def __mul__(self, other):\n"\
"        return Vector2(self.x * other.x, self.y * other.y)\n"\
"\n"\
"    def __truediv__(self, other):\n"\
"        return Vector2(self.x / other.x, self.y / other.y)\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"({self.x}, {self.y})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"({self.x}, {self.y})\"\n"\
"\n"\
"    def magnitude(self) -> float:\n"\
"        return math.sqrt(self.x * self.x + self.y * self.y)\n"\
"\n"\
"    def normalized(self) -> \"Vector2\":\n"\
"        mag = self.magnitude()\n"\
"        self.x = self.x / mag\n"\
"        self.y = self.y / mag\n"\
"        return self\n"\
"\n"\
"    def direction_to(self, target: \"Vector2\") -> \"Vector2\":\n"\
"        return (target - self).normalized()\n"\
"\n"\
"    def distance_to(self, target: \"Vector2\") -> float:\n"\
"        return math.sqrt(\n"\
"            (self.x - target.x) * (self.x - target.x) + (self.y - target.y) * (self.y - target.y)\n"\
"        )\n"\
"\n"\
"    def copy(self) -> \"Vector2\":\n"\
"        return Vector2(self.x, self.y)\n"\
"\n"\
"    @staticmethod\n"\
"    def lerp(source, destination, amount: float) -> float:\n"\
"        return source + (destination - source) * Vector2(amount, amount)\n"\
"\n"\
"    @staticmethod\n"\
"    def ZERO() -> \"Vector2\":\n"\
"        return Vector2(0.0, 0.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def LEFT() -> \"Vector2\":\n"\
"        return Vector2(-1.0, 0.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def RIGHT() -> \"Vector2\":\n"\
"        return Vector2(1.0, 0.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def UP() -> \"Vector2\":\n"\
"        return Vector2(0.0, -1.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def DOWN() -> \"Vector2\":\n"\
"        return Vector2(0.0, 1.0)\n"\
"\n"\
"\n"\
"class Vector3:\n"\
"    def __init__(self, x=0.0, y=0.0, z=0.0):\n"\
"        self.x = x\n"\
"        self.y = y\n"\
"        self.z = z\n"\
"\n"\
"    def __eq__(self, other) -> bool:\n"\
"        if self.x == other.x and self.y == other.y and self.z == other.z:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __gt__(self, other) -> bool:\n"\
"        if self.x + self.y + self.z > other.x + other.y + other.z:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __lt__(self, other) -> bool:\n"\
"        if self.x + self.y + self.z < other.x + other.y + other.z:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __sub__(self, other):\n"\
"        return Vector3(self.x - other.x, self.y - other.y, self.z - other.z)\n"\
"\n"\
"    def __add__(self, other):\n"\
"        return Vector3(self.x + other.x, self.y + other.y, self.z + other.z)\n"\
"\n"\
"    def __mul__(self, other):\n"\
"        return Vector3(self.x * other.x, self.y * other.y, self.z * other.z)\n"\
"\n"\
"    def __truediv__(self, other):\n"\
"        return Vector3(self.x / other.x, self.y / other.y, self.z / other.z)\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"({self.x}, {self.y}, {self.z})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"({self.x}, {self.y}, {self.z})\"\n"\
"\n"\
"\n"\
"class Vector4:\n"\
"    def __init__(self, x=0.0, y=0.0, z=0.0, w=0.0):\n"\
"        self.x = x\n"\
"        self.y = y\n"\
"        self.z = z\n"\
"        self.w = w\n"\
"\n"\
"    def __eq__(self, other) -> bool:\n"\
"        if self.x == other.x and self.y == other.y and self.z == other.z and self.w == other.w:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __gt__(self, other) -> bool:\n"\
"        if self.x + self.y + self.z + self.w > other.x + other.y + other.z + other.w:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __lt__(self, other) -> bool:\n"\
"        if self.x + self.y + self.z + self.w < other.x + other.y + other.z + other.w:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __sub__(self, other):\n"\
"        return Vector4(\n"\
"            self.x - other.x, self.y - other.y, self.z - other.z, self.w - other.w\n"\
"        )\n"\
"\n"\
"    def __add__(self, other):\n"\
"        return Vector4(\n"\
"            self.x + other.x, self.y + other.y, self.z + other.z, self.w + other.w\n"\
"        )\n"\
"\n"\
"    def __mul__(self, other):\n"\
"        return Vector4(\n"\
"            self.x * other.x, self.y * other.y, self.z * other.z, self.w * other.w\n"\
"        )\n"\
"\n"\
"    def __truediv__(self, other):\n"\
"        return Vector4(\n"\
"            self.x / other.x, self.y / other.y, self.z / other.z, self.w / other.w\n"\
"        )\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"({self.x}, {self.y}, {self.z}, {self.w})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"({self.x}, {self.y}, {self.z}, {self.w})\"\n"\
"\n"\
"\n"\
"class Size2D:\n"\
"    def __init__(self, w=0.0, h=0.0):\n"\
"        self.w = w\n"\
"        self.h = h\n"\
"\n"\
"    def __eq__(self, other) -> bool:\n"\
"        if self.w == other.w and self.h == other.h:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __gt__(self, other) -> bool:\n"\
"        if self.total_length() > other.total_length():\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __lt__(self, other) -> bool:\n"\
"        if self.total_length() < other.total_length():\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __sub__(self, other):\n"\
"        return Size2D(self.w - other.w, self.h - other.h)\n"\
"\n"\
"    def __add__(self, other):\n"\
"        return Size2D(self.w + other.w, self.h + other.h)\n"\
"\n"\
"    def __mul__(self, other):\n"\
"        return Size2D(self.w * other.w, self.h * other.h)\n"\
"\n"\
"    def __truediv__(self, other):\n"\
"        return Size2D(self.w / other.w, self.h / other.h)\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"({self.w}, {self.h})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"({self.w}, {self.h})\"\n"\
"\n"\
"    def total_length(self) -> float:\n"\
"        return self.w + self.h\n"\
"\n"\
"    def to_vec2(self) -> Vector2:\n"\
"        return Vector2(self.w, self.h)\n"\
"\n"\
"\n"\
"class Rect2:\n"\
"    def __init__(self, x=0.0, y=0.0, w=0.0, h=0.0):\n"\
"        self.x = x\n"\
"        self.y = y\n"\
"        self.w = w\n"\
"        self.h = h\n"\
"\n"\
"    def __eq__(self, other) -> bool:\n"\
"        if self.x == other.x and self.y == other.y and self.w == other.w and self.h == other.h:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __gt__(self, other) -> bool:\n"\
"        if self.total_length() > other.total_length():\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __lt__(self, other) -> bool:\n"\
"        if self.total_length() < other.total_length():\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __sub__(self, other):\n"\
"        return Rect2(\n"\
"            self.x - other.x, self.y - other.y, self.w - other.w, self.h - other.h\n"\
"        )\n"\
"\n"\
"    def __add__(self, other):\n"\
"        return Rect2(\n"\
"            self.x + other.x, self.y + other.y, self.w + other.w, self.h + other.h\n"\
"        )\n"\
"\n"\
"    def __mul__(self, other):\n"\
"        return Rect2(\n"\
"            self.x * other.x, self.y * other.y, self.w * other.w, self.h * other.h\n"\
"        )\n"\
"\n"\
"    def __truediv__(self, other):\n"\
"        return Rect2(\n"\
"            self.x / other.x, self.y / other.y, self.w / other.w, self.h / other.h\n"\
"        )\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"({self.x}, {self.y}, {self.w}, {self.h})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"({self.x}, {self.y}, {self.w}, {self.h})\"\n"\
"\n"\
"    @property\n"\
"    def position(self) -> Vector2:\n"\
"        return Vector2(self.x, self.y)\n"\
"\n"\
"    @position.setter\n"\
"    def position(self, value: Vector2) -> None:\n"\
"        self.x = value.x\n"\
"        self.y = value.y\n"\
"\n"\
"    @property\n"\
"    def size(self) -> Size2D:\n"\
"        return Size2D(self.w, self.h)\n"\
"\n"\
"    @size.setter\n"\
"    def size(self, value: Size2D) -> None:\n"\
"        self.w = value.w\n"\
"        self.h = value.h\n"\
"\n"\
"    def total_length(self) -> float:\n"\
"        return self.x + self.y + self.w + self.h\n"\
"\n"\
"    def contains_point(self, point: Vector2) -> bool:\n"\
"        return self.x <= point.x <= self.w and self.y <= point.y <= self.h\n"\
"\n"\
"\n"\
"class MinMax:\n"\
"    def __init__(self, value_min: float, value_max: float):\n"\
"        self.min = value_min\n"\
"        self.max = value_max\n"\
"\n"\
"    def contain(self, value: float) -> bool:\n"\
"        return self.min <= value <= self.max\n"\
"\n"\
"    def is_below(self, value: float) -> bool:\n"\
"        return value < self.min\n"\
"\n"\
"    def is_above(self, value: float) -> bool:\n"\
"        return value > self.max\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"({self.min}, {self.max})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"({self.min}, {self.max})\"\n"\
"\n"\
"\n"\
"class Texture:\n"\
"    def __init__(self, file_path: str, wrap_s=\"clamp_to_border\", wrap_t=\"clamp_to_border\", nearest_neighbor=True):\n"\
"        self.file_path = file_path\n"\
"        self.wrap_s = wrap_s\n"\
"        self.wrap_t = wrap_t\n"\
"        self.nearest_neighbor = nearest_neighbor\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, nearest_neighbor: {self.nearest_neighbor})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, nearest_neighbor: {self.nearest_neighbor})\"\n"\
"\n"\
"\n"\
"class Font:\n"\
"    def __init__(self, file_path: str, uid: str, size: int):\n"\
"        self.file_path = file_path\n"\
"        self.uid = uid\n"\
"        self.size = size\n"\
"\n"\
"\n"\
"class InputAction:\n"\
"    def __init__(self, name: str, values: list, device_id=0):\n"\
"        self.name = name\n"\
"        self.values = values\n"\
"        self.device_id = device_id\n"\
"\n"\
"\n"\
"class AnimationFrame:\n"\
"    def __init__(self, frame: int, texture_path: str, draw_source: Rect2):\n"\
"        self.frame = frame\n"\
"        self.texture_path = texture_path\n"\
"        self.draw_source = draw_source\n"\
"\n"\
"\n"\
"class Animation:\n"\
"    def __init__(self, name: str, speed: int, loops: bool, frames: List[AnimationFrame]):\n"\
"        self.name = name\n"\
"        self.speed = speed\n"\
"        self.loops = loops\n"\
"        self.frames = frames\n"\
"\n"\
"\n"\
"class ShaderInstance:\n"\
"    def __init__(self, shader_id: int):\n"\
"        self.shader_id = shader_id\n"\
"        self._is_active = True\n"\
"\n"\
"    def delete(self) -> bool:\n"\
"        if self._is_active:\n"\
"            has_deleted = crescent_internal.shader_instance_delete(self.shader_id)\n"\
"            if has_deleted:\n"\
"                self._is_active = False\n"\
"            return has_deleted\n"\
"        return False\n"\
"\n"\
"    def create_bool_param(self, name: str, initial_value: bool) -> None:\n"\
"        crescent_internal.shader_instance_create_bool_param(self.shader_id, name, initial_value)\n"\
"\n"\
"    def set_bool_param(self, name: str, value: bool) -> None:\n"\
"        crescent_internal.shader_instance_set_bool_param(self.shader_id, name, value)\n"\
"\n"\
"    def get_bool_param(self, name: str) -> bool:\n"\
"        return crescent_internal.shader_instance_get_bool_param(self.shader_id, name)\n"\
"\n"\
"    def create_int_param(self, name: str, initial_value: int) -> None:\n"\
"        crescent_internal.shader_instance_create_int_param(self.shader_id, name, initial_value)\n"\
"\n"\
"    def set_int_param(self, name: str, value: int) -> None:\n"\
"        crescent_internal.shader_instance_set_int_param(self.shader_id, name, value)\n"\
"\n"\
"    def get_int_param(self, name: str) -> int:\n"\
"        return crescent_internal.shader_instance_get_int_param(self.shader_id, name)\n"\
"\n"\
"    def create_float_param(self, name: str, initial_value: float) -> None:\n"\
"        crescent_internal.shader_instance_create_float_param(self.shader_id, name, initial_value)\n"\
"\n"\
"    def set_float_param(self, name: str, value: float) -> None:\n"\
"        crescent_internal.shader_instance_set_float_param(self.shader_id, name, value)\n"\
"\n"\
"    def get_float_param(self, name: str) -> float:\n"\
"        return crescent_internal.shader_instance_get_float_param(self.shader_id, name)\n"\
"\n"\
"    def create_float2_param(self, name: str, initial_value: Vector2) -> None:\n"\
"        crescent_internal.shader_instance_create_float2_param(self.shader_id, name, initial_value.x, initial_value.y)\n"\
"\n"\
"    def set_float2_param(self, name: str, value: Vector2) -> None:\n"\
"        crescent_internal.shader_instance_set_float2_param(self.shader_id, name, value.x, value.y)\n"\
"\n"\
"    def get_float2_param(self, name: str) -> Vector2:\n"\
"        x, y = crescent_internal.shader_instance_get_float2_param(self.shader_id, name)\n"\
"        return Vector2(x, y)\n"\
"\n"\
"    def create_float3_param(self, name: str, initial_value: Vector3) -> None:\n"\
"        crescent_internal.shader_instance_create_float3_param(self.shader_id, name, initial_value.x, initial_value.y, initial_value.z)\n"\
"\n"\
"    def set_float3_param(self, name: str, value: Vector3) -> None:\n"\
"        crescent_internal.shader_instance_set_float3_param(self.shader_id, name, value.x, value.y, value.z)\n"\
"\n"\
"    def get_float3_param(self, name: str) -> Vector3:\n"\
"        x, y, z = crescent_internal.shader_instance_get_float3_param(self.shader_id, name)\n"\
"        return Vector3(x, y, z)\n"\
"\n"\
"    def create_float4_param(self, name: str, initial_value: Vector4) -> None:\n"\
"        crescent_internal.shader_instance_create_float4_param(self.shader_id, name, initial_value.x, initial_value.y, initial_value.z, initial_value.w)\n"\
"\n"\
"    def set_float4_param(self, name: str, value: Vector4) -> None:\n"\
"        crescent_internal.shader_instance_set_float4_param(self.shader_id, name, value.x, value.y, value.z, value.w)\n"\
"\n"\
"    def get_float4_param(self, name: str) -> Vector4:\n"\
"        x, y, z, w = crescent_internal.shader_instance_get_float4_param(self.shader_id, name)\n"\
"        return Vector4(x, y, z, w)\n"\
"\n"\
"\n"\
"class ShaderUtil:\n"\
"    @staticmethod\n"\
"    def compile_shader(shader_path: str) -> ShaderInstance:\n"\
"        shader_id = crescent_internal.shader_util_compile_shader(shader_path)\n"\
"        return ShaderInstance(shader_id)\n"\
"\n"\
"    @staticmethod\n"\
"    def compile_shader_raw(vertex_path: str, fragment_path: str) -> ShaderInstance:\n"\
"        shader_id = crescent_internal.shader_util_compile_shader_raw(vertex_path, fragment_path)\n"\
"        return ShaderInstance(shader_id)\n"\
"\n"\
"    @staticmethod\n"\
"    def set_screen_shader(shader_instance: ShaderInstance) -> bool:\n"\
"        return crescent_internal.shader_util_set_screen_shader(shader_instance.shader_id)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_current_screen_shader() -> Optional[ShaderInstance]:\n"\
"        shader_instance_id = (crescent_internal.shader_util_get_current_screen_shader())\n"\
"        if shader_instance_id >= 0:\n"\
"            return ShaderInstance(shader_instance_id)\n"\
"        return None\n"\
"\n"\
"    @staticmethod\n"\
"    def reset_screen_shader_to_default() -> None:\n"\
"        crescent_internal.shader_util_reset_screen_shader_to_default()\n"\
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
"        subscriber = _NodeEventSubscriber(entity_id, call_back, self.entity_id, self.name)\n"\
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
"    \"\"\"\n"\
"    Used to manage events between nodes\n"\
"    \"\"\"\n"\
"    def __init__(self, events: Dict[int, Dict[str, _NodeEvent]] = None, entity_subscribers: Dict[int, List[_NodeEventSubscriber]] = None) -> None:\n"\
"        if not events:\n"\
"            events = {}\n"\
"        if not entity_subscribers:\n"\
"            entity_subscribers = {}\n"\
"        self.events = events\n"\
"        self.entity_subscribers = entity_subscribers\n"\
"\n"\
"    def create_event(self, entity_id: int, event_name: str) -> _NodeEvent:\n"\
"        if entity_id not in self.events:\n"\
"            self.events[entity_id] = {}\n"\
"        if event_name not in self.events[entity_id]:\n"\
"            self.events[entity_id][event_name] = _NodeEvent(entity_id, event_name)\n"\
"        return self.events[entity_id][event_name]\n"\
"\n"\
"    def remove_event(self, entity_id: int, event_name: str) -> None:\n"\
"        if self.has_event(entity_id, event_name):\n"\
"            del self.events[entity_id][event_name]\n"\
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
"            sub.call_back(*args)\n"\
"\n"\
"    def subscribe_to_event(self, owner_entity: int, event_name: str, subscriber_entity: int, subscriber_call_back: Callable[[...], None]) -> None:\n"\
"        event = self.create_event(owner_entity, event_name)\n"\
"        subscriber = event.add_or_update_subscriber(\n"\
"            subscriber_entity, subscriber_call_back\n"\
"        )\n"\
"        if subscriber_entity not in self.entity_subscribers:\n"\
"            self.entity_subscribers[subscriber_entity] = []\n"\
"        sub_list = self.entity_subscribers.get(subscriber_entity, [])\n"\
"        if subscriber not in sub_list:\n"\
"            sub_list.append(subscriber)\n"\
"\n"\
"    def clear_all_data(self) -> None:\n"\
"        self.events.clear()\n"\
"        self.entity_subscribers.clear()\n"\
"\n"\
"\n"\
"_node_event_manager = _NodeEventManager()\n"\
"\n"\
"\n"\
"class NodeType:\n"\
"    Node = 1\n"\
"    Node2D = 3\n"\
"\n"\
"\n"\
"class Node:\n"\
"    def __init__(self, entity_id: int) -> None:\n"\
"        self.entity_id = entity_id\n"\
"\n"\
"    # TODO: Replace with proper method once class method is added\n"\
"    @staticmethod\n"\
"    def new() -> \"Node\":\n"\
"        return crescent_internal.node_new(\"crescent\", \"Node\", NodeType.Node)\n"\
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
"            if isinstance(children, tuple):\n"\
"                return list(children)\n"\
"            elif isinstance(children, Node):\n"\
"                return [children]\n"\
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
"        _node_event_manager.create_event(self.entity_id, event_name)\n"\
"\n"\
"    def subscribe_to_event(self, event_name: str, subscriber: \"Node\", callback_func: Callable[[...], None]) -> None:\n"\
"        _node_event_manager.subscribe_to_event(self.entity_id, event_name, subscriber.entity_id, callback_func)\n"\
"\n"\
"    def broadcast_event(self, event_name: str, *args) -> None:\n"\
"        _node_event_manager.broadcast_event(self.entity_id, event_name, *args)\n"\
"\n"\
"    def __eq__(self, other: \"Node\") -> bool:\n"\
"        return self.entity_id == other.entity_id\n"\
"\n"\
"    def __str__(self) -> str:\n"\
"        return f\"{self.__class__.__name__}(entity_id: {self.entity_id})\"\n"\
"\n"\
"    def __repr__(self) -> str:\n"\
"        return f\"{self.__class__.__name__}(entity_id: {self.entity_id})\"\n"\
"\n"\
"\n"\
"class Node2D(Node):\n"\
"    @staticmethod\n"\
"    def new() -> \"Node2D\":\n"\
"        return crescent_internal.node_new(\"crescent\", \"Node2D\", NodeType.Node2D)\n"\
"\n"\
"    def set_position(self, value: Vector2) -> None:\n"\
"        crescent_internal.node2d_set_position(self.entity_id, value.x, value.y\n"\
"        )\n"\
"\n"\
"    def add_to_position(self, value: Vector2) -> None:\n"\
"        crescent_internal.node2d_add_to_position(self.entity_id, value.x, value.y\n"\
"        )\n"\
"\n"\
"    def get_position(self) -> Vector2:\n"\
"        px, py = crescent_internal.node2d_get_position(self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @property\n"\
"    def position(self) -> Vector2:\n"\
"        px, py = crescent_internal.node2d_get_position(self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @position.setter\n"\
"    def position(self, value: Vector2) -> None:\n"\
"        crescent_internal.node2d_set_position(self.entity_id, value.x, value.y)\n"\
"\n"\
"    def get_global_position(self) -> Vector2:\n"\
"        px, py = crescent_internal.node2d_get_global_position(self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @property\n"\
"    def global_position(self) -> Vector2:\n"\
"        px, py = crescent_internal.node2d_get_global_position(self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    def set_scale(self, value: Vector2) -> None:\n"\
"        crescent_internal.node2d_set_scale(self.entity_id, value.x, value.y)\n"\
"\n"\
"    def add_to_scale(self, value: Vector2) -> None:\n"\
"        crescent_internal.node2d_add_to_scale(self.entity_id, value.x, value.y)\n"\
"\n"\
"    def get_scale(self) -> Vector2:\n"\
"        sx, sy = crescent_internal.node2d_get_scale(self.entity_id)\n"\
"        return Vector2(sx, sy)\n"\
"\n"\
"    @property\n"\
"    def scale(self) -> Vector2:\n"\
"        sx, sy = crescent_internal.node2d_get_scale(self.entity_id)\n"\
"        return Vector2(sx, sy)\n"\
"\n"\
"    @scale.setter\n"\
"    def scale(self, value: Vector2) -> None:\n"\
"        crescent_internal.node2d_set_scale(self.entity_id, value.x, value.y)\n"\
"\n"\
"    def set_rotation(self, value: float) -> None:\n"\
"        crescent_internal.node2d_set_rotation(self.entity_id, value)\n"\
"\n"\
"    def add_to_rotation(self, value: float) -> None:\n"\
"        crescent_internal.node2d_add_to_rotation(self.entity_id, value)\n"\
"\n"\
"    def get_rotation(self) -> float:\n"\
"        return crescent_internal.node2d_get_rotation(self.entity_id)\n"\
"\n"\
"    @property\n"\
"    def rotation(self) -> float:\n"\
"        return crescent_internal.node2d_get_rotation(self.entity_id)\n"\
"\n"\
"    @rotation.setter\n"\
"    def rotation(self, value: float) -> None:\n"\
"        crescent_internal.node2d_set_rotation(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def z_index(self) -> int:\n"\
"        return crescent_internal.node2d_get_z_index(self.entity_id)\n"\
"\n"\
"    @z_index.setter\n"\
"    def z_index(self, value: int) -> None:\n"\
"        crescent_internal.node2d_set_z_index(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def z_index_relative_to_parent(self) -> bool:\n"\
"        return crescent_internal.node2d_get_z_index_relative_to_parent(self.entity_id)\n"\
"\n"\
"    @z_index_relative_to_parent.setter\n"\
"    def z_index_relative_to_parent(self, value: bool) -> None:\n"\
"        crescent_internal.node2d_set_z_index_relative_to_parent(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def ignore_camera(self) -> bool:\n"\
"        return crescent_internal.node2d_get_ignore_camera(self.entity_id)\n"\
"\n"\
"    @ignore_camera.setter\n"\
"    def ignore_camera(self, value: bool) -> None:\n"\
"        crescent_internal.node2d_set_ignore_camera(self.entity_id, value)\n"\
"\n"\
"\n"\
"class Sprite(Node2D):\n"\
"    @property\n"\
"    def texture(self) -> Texture:\n"\
"        file_path, wrap_s, wrap_t, nearest_neighbor = crescent_internal.sprite_get_texture(self.entity_id)\n"\
"        return Texture(file_path, wrap_s, wrap_t, nearest_neighbor)\n"\
"\n"\
"    @texture.setter\n"\
"    def texture(self, value: Texture) -> None:\n"\
"        crescent_internal.sprite_set_texture(self.entity_id, value.file_path)\n"\
"\n"\
"    @property\n"\
"    def draw_source(self) -> Rect2:\n"\
"        x, y, w, h = crescent_internal.sprite_get_draw_source(self.entity_id)\n"\
"        return Rect2(x=x, y=y, w=w, h=h)\n"\
"\n"\
"    @draw_source.setter\n"\
"    def draw_source(self, value: Rect2) -> None:\n"\
"        crescent_internal.sprite_set_draw_source(self.entity_id, value.x, value.y, value.w, value.h)\n"\
"\n"\
"    @property\n"\
"    def flip_h(self) -> bool:\n"\
"        return crescent_internal.sprite_get_flip_h(self.entity_id)\n"\
"\n"\
"    @flip_h.setter\n"\
"    def flip_h(self, value: bool) -> None:\n"\
"        crescent_internal.sprite_set_flip_h(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def flip_v(self) -> bool:\n"\
"        return crescent_internal.sprite_get_flip_v(self.entity_id)\n"\
"\n"\
"    @flip_v.setter\n"\
"    def flip_v(self, value: bool) -> None:\n"\
"        crescent_internal.sprite_set_flip_v(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def modulate(self) -> Color:\n"\
"        red, green, blue, alpha = crescent_internal.sprite_get_modulate(self.entity_id)\n"\
"        return Color(red, green, blue, alpha)\n"\
"\n"\
"    @modulate.setter\n"\
"    def modulate(self, color: Color) -> None:\n"\
"        crescent_internal.sprite_set_modulate(self.entity_id, color.r, color.g, color.b, color.a)\n"\
"\n"\
"    @property\n"\
"    def origin(self) -> Vector2:\n"\
"        x, y = crescent_internal.sprite_get_origin(self.entity_id)\n"\
"        return Vector2(x, y)\n"\
"\n"\
"    @origin.setter\n"\
"    def origin(self, value: Vector2) -> None:\n"\
"        crescent_internal.sprite_set_origin(self.entity_id, value.x, value.y)\n"\
"\n"\
"    @property\n"\
"    def shader_instance(self) -> Optional[ShaderInstance]:\n"\
"        shader_instance_id = crescent_internal.sprite_get_shader_instance(self.entity_id)\n"\
"        if shader_instance_id >= 0:\n"\
"            return ShaderInstance(shader_instance_id)\n"\
"        return None\n"\
"\n"\
"    @shader_instance.setter\n"\
"    def shader_instance(self, value: ShaderInstance) -> None:\n"\
"        crescent_internal.sprite_set_shader_instance(self.entity_id, value.shader_id)\n"\
"\n"\
"\n"\
"class AnimatedSprite(Node2D):\n"\
"    def play(self, name: str) -> bool:\n"\
"        return crescent_internal.animated_sprite_play(self.entity_id, name)\n"\
"\n"\
"    def stop(self) -> None:\n"\
"        crescent_internal.animated_sprite_stop(self.entity_id)\n"\
"\n"\
"    def set_current_animation_frame(self, frame: int) -> None:\n"\
"        crescent_internal.animated_sprite_set_current_animation_frame(self.entity_id, frame)\n"\
"\n"\
"    def add_animation(self, animation: Animation) -> None:\n"\
"        crescent_internal.animated_sprite_add_animation(self.entity_id, animation.name, animation.speed, animation.loops, len(animation.frames), *animation.frames)\n"\
"\n"\
"    @property\n"\
"    def flip_h(self) -> bool:\n"\
"        return crescent_internal.animated_sprite_get_flip_h(self.entity_id)\n"\
"\n"\
"    @flip_h.setter\n"\
"    def flip_h(self, value: bool) -> None:\n"\
"        crescent_internal.animated_sprite_set_flip_h(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def flip_v(self) -> bool:\n"\
"        return crescent_internal.animated_sprite_get_flip_v(self.entity_id)\n"\
"\n"\
"    @flip_v.setter\n"\
"    def flip_v(self, value: bool) -> None:\n"\
"        crescent_internal.animated_sprite_set_flip_v(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def modulate(self) -> Color:\n"\
"        red, green, blue, alpha = crescent_internal.animated_sprite_get_modulate(self.entity_id)\n"\
"        return Color(r=red, g=green, b=blue, a=alpha)\n"\
"\n"\
"    @modulate.setter\n"\
"    def modulate(self, color: Color) -> None:\n"\
"        crescent_internal.animated_sprite_set_modulate(self.entity_id, color.r, color.g, color.b, color.a)\n"\
"\n"\
"    @property\n"\
"    def origin(self) -> Vector2:\n"\
"        x, y = crescent_internal.animated_sprite_get_origin(self.entity_id)\n"\
"        return Vector2(x, y)\n"\
"\n"\
"    @origin.setter\n"\
"    def origin(self, value: Vector2) -> None:\n"\
"        crescent_internal.animated_sprite_set_origin(self.entity_id, value.x, value.y)\n"\
"\n"\
"    @property\n"\
"    def stagger_animation_start_times(self) -> bool:\n"\
"        return crescent_internal.animated_sprite_get_stagger_animation_start_times(self.entity_id)\n"\
"\n"\
"    @stagger_animation_start_times.setter\n"\
"    def stagger_animation_start_times(self, value: bool) -> None:\n"\
"        crescent_internal.animated_sprite_set_stagger_animation_start_times(self.entity_id, value)\n"\
"\n"\
"    @property\n"\
"    def shader_instance(self) -> Optional[ShaderInstance]:\n"\
"        shader_instance_id = crescent_internal.animated_sprite_get_shader_instance(self.entity_id)\n"\
"        if shader_instance_id >= 0:\n"\
"            return ShaderInstance(shader_instance_id)\n"\
"        return None\n"\
"\n"\
"    @shader_instance.setter\n"\
"    def shader_instance(self, value: ShaderInstance) -> None:\n"\
"        crescent_internal.animated_sprite_set_shader_instance(self.entity_id, value.shader_id)\n"\
"\n"\
"\n"\
"class TextLabel(Node2D):\n"\
"    @property\n"\
"    def text(self) -> str:\n"\
"        return crescent_internal.text_label_get_text(self.entity_id)\n"\
"\n"\
"    @text.setter\n"\
"    def text(self, value: str) -> None:\n"\
"        crescent_internal.text_label_set_text(self.entity_id, value)\n"\
"\n"\
"    def get_text(self) -> str:\n"\
"        return crescent_internal.text_label_get_text(self.entity_id)\n"\
"\n"\
"    def set_text(self, text: str) -> None:\n"\
"        crescent_internal.text_label_set_text(self.entity_id, text)\n"\
"\n"\
"    @property\n"\
"    def color(self) -> Color:\n"\
"        r, g, b, a = crescent_internal.text_label_get_color(self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    @color.setter\n"\
"    def color(self, value: Color) -> None:\n"\
"        crescent_internal.text_label_set_color(self.entity_id, value.r, value.g, value.b, value.a)\n"\
"\n"\
"    def get_color(self) -> Color:\n"\
"        r, g, b, a = crescent_internal.text_label_get_color(self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    def set_color(self, color: Color) -> None:\n"\
"        crescent_internal.text_label_set_color(self.entity_id, color.r, color.g, color.b, color.a)\n"\
"\n"\
"    def set_font(self, uid: str) -> None:\n"\
"        crescent_internal.text_label_set_font_uid(self.entity_id, uid)\n"\
"\n"\
"\n"\
"class Collider2D(Node2D):\n"\
"    def get_extents(self) -> Size2D:\n"\
"        w, h = crescent_internal.collider2d_get_extents(self.entity_id)\n"\
"        return Size2D(w, h)\n"\
"\n"\
"    def set_extents(self, extents: Size2D) -> None:\n"\
"        crescent_internal.collider2d_set_extents(self.entity_id, extents.w, extents.h)\n"\
"\n"\
"    @property\n"\
"    def extents(self) -> Size2D:\n"\
"        w, h = crescent_internal.collider2d_get_extents(self.entity_id)\n"\
"        return Size2D(w, h)\n"\
"\n"\
"    @extents.setter\n"\
"    def extents(self, value: Size2D) -> None:\n"\
"        crescent_internal.collider2d_set_extents(self.entity_id, value.w, value.h)\n"\
"\n"\
"    def get_color(self) -> Color:\n"\
"        r, g, b, a = crescent_internal.collider2d_get_color(self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    def set_color(self, color: Color) -> None:\n"\
"        crescent_internal.collider2d_set_color(self.entity_id, color.r, color.g, color.b, color.a)\n"\
"\n"\
"    @property\n"\
"    def color(self) -> Color:\n"\
"        r, g, b, a = crescent_internal.collider2d_get_color(self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    @color.setter\n"\
"    def color(self, value: Color) -> None:\n"\
"        crescent_internal.collider2d_set_color(self.entity_id, value.r, value.g, value.b, value.a)\n"\
"\n"\
"\n"\
"class ColorRect(Node2D):\n"\
"    def get_size(self) -> Size2D:\n"\
"        w, h = crescent_internal.color_rect_get_size(self.entity_id)\n"\
"        return Size2D(w, h)\n"\
"\n"\
"    def set_size(self, extents: Size2D) -> None:\n"\
"        crescent_internal.color_rect_set_size(self.entity_id, extents.w, extents.h)\n"\
"\n"\
"    @property\n"\
"    def size(self) -> Size2D:\n"\
"        w, h = crescent_internal.color_rect_get_size(self.entity_id)\n"\
"        return Size2D(w, h)\n"\
"\n"\
"    @size.setter\n"\
"    def size(self, value: Size2D) -> None:\n"\
"        crescent_internal.color_rect_set_size(self.entity_id, value.w, value.h)\n"\
"\n"\
"    def get_color(self) -> Color:\n"\
"        r, g, b, a = crescent_internal.color_rect_get_color(self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    def set_color(self, color: Color) -> None:\n"\
"        crescent_internal.color_rect_set_color(self.entity_id, color.r, color.g, color.b, color.a)\n"\
"\n"\
"    @property\n"\
"    def color(self) -> Color:\n"\
"        r, g, b, a = crescent_internal.color_rect_get_color(self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    @color.setter\n"\
"    def color(self, value: Color) -> None:\n"\
"        crescent_internal.color_rect_set_color(self.entity_id, value.r, value.g, value.b, value.a)\n"\
"\n"\
"\n"\
"class Parallax(Node2D):\n"\
"    @property\n"\
"    def scroll_speed(self) -> Vector2:\n"\
"        x, y = crescent_internal.parallax_get_scroll_speed(self.entity_id)\n"\
"        return Vector2(x, y)\n"\
"\n"\
"    @scroll_speed.setter\n"\
"    def scroll_speed(self, value: Vector2) -> None:\n"\
"        crescent_internal.parallax_set_scroll_speed(self.entity_id, value.x, value.y)\n"\
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
"\n"

