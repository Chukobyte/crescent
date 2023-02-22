#pragma once

#define RBE_PY_API_SOURCE_VERSION "0.0.1"

#define RBE_PY_API_SOURCE_IMPORTER ""\
"import sys\n"\
"import importlib\n"\
"import importlib.util\n"\
"\n"\
"module_name = \"source_importer\"\n"\
"source_code = \"\"\"\n"\
"import sys\n"\
"import importlib\n"\
"import importlib.util\n"\
"\n"\
"\n"\
"class SourceImporter:\n"\
"    @staticmethod\n"\
"    def import_from_source(module_name: str, source_code: str, reload_module=False):\n"\
"        if module_name not in sys.modules or reload_module:\n"\
"            spec = importlib.util.spec_from_loader(\n"\
"                module_name, loader=None, is_package=False\n"\
"            )\n"\
"            module = importlib.util.module_from_spec(spec)\n"\
"            exec(source_code, module.__dict__)\n"\
"            sys.modules[module_name] = module\n"\
"        return sys.modules[module_name]\n"\
"\"\"\"\n"\
"\n"\
"# Load source importer module\n"\
"spec = importlib.util.spec_from_loader(module_name, loader=None, is_package=False)\n"\
"module = importlib.util.module_from_spec(spec)\n"\
"exec(source_code, module.__dict__)\n"\
"sys.modules[module_name] = module\n"\
"\n"

#define RBE_PY_API_SOURCE ""\
"import json\n"\
"from enum import Enum\n"\
"from json import JSONDecodeError\n"\
"from typing import Callable, Type, List\n"\
"\n"\
"import crescent_api_internal\n"\
"\n"\
"import math\n"\
"\n"\
"\n"\
"# COLOR\n"\
"class Color:\n"\
"    # 255 is the limit...\n"\
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
"            (self.x - target.x) * (self.x - target.x)\n"\
"            + (self.y - target.y) * (self.y - target.y)\n"\
"        )\n"\
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
"        if (\n"\
"            self.x == other.x\n"\
"            and self.y == other.y\n"\
"            and self.z == other.z\n"\
"            and self.w == other.w\n"\
"        ):\n"\
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
"        if (\n"\
"            self.x == other.x\n"\
"            and self.y == other.y\n"\
"            and self.w == other.w\n"\
"            and self.h == other.h\n"\
"        ):\n"\
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
"    def size(self) -> Vector2:\n"\
"        return Vector2(self.w, self.h)\n"\
"\n"\
"    @size.setter\n"\
"    def size(self, value: Vector2) -> None:\n"\
"        self.w = value.x\n"\
"        self.h = value.y\n"\
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
"\n"\
"class CurveFloatPoint:\n"\
"    def __init__(self, x: float, y: float, tangent_in: float, tangent_out: float):\n"\
"        self.x = x\n"\
"        self.y = y\n"\
"        self.tangent_in = tangent_in\n"\
"        self.tangent_out = tangent_out\n"\
"\n"\
"\n"\
"class CurveFloat:\n"\
"    def __init__(self, file_path: str = None):\n"\
"        self._valid = False\n"\
"        self._id = crescent_api_internal.curve_float_create_new()\n"\
"        if file_path:\n"\
"            if crescent_api_internal.curve_float_load_from_file(\n"\
"                curve_id=self._id, file_path=file_path\n"\
"            ):\n"\
"                self._valid = True\n"\
"        else:\n"\
"            self._id = crescent_api_internal.curve_float_create_new()\n"\
"            self._valid = True\n"\
"\n"\
"    @property\n"\
"    def valid(self) -> bool:\n"\
"        return self._valid\n"\
"\n"\
"    def add_point(self, point: CurveFloatPoint) -> None:\n"\
"        crescent_api_internal.curve_float_add_point(\n"\
"            curve_id=self._id,\n"\
"            x=point.x,\n"\
"            y=point.y,\n"\
"            tangent_in=point.tangent_in,\n"\
"            tangent_out=point.tangent_out,\n"\
"        )\n"\
"\n"\
"    def remove_point(self, x: float, y: float) -> bool:\n"\
"        return crescent_api_internal.curve_float_remove_point(\n"\
"            curve_id=self._id, x=x, y=y\n"\
"        )\n"\
"\n"\
"    def eval(self, t: float) -> float:\n"\
"        return crescent_api_internal.curve_float_eval(curve_id=self._id, t=t)\n"\
"\n"\
"    def cleanup(self) -> None:\n"\
"        if self._valid:\n"\
"            crescent_api_internal.curve_float_delete(curve_id=self._id)\n"\
"            self._valid = False\n"\
"\n"\
"    def __del__(self):\n"\
"        self.cleanup()\n"\
"\n"\
"\n"\
"# ASSETS\n"\
"class AudioSource:\n"\
"    def __init__(self, file_path: str):\n"\
"        self.file_path = file_path\n"\
"\n"\
"\n"\
"class Texture:\n"\
"    def __init__(\n"\
"        self,\n"\
"        file_path: str,\n"\
"        wrap_s=\"clamp_to_border\",\n"\
"        wrap_t=\"clamp_to_border\",\n"\
"        filter_min=\"nearest\",\n"\
"        filter_mag=\"nearest\",\n"\
"    ):\n"\
"        self.file_path = file_path\n"\
"        self.wrap_s = wrap_s\n"\
"        self.wrap_t = wrap_t\n"\
"        self.filter_min = filter_min\n"\
"        self.filter_mag = filter_mag\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, filter_min: {self.filter_min}, filter_mag: {self.filter_mag})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"Texture(file_path: {self.file_path}, wrap_s: {self.wrap_s}, wrap_s: {self.wrap_t}, filter_min: {self.filter_min}, filter_mag: {self.filter_mag})\"\n"\
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
"    def __init__(\n"\
"        self, name: str, speed: int, loops: bool, frames: List[AnimationFrame]\n"\
"    ):\n"\
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
"            has_deleted = crescent_api_internal.shader_instance_delete(\n"\
"                shader_id=self.shader_id\n"\
"            )\n"\
"            if has_deleted:\n"\
"                self._is_active = False\n"\
"            return has_deleted\n"\
"        return False\n"\
"\n"\
"    def create_bool_param(self, name: str, initial_value: bool) -> None:\n"\
"        crescent_api_internal.shader_instance_create_bool_param(\n"\
"            shader_id=self.shader_id, name=name, initial_value=initial_value\n"\
"        )\n"\
"\n"\
"    def set_bool_param(self, name: str, value: bool) -> None:\n"\
"        crescent_api_internal.shader_instance_set_bool_param(\n"\
"            shader_id=self.shader_id, name=name, value=value\n"\
"        )\n"\
"\n"\
"    def get_bool_param(self, name: str) -> bool:\n"\
"        return crescent_api_internal.shader_instance_get_bool_param(\n"\
"            shader_id=self.shader_id, name=name\n"\
"        )\n"\
"\n"\
"    def create_int_param(self, name: str, initial_value: int) -> None:\n"\
"        crescent_api_internal.shader_instance_create_int_param(\n"\
"            shader_id=self.shader_id, name=name, initial_value=initial_value\n"\
"        )\n"\
"\n"\
"    def set_int_param(self, name: str, value: int) -> None:\n"\
"        crescent_api_internal.shader_instance_set_int_param(\n"\
"            shader_id=self.shader_id, name=name, value=value\n"\
"        )\n"\
"\n"\
"    def get_int_param(self, name: str) -> int:\n"\
"        return crescent_api_internal.shader_instance_get_int_param(\n"\
"            shader_id=self.shader_id, name=name\n"\
"        )\n"\
"\n"\
"    def create_float_param(self, name: str, initial_value: float) -> None:\n"\
"        crescent_api_internal.shader_instance_create_float_param(\n"\
"            shader_id=self.shader_id, name=name, initial_value=initial_value\n"\
"        )\n"\
"\n"\
"    def set_float_param(self, name: str, value: float) -> None:\n"\
"        crescent_api_internal.shader_instance_set_float_param(\n"\
"            shader_id=self.shader_id, name=name, value=value\n"\
"        )\n"\
"\n"\
"    def get_float_param(self, name: str) -> float:\n"\
"        return crescent_api_internal.shader_instance_get_float_param(\n"\
"            shader_id=self.shader_id, name=name\n"\
"        )\n"\
"\n"\
"    def create_float2_param(self, name: str, initial_value: Vector2) -> None:\n"\
"        crescent_api_internal.shader_instance_create_float2_param(\n"\
"            shader_id=self.shader_id,\n"\
"            name=name,\n"\
"            initial_value_x=initial_value.x,\n"\
"            initial_value_y=initial_value.y,\n"\
"        )\n"\
"\n"\
"    def set_float2_param(self, name: str, value: Vector2) -> None:\n"\
"        crescent_api_internal.shader_instance_set_float2_param(\n"\
"            shader_id=self.shader_id, name=name, value_x=value.x, value_y=value.y\n"\
"        )\n"\
"\n"\
"    def get_float2_param(self, name: str) -> Vector2:\n"\
"        x, y = crescent_api_internal.shader_instance_get_float2_param(\n"\
"            shader_id=self.shader_id, name=name\n"\
"        )\n"\
"        return Vector2(x, y)\n"\
"\n"\
"    def create_float3_param(self, name: str, initial_value: Vector3) -> None:\n"\
"        crescent_api_internal.shader_instance_create_float3_param(\n"\
"            shader_id=self.shader_id,\n"\
"            name=name,\n"\
"            initial_value_x=initial_value.x,\n"\
"            initial_value_y=initial_value.y,\n"\
"            initial_value_z=initial_value.z,\n"\
"        )\n"\
"\n"\
"    def set_float3_param(self, name: str, value: Vector3) -> None:\n"\
"        crescent_api_internal.shader_instance_set_float3_param(\n"\
"            shader_id=self.shader_id,\n"\
"            name=name,\n"\
"            value_x=value.x,\n"\
"            value_y=value.y,\n"\
"            value_z=value.z,\n"\
"        )\n"\
"\n"\
"    def get_float3_param(self, name: str) -> Vector3:\n"\
"        x, y, z = crescent_api_internal.shader_instance_get_float3_param(\n"\
"            shader_id=self.shader_id, name=name\n"\
"        )\n"\
"        return Vector3(x, y, z)\n"\
"\n"\
"    def create_float4_param(self, name: str, initial_value: Vector4) -> None:\n"\
"        crescent_api_internal.shader_instance_create_float4_param(\n"\
"            shader_id=self.shader_id,\n"\
"            name=name,\n"\
"            initial_value_x=initial_value.x,\n"\
"            initial_value_y=initial_value.y,\n"\
"            initial_value_z=initial_value.z,\n"\
"            initial_value_w=initial_value.w,\n"\
"        )\n"\
"\n"\
"    def set_float4_param(self, name: str, value: Vector4) -> None:\n"\
"        crescent_api_internal.shader_instance_set_float4_param(\n"\
"            shader_id=self.shader_id,\n"\
"            name=name,\n"\
"            value_x=value.x,\n"\
"            value_y=value.y,\n"\
"            value_z=value.z,\n"\
"            value_w=value.w,\n"\
"        )\n"\
"\n"\
"    def get_float4_param(self, name: str) -> Vector4:\n"\
"        x, y, z, w = crescent_api_internal.shader_instance_get_float4_param(\n"\
"            shader_id=self.shader_id, name=name\n"\
"        )\n"\
"        return Vector4(x, y, z, w)\n"\
"\n"\
"\n"\
"class ShaderUtil:\n"\
"    @staticmethod\n"\
"    def compile_shader(vertex_path: str, fragment_path: str) -> ShaderInstance:\n"\
"        shader_id = crescent_api_internal.shader_util_compile_shader(\n"\
"            vertex_path=vertex_path, fragment_path=fragment_path\n"\
"        )\n"\
"        return ShaderInstance(shader_id=shader_id)\n"\
"\n"\
"    @staticmethod\n"\
"    def set_screen_shader(shader_instance: ShaderInstance) -> bool:\n"\
"        return crescent_api_internal.shader_util_set_screen_shader(\n"\
"            shader_id=shader_instance.shader_id\n"\
"        )\n"\
"\n"\
"    @staticmethod\n"\
"    def reset_screen_shader_to_default() -> None:\n"\
"        crescent_api_internal.shader_util_reset_screen_shader_to_default()\n"\
"\n"\
"\n"\
"# ENGINE\n"\
"class Engine:\n"\
"    @staticmethod\n"\
"    def exit(code=0) -> None:\n"\
"        crescent_api_internal.engine_exit(code=code)\n"\
"\n"\
"    @staticmethod\n"\
"    def set_target_fps(fps: int) -> None:\n"\
"        crescent_api_internal.engine_set_target_fps(fps=fps)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_target_fps() -> int:\n"\
"        return crescent_api_internal.engine_get_target_fps()\n"\
"\n"\
"    @staticmethod\n"\
"    def get_average_fps() -> float:\n"\
"        return crescent_api_internal.engine_get_average_fps()\n"\
"\n"\
"    @staticmethod\n"\
"    def set_fps_display_enabled(enabled: bool) -> None:\n"\
"        crescent_api_internal.engine_set_fps_display_enabled(enabled=enabled)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_global_physics_delta_time() -> float:\n"\
"        return crescent_api_internal.engine_get_global_physics_delta_time()\n"\
"\n"\
"\n"\
"# INPUT\n"\
"class Input:\n"\
"    @staticmethod\n"\
"    def add_action(name: str, value: str, device_id=0) -> None:\n"\
"        crescent_api_internal.input_add_action(\n"\
"            name=name, value=value, device_id=device_id\n"\
"        )\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_pressed(name: str) -> bool:\n"\
"        return crescent_api_internal.input_is_action_pressed(name=name)\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_just_pressed(name: str) -> bool:\n"\
"        return crescent_api_internal.input_is_action_just_pressed(name=name)\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_just_released(name: str) -> bool:\n"\
"        return crescent_api_internal.input_is_action_just_released(name=name)\n"\
"\n"\
"    class Mouse:\n"\
"        LEFT_BUTTON = \"mb_left\"\n"\
"        RIGHT_BUTTON = \"mb_right\"\n"\
"\n"\
"        @staticmethod\n"\
"        def get_position() -> Vector2:\n"\
"            mouse_x, mouse_y = crescent_api_internal.mouse_get_position()\n"\
"            return Vector2(mouse_x, mouse_y)\n"\
"\n"\
"        @staticmethod\n"\
"        def get_world_position() -> Vector2:\n"\
"            mouse_x, mouse_y = crescent_api_internal.mouse_get_world_position()\n"\
"            return Vector2(mouse_x, mouse_y)\n"\
"\n"\
"    class Keyboard:\n"\
"        NUM_0 = \"0\"\n"\
"        NUM_1 = \"1\"\n"\
"        NUM_2 = \"2\"\n"\
"        NUM_3 = \"3\"\n"\
"        NUM_4 = \"4\"\n"\
"        NUM_5 = \"5\"\n"\
"        NUM_6 = \"6\"\n"\
"        NUM_7 = \"7\"\n"\
"        NUM_8 = \"8\"\n"\
"        NUM_9 = \"9\"\n"\
"\n"\
"        A = \"a\"\n"\
"        B = \"b\"\n"\
"        C = \"c\"\n"\
"        D = \"d\"\n"\
"        E = \"e\"\n"\
"        F = \"f\"\n"\
"        G = \"g\"\n"\
"        H = \"h\"\n"\
"        I = \"i\"\n"\
"        J = \"j\"\n"\
"        K = \"k\"\n"\
"        L = \"l\"\n"\
"        M = \"m\"\n"\
"        N = \"n\"\n"\
"        O = \"o\"\n"\
"        P = \"p\"\n"\
"        Q = \"q\"\n"\
"        R = \"r\"\n"\
"        S = \"s\"\n"\
"        T = \"t\"\n"\
"        U = \"u\"\n"\
"        V = \"v\"\n"\
"        W = \"w\"\n"\
"        X = \"x\"\n"\
"        Y = \"y\"\n"\
"        Z = \"z\"\n"\
"\n"\
"        UP = \"up\"\n"\
"        DOWN = \"down\"\n"\
"        LEFT = \"left\"\n"\
"        RIGHT = \"right\"\n"\
"        SPACE = \"space\"\n"\
"        ESC = \"esc\"\n"\
"        RETURN = \"return\"\n"\
"\n"\
"        F1 = \"f1\"\n"\
"        F2 = \"f2\"\n"\
"        F3 = \"f3\"\n"\
"        F4 = \"f4\"\n"\
"        F5 = \"f5\"\n"\
"        F6 = \"f6\"\n"\
"        F7 = \"f7\"\n"\
"        F8 = \"f8\"\n"\
"        F9 = \"f9\"\n"\
"        F10 = \"f10\"\n"\
"        F11 = \"f11\"\n"\
"        F12 = \"f12\"\n"\
"\n"\
"    class Gamepad:\n"\
"        BUTTON_A = \"joystick_button_a\"\n"\
"        BUTTON_B = \"joystick_button_b\"\n"\
"        BUTTON_X = \"joystick_button_x\"\n"\
"        BUTTON_Y = \"joystick_button_y\"\n"\
"        BUTTON_START = \"joystick_start\"\n"\
"        BUTTON_BACK = \"joystick_back\"\n"\
"        BUTTON_DPAD_LEFT = \"joystick_dpad_left\"\n"\
"        BUTTON_DPAD_RIGHT = \"joystick_dpad_right\"\n"\
"        BUTTON_DPAD_UP = \"joystick_dpad_up\"\n"\
"        BUTTON_DPAD_DOWN = \"joystick_dpad_down\"\n"\
"        BUTTON_LEFT_SHOULDER = \"joystick_left_shoulder\"\n"\
"        BUTTON_RIGHT_SHOULDER = \"joystick_right_shoulder\"\n"\
"        BUTTON_LEFT_ANALOG = \"joystick_left_analog\"\n"\
"        BUTTON_RIGHT_ANALOG = \"joystick_right_analog\"\n"\
"\n"\
"        AXIS_LEFT_TRIGGER = \"joystick_left_trigger\"\n"\
"        AXIS_RIGHT_TRIGGER = \"joystick_right_trigger\"\n"\
"        AXIS_LEFT_ANALOG_LEFT = \"joystick_left_analog_left\"\n"\
"        AXIS_LEFT_ANALOG_RIGHT = \"joystick_left_analog_right\"\n"\
"        AXIS_LEFT_ANALOG_UP = \"joystick_left_analog_up\"\n"\
"        AXIS_LEFT_ANALOG_DOWN = \"joystick_left_analog_down\"\n"\
"        AXIS_RIGHT_ANALOG_LEFT = \"joystick_right_analog_left\"\n"\
"        AXIS_RIGHT_ANALOG_RIGHT = \"joystick_right_analog_right\"\n"\
"        AXIS_RIGHT_ANALOG_UP = \"joystick_right_analog_up\"\n"\
"        AXIS_RIGHT_ANALOG_DOWN = \"joystick_right_analog_down\"\n"\
"\n"\
"\n"\
"# Game Properties\n"\
"class GameProperties:\n"\
"    _instance = None\n"\
"\n"\
"    def __new__(cls, *args, **kwargs):\n"\
"        if not cls._instance:\n"\
"            cls._instance = object.__new__(cls)\n"\
"            (\n"\
"                game_title,\n"\
"                res_w,\n"\
"                res_h,\n"\
"                window_w,\n"\
"                window_h,\n"\
"                target_fps,\n"\
"                initial_scene_path,\n"\
"                are_colliders_visible,\n"\
"            ) = crescent_api_internal.game_properties_get()\n"\
"            cls._instance.game_title = game_title\n"\
"            cls._instance.game_resolution = Size2D(res_w, res_h)\n"\
"            cls._instance.default_window_size = Size2D(window_w, window_h)\n"\
"            cls._instance.target_fps = target_fps\n"\
"            cls._instance.initial_scene_path = initial_scene_path\n"\
"            cls._instance.are_colliders_visible = are_colliders_visible != 0\n"\
"        return cls._instance\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"GameProperties(game_title: {self.game_title}, game_resolution: {self.game_resolution}), default_window_size: {self.default_window_size}, target_fps: {self.target_fps}, initial_scene_path: {self.initial_scene_path}, are_colliders_visible: {self.are_colliders_visible})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"GameProperties(game_title: {self.game_title}, game_resolution: {self.game_resolution}), default_window_size: {self.default_window_size}, target_fps: {self.target_fps}, initial_scene_path: {self.initial_scene_path}, are_colliders_visible: {self.are_colliders_visible})\"\n"\
"\n"\
"\n"\
"# STAGE SETUP\n"\
"class StageNode:\n"\
"    def __init__(\n"\
"        self, name: str, type: str, tags, external_node_source, components, children\n"\
"    ):\n"\
"        self.name = name\n"\
"        self.type = type\n"\
"        self.tags = tags\n"\
"        self.external_node_source = external_node_source\n"\
"        self.components = components\n"\
"        self.children = children\n"\
"\n"\
"\n"\
"# NODE\n"\
"class NodeType(str, Enum):\n"\
"    NODE = \"Node\"\n"\
"    TIMER = \"Timer\"\n"\
"    NODE2D = \"Node2D\"\n"\
"    SPRITE = \"Sprite\"\n"\
"    ANIMATED_SPRITE = \"AnimatedSprite\"\n"\
"    TEXT_LABEL = \"TextLabel\"\n"\
"    COLLIDER2D = \"Collider2D\"\n"\
"    COLOR_RECT = \"ColorRect\"\n"\
"    PARALLAX = \"Parallax\"\n"\
"\n"\
"\n"\
"class Node:\n"\
"    def __init__(self, entity_id: int) -> None:\n"\
"        self.entity_id = entity_id\n"\
"\n"\
"    def __eq__(self, other) -> bool:\n"\
"        if self.entity_id == other.entity_id:\n"\
"            return True\n"\
"        else:\n"\
"            return False\n"\
"\n"\
"    def __str__(self):\n"\
"        return f\"Node(entity_id: {self.entity_id}, type: {type(self).__name__})\"\n"\
"\n"\
"    def __repr__(self):\n"\
"        return f\"Node(entity_id: {self.entity_id}, type: {type(self).__name__})\"\n"\
"\n"\
"    # New API\n"\
"    def get_child(self, name: str) -> \"Node\":\n"\
"        node = crescent_api_internal.node_get_child(\n"\
"            entity_id=self.entity_id, child_name=name\n"\
"        )\n"\
"        return self.parse_scene_node_from_engine(scene_node=node)\n"\
"\n"\
"    def get_children(self) -> List[\"Node\"]:\n"\
"        children_nodes = []\n"\
"        children = crescent_api_internal.node_get_children(entity_id=self.entity_id)\n"\
"        for child_node in children:\n"\
"            children_nodes.append(\n"\
"                self.parse_scene_node_from_engine(scene_node=child_node)\n"\
"            )\n"\
"        return children_nodes\n"\
"\n"\
"    def get_parent(self) -> \"Node\":\n"\
"        parent_node = crescent_api_internal.node_get_parent(entity_id=self.entity_id)\n"\
"        return self.parse_scene_node_from_engine(scene_node=parent_node)\n"\
"\n"\
"    # Old API\n"\
"\n"\
"    @classmethod\n"\
"    def extract_valid_inheritance_node(cls) -> str:\n"\
"        valid_node_type_list = [e.value for e in NodeType]\n"\
"        for class_path in cls.__mro__:\n"\
"            if class_path.__name__ in valid_node_type_list:\n"\
"                return class_path.__name__\n"\
"        return \"\"\n"\
"\n"\
"    @classmethod\n"\
"    def new(cls) -> \"Node\":\n"\
"        return crescent_api_internal.node_new(\n"\
"            class_path=f\"{cls.__module__}\",\n"\
"            class_name=f\"{cls.__name__}\",\n"\
"            node_type=f\"{cls.extract_valid_inheritance_node()}\",\n"\
"        )\n"\
"\n"\
"    @staticmethod\n"\
"    def parse_scene_node_from_engine(scene_node):\n"\
"        if not isinstance(scene_node, tuple):\n"\
"            return scene_node\n"\
"        elif scene_node:\n"\
"            entity_id = scene_node[0]\n"\
"            node_type = scene_node[1]\n"\
"            node_class = globals()[node_type]\n"\
"            instance = node_class(entity_id=entity_id)\n"\
"            return instance\n"\
"        # Is not a valid node, return 'None'\n"\
"        return None\n"\
"\n"\
"    def add_child(self, child_node) -> None:\n"\
"        crescent_api_internal.node_add_child(\n"\
"            parent_entity_id=self.entity_id, child_entity_id=child_node.entity_id\n"\
"        )\n"\
"\n"\
"    def queue_deletion(self) -> None:\n"\
"        crescent_api_internal.node_queue_deletion(entity_id=self.entity_id)\n"\
"\n"\
"    def create_event(self, event_id: str) -> None:\n"\
"        crescent_api_internal.node_create_event(\n"\
"            entity_id=self.entity_id, event_id=event_id\n"\
"        )\n"\
"\n"\
"    def subscribe_to_event(\n"\
"        self, event_id: str, scoped_node, callback_func: Callable\n"\
"    ) -> None:\n"\
"        crescent_api_internal.node_subscribe_to_event(\n"\
"            entity_id=self.entity_id,\n"\
"            event_id=event_id,\n"\
"            scoped_entity_id=scoped_node.entity_id,\n"\
"            callback_func=callback_func,\n"\
"        )\n"\
"\n"\
"    def broadcast_event(self, event_id: str, args=None) -> None:\n"\
"        crescent_api_internal.node_broadcast_event(\n"\
"            entity_id=self.entity_id, event_id=event_id, args=args\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def name(self) -> str:\n"\
"        return crescent_api_internal.node_get_name(entity_id=self.entity_id)\n"\
"\n"\
"    @property\n"\
"    def time_dilation(self) -> float:\n"\
"        return crescent_api_internal.node_get_time_dilation(entity_id=self.entity_id)\n"\
"\n"\
"    @time_dilation.setter\n"\
"    def time_dilation(self, value: float) -> None:\n"\
"        crescent_api_internal.node_set_time_dilation(\n"\
"            entity_id=self.entity_id, time_dilation=value\n"\
"        )\n"\
"\n"\
"    def get_full_time_dilation(self) -> float:\n"\
"        return crescent_api_internal.node_get_full_time_dilation(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"\n"\
"\n"\
"# 2D\n"\
"class Node2D(Node):\n"\
"    def set_position(self, value: Vector2) -> None:\n"\
"        crescent_api_internal.node2D_set_position(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def add_to_position(self, value: Vector2) -> None:\n"\
"        crescent_api_internal.node2D_add_to_position(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def get_position(self) -> Vector2:\n"\
"        px, py = crescent_api_internal.node2D_get_position(entity_id=self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @property\n"\
"    def position(self) -> Vector2:\n"\
"        px, py = crescent_api_internal.node2D_get_position(entity_id=self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @position.setter\n"\
"    def position(self, value: Vector2) -> None:\n"\
"        crescent_api_internal.node2D_set_position(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def get_global_position(self) -> Vector2:\n"\
"        px, py = crescent_api_internal.node2D_get_global_position(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @property\n"\
"    def global_position(self) -> Vector2:\n"\
"        px, py = crescent_api_internal.node2D_get_global_position(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    def set_scale(self, value: Vector2) -> None:\n"\
"        crescent_api_internal.node2D_set_scale(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def add_to_scale(self, value: Vector2) -> None:\n"\
"        crescent_api_internal.node2D_add_to_scale(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def get_scale(self) -> Vector2:\n"\
"        sx, sy = crescent_api_internal.node2D_get_scale(entity_id=self.entity_id)\n"\
"        return Vector2(sx, sy)\n"\
"\n"\
"    @property\n"\
"    def scale(self) -> Vector2:\n"\
"        sx, sy = crescent_api_internal.node2D_get_scale(entity_id=self.entity_id)\n"\
"        return Vector2(sx, sy)\n"\
"\n"\
"    @scale.setter\n"\
"    def scale(self, value: Vector2) -> None:\n"\
"        crescent_api_internal.node2D_set_scale(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def set_rotation(self, value: float) -> None:\n"\
"        crescent_api_internal.node2D_set_rotation(\n"\
"            entity_id=self.entity_id, rotation=value\n"\
"        )\n"\
"\n"\
"    def add_to_rotation(self, value: float) -> None:\n"\
"        crescent_api_internal.node2D_add_to_rotation(\n"\
"            entity_id=self.entity_id, rotation=value\n"\
"        )\n"\
"\n"\
"    def get_rotation(self) -> float:\n"\
"        return crescent_api_internal.node2D_get_rotation(entity_id=self.entity_id)\n"\
"\n"\
"    @property\n"\
"    def rotation(self) -> float:\n"\
"        return crescent_api_internal.node2D_get_rotation(entity_id=self.entity_id)\n"\
"\n"\
"    @rotation.setter\n"\
"    def rotation(self, value: float) -> None:\n"\
"        crescent_api_internal.node2D_set_rotation(\n"\
"            entity_id=self.entity_id, rotation=value\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def z_index(self) -> int:\n"\
"        return crescent_api_internal.node2D_get_z_index(entity_id=self.entity_id)\n"\
"\n"\
"    @z_index.setter\n"\
"    def z_index(self, value: int) -> None:\n"\
"        crescent_api_internal.node2D_set_z_index(\n"\
"            entity_id=self.entity_id, z_index=value\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def ignore_camera(self) -> bool:\n"\
"        return crescent_api_internal.node2D_get_ignore_camera(entity_id=self.entity_id)\n"\
"\n"\
"    @ignore_camera.setter\n"\
"    def ignore_camera(self, value: bool) -> None:\n"\
"        crescent_api_internal.node2D_set_ignore_camera(\n"\
"            entity_id=self.entity_id, ignore_camera=value\n"\
"        )\n"\
"\n"\
"\n"\
"class Sprite(Node2D):\n"\
"    @property\n"\
"    def flip_h(self) -> bool:\n"\
"        return crescent_api_internal.sprite_get_flip_h(entity_id=self.entity_id)\n"\
"\n"\
"    @flip_h.setter\n"\
"    def flip_h(self, value: bool) -> None:\n"\
"        crescent_api_internal.sprite_set_flip_h(entity_id=self.entity_id, flip_h=value)\n"\
"\n"\
"    @property\n"\
"    def flip_v(self) -> bool:\n"\
"        return crescent_api_internal.sprite_get_flip_v(entity_id=self.entity_id)\n"\
"\n"\
"    @flip_v.setter\n"\
"    def flip_v(self, value: bool) -> None:\n"\
"        crescent_api_internal.sprite_set_flip_v(entity_id=self.entity_id, flip_h=value)\n"\
"\n"\
"    @property\n"\
"    def modulate(self) -> Color:\n"\
"        red, green, blue, alpha = crescent_api_internal.sprite_get_modulate(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r=red, g=green, b=blue, a=alpha)\n"\
"\n"\
"    @modulate.setter\n"\
"    def modulate(self, color: Color) -> None:\n"\
"        crescent_api_internal.sprite_set_modulate(\n"\
"            entity_id=self.entity_id,\n"\
"            red=color.r,\n"\
"            green=color.g,\n"\
"            blue=color.b,\n"\
"            alpha=color.a,\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def texture(self) -> Texture:\n"\
"        (\n"\
"            file_path,\n"\
"            wrap_s,\n"\
"            wrap_t,\n"\
"            filter_min,\n"\
"            filter_mag,\n"\
"        ) = crescent_api_internal.sprite_get_texture(entity_id=self.entity_id)\n"\
"        return Texture(\n"\
"            file_path=file_path,\n"\
"            wrap_s=wrap_s,\n"\
"            wrap_t=wrap_t,\n"\
"            filter_min=filter_min,\n"\
"            filter_mag=filter_mag,\n"\
"        )\n"\
"\n"\
"    @texture.setter\n"\
"    def texture(self, value: Texture) -> None:\n"\
"        crescent_api_internal.sprite_set_texture(\n"\
"            entity_id=self.entity_id, file_path=value.file_path\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def draw_source(self) -> Rect2:\n"\
"        (\n"\
"            x,\n"\
"            y,\n"\
"            w,\n"\
"            h,\n"\
"        ) = crescent_api_internal.sprite_get_draw_source(entity_id=self.entity_id)\n"\
"        return Rect2(x=x, y=y, w=w, h=h)\n"\
"\n"\
"    @draw_source.setter\n"\
"    def draw_source(self, value: Rect2) -> None:\n"\
"        crescent_api_internal.sprite_set_draw_source(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y, w=value.w, h=value.h\n"\
"        )\n"\
"\n"\
"\n"\
"class AnimatedSprite(Node2D):\n"\
"    def play(self, name: str) -> bool:\n"\
"        return crescent_api_internal.animated_sprite_play(\n"\
"            entity_id=self.entity_id, name=name\n"\
"        )\n"\
"\n"\
"    def stop(self) -> None:\n"\
"        crescent_api_internal.animated_sprite_stop(entity_id=self.entity_id)\n"\
"\n"\
"    def add_animation(self, animation: Animation) -> None:\n"\
"        anim_frames = []\n"\
"        for frame in animation.frames:\n"\
"            anim_frames.append(\n"\
"                (\n"\
"                    frame.frame,\n"\
"                    frame.texture_path,\n"\
"                    frame.draw_source.x,\n"\
"                    frame.draw_source.y,\n"\
"                    frame.draw_source.w,\n"\
"                    frame.draw_source.h,\n"\
"                )\n"\
"            )\n"\
"        crescent_api_internal.animated_sprite_add_animation(\n"\
"            entity_id=self.entity_id,\n"\
"            name=animation.name,\n"\
"            speed=animation.speed,\n"\
"            loops=animation.loops,\n"\
"            frames=anim_frames,\n"\
"        )\n"\
"\n"\
"\n"\
"class TextLabel(Node2D):\n"\
"    @property\n"\
"    def text(self) -> str:\n"\
"        return crescent_api_internal.text_label_get_text(entity_id=self.entity_id)\n"\
"\n"\
"    @text.setter\n"\
"    def text(self, value: str) -> None:\n"\
"        crescent_api_internal.text_label_set_text(entity_id=self.entity_id, text=value)\n"\
"\n"\
"    def get_text(self) -> str:\n"\
"        return crescent_api_internal.text_label_get_text(entity_id=self.entity_id)\n"\
"\n"\
"    def set_text(self, text: str) -> None:\n"\
"        crescent_api_internal.text_label_set_text(entity_id=self.entity_id, text=text)\n"\
"\n"\
"    @property\n"\
"    def color(self) -> Color:\n"\
"        r, g, b, a = crescent_api_internal.text_label_get_color(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    @color.setter\n"\
"    def color(self, value: Color) -> None:\n"\
"        crescent_api_internal.text_label_set_color(\n"\
"            entity_id=self.entity_id, r=value.r, g=value.g, b=value.b, a=value.a\n"\
"        )\n"\
"\n"\
"    def get_color(self) -> Color:\n"\
"        r, g, b, a = crescent_api_internal.text_label_get_color(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    def set_color(self, color: Color) -> None:\n"\
"        crescent_api_internal.text_label_set_color(\n"\
"            entity_id=self.entity_id, r=color.r, g=color.g, b=color.b, a=color.a\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def font_uid(self) -> str:\n"\
"        return crescent_api_internal.text_label_get_font_uid(entity_id=self.entity_id)\n"\
"\n"\
"    @font_uid.setter\n"\
"    def font_uid(self, value: str) -> None:\n"\
"        crescent_api_internal.text_label_set_font_uid(\n"\
"            entity_id=self.entity_id, uid=value\n"\
"        )\n"\
"\n"\
"\n"\
"class Collider2D(Node2D):\n"\
"    def get_extents(self) -> Size2D:\n"\
"        w, h = crescent_api_internal.collider2D_get_extents(entity_id=self.entity_id)\n"\
"        return Size2D(w=w, h=h)\n"\
"\n"\
"    def set_extents(self, extents: Size2D) -> None:\n"\
"        crescent_api_internal.collider2D_set_extents(\n"\
"            entity_id=self.entity_id, w=extents.w, h=extents.h\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def extents(self) -> Size2D:\n"\
"        w, h = crescent_api_internal.collider2D_get_extents(entity_id=self.entity_id)\n"\
"        return Size2D(w=w, h=h)\n"\
"\n"\
"    @extents.setter\n"\
"    def extents(self, value: Size2D) -> None:\n"\
"        crescent_api_internal.collider2D_set_extents(\n"\
"            entity_id=self.entity_id, w=value.w, h=value.h\n"\
"        )\n"\
"\n"\
"    def get_color(self) -> Color:\n"\
"        r, g, b, a = crescent_api_internal.collider2D_get_color(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r=r, g=g, b=b, a=a)\n"\
"\n"\
"    def set_color(self, color: Color) -> None:\n"\
"        crescent_api_internal.collider2D_set_color(\n"\
"            entity_id=self.entity_id, r=color.r, g=color.g, b=color.b, a=color.a\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def color(self) -> Color:\n"\
"        r, g, b, a = crescent_api_internal.collider2D_get_color(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r=r, g=g, b=b, a=a)\n"\
"\n"\
"    @color.setter\n"\
"    def color(self, value: Color) -> None:\n"\
"        crescent_api_internal.collider2D_set_color(\n"\
"            entity_id=self.entity_id, r=value.r, g=value.g, b=value.b, a=value.a\n"\
"        )\n"\
"\n"\
"\n"\
"class ColorRect(Node2D):\n"\
"    def get_size(self) -> Size2D:\n"\
"        w, h = crescent_api_internal.color_rect_get_size(entity_id=self.entity_id)\n"\
"        return Size2D(w=w, h=h)\n"\
"\n"\
"    def set_size(self, extents: Size2D) -> None:\n"\
"        crescent_api_internal.color_rect_set_size(\n"\
"            entity_id=self.entity_id, w=extents.w, h=extents.h\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def size(self) -> Size2D:\n"\
"        w, h = crescent_api_internal.color_rect_get_size(entity_id=self.entity_id)\n"\
"        return Size2D(w=w, h=h)\n"\
"\n"\
"    @size.setter\n"\
"    def size(self, value: Size2D) -> None:\n"\
"        crescent_api_internal.color_rect_set_size(\n"\
"            entity_id=self.entity_id, w=value.w, h=value.h\n"\
"        )\n"\
"\n"\
"    def get_color(self) -> Color:\n"\
"        r, g, b, a = crescent_api_internal.color_rect_get_color(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r=r, g=g, b=b, a=a)\n"\
"\n"\
"    def set_color(self, color: Color) -> None:\n"\
"        crescent_api_internal.color_rect_set_color(\n"\
"            entity_id=self.entity_id, r=color.r, g=color.g, b=color.b, a=color.a\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def color(self) -> Color:\n"\
"        r, g, b, a = crescent_api_internal.color_rect_get_color(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r=r, g=g, b=b, a=a)\n"\
"\n"\
"    @color.setter\n"\
"    def color(self, value: Color) -> None:\n"\
"        crescent_api_internal.color_rect_set_color(\n"\
"            entity_id=self.entity_id, r=value.r, g=value.g, b=value.b, a=value.a\n"\
"        )\n"\
"\n"\
"\n"\
"class Parallax(Node2D):\n"\
"    @property\n"\
"    def scroll_speed(self) -> Vector2:\n"\
"        return Vector2.ZERO()\n"\
"\n"\
"    @scroll_speed.setter\n"\
"    def scroll_speed(self, value: Vector2) -> None:\n"\
"        pass\n"\
"\n"\
"\n"\
"# SCENE TREE\n"\
"class SceneTree:\n"\
"    @staticmethod\n"\
"    def change_scene(path: str) -> None:\n"\
"        crescent_api_internal.scene_tree_change_scene(path=path)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_root() -> Node:\n"\
"        node = crescent_api_internal.scene_tree_get_root()\n"\
"        return Node.parse_scene_node_from_engine(scene_node=node)\n"\
"\n"\
"\n"\
"# WORLD\n"\
"class World:\n"\
"    @staticmethod\n"\
"    def set_time_dilation(value: float) -> None:\n"\
"        crescent_api_internal.world_set_time_dilation(time_dilation=value)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_time_dilation() -> float:\n"\
"        return crescent_api_internal.world_get_time_dilation()\n"\
"\n"\
"\n"\
"# AUDIO MANAGER\n"\
"class AudioManager:\n"\
"    @staticmethod\n"\
"    def play_sound(path: str, loops=False):\n"\
"        crescent_api_internal.audio_manager_play_sound(path=path, loops=loops)\n"\
"\n"\
"    @staticmethod\n"\
"    def stop_sound(path: str):\n"\
"        crescent_api_internal.audio_manager_stop_sound(path=path)\n"\
"\n"\
"\n"\
"# PHYSICS\n"\
"class CollisionHandler:\n"\
"    # A temp (ish) class used to brute force collision checks for now\n"\
"\n"\
"    @staticmethod\n"\
"    def process_collisions(collider: Collider2D):\n"\
"        collided_entities = crescent_api_internal.collision_handler_process_collisions(\n"\
"            entity_id=collider.entity_id\n"\
"        )\n"\
"        for index, node in enumerate(collided_entities):\n"\
"            yield Node.parse_scene_node_from_engine(scene_node=node)\n"\
"\n"\
"    @staticmethod\n"\
"    def process_mouse_collisions(\n"\
"        pos_offset=Vector2(0.0, 0.0), collision_size=Size2D(1.0, 1.0)\n"\
"    ):\n"\
"        collided_entities = (\n"\
"            crescent_api_internal.collision_handler_process_mouse_collisions(\n"\
"                pos_offset_x=pos_offset.x,\n"\
"                pos_offset_y=pos_offset.y,\n"\
"                collision_size_w=collision_size.w,\n"\
"                collision_size_h=collision_size.h,\n"\
"            )\n"\
"        )\n"\
"        for index, node in enumerate(collided_entities):\n"\
"            yield Node.parse_scene_node_from_engine(scene_node=node)\n"\
"\n"\
"\n"\
"# CAMERA\n"\
"class Camera2D:\n"\
"    @staticmethod\n"\
"    def set_position(position: Vector2) -> None:\n"\
"        crescent_api_internal.camera2D_set_position(x=position.x, y=position.y)\n"\
"\n"\
"    @staticmethod\n"\
"    def add_to_position(position: Vector2) -> None:\n"\
"        crescent_api_internal.camera2D_add_to_position(x=position.x, y=position.y)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_position() -> Vector2:\n"\
"        x, y = crescent_api_internal.camera2D_get_position()\n"\
"\n"\
"    @staticmethod\n"\
"    def set_offset(offset: Vector2) -> None:\n"\
"        crescent_api_internal.camera2D_set_offset(x=offset.x, y=offset.y)\n"\
"\n"\
"    @staticmethod\n"\
"    def add_to_offset(offset: Vector2) -> None:\n"\
"        crescent_api_internal.camera2D_add_to_offset(x=offset.x, y=offset.y)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_offset() -> Vector2:\n"\
"        x, y = crescent_api_internal.camera2D_get_offset()\n"\
"\n"\
"    @staticmethod\n"\
"    def set_zoom(zoom: Vector2) -> None:\n"\
"        crescent_api_internal.camera2D_set_zoom(x=zoom.x, y=zoom.y)\n"\
"\n"\
"    @staticmethod\n"\
"    def add_to_zoom(zoom: Vector2) -> None:\n"\
"        crescent_api_internal.camera2D_add_to_zoom(x=zoom.x, y=zoom.y)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_zoom() -> Vector2:\n"\
"        x, y = crescent_api_internal.camera2D_get_zoom()\n"\
"\n"\
"    @staticmethod\n"\
"    def set_boundary(boundary: Rect2) -> None:\n"\
"        crescent_api_internal.camera2D_set_boundary(\n"\
"            x=boundary.x, y=boundary.y, w=boundary.w, h=boundary.h\n"\
"        )\n"\
"\n"\
"    @staticmethod\n"\
"    def get_boundary() -> Rect2:\n"\
"        x, y, w, h = crescent_api_internal.camera2D_get_boundary()\n"\
"        return Rect2(x=x, y=y, w=w, h=h)\n"\
"\n"\
"    @staticmethod\n"\
"    def follow_node(node: Node2D) -> None:\n"\
"        crescent_api_internal.camera2D_follow_node(entity_id=node.entity_id)\n"\
"\n"\
"    @staticmethod\n"\
"    def unfollow_node(node: Node2D) -> None:\n"\
"        crescent_api_internal.camera2D_unfollow_node(entity_id=node.entity_id)\n"\
"\n"\
"\n"\
"# NETWORK\n"\
"class Network:\n"\
"    @staticmethod\n"\
"    def is_server() -> bool:\n"\
"        return crescent_api_internal.network_is_server()\n"\
"\n"\
"\n"\
"class Server:\n"\
"    @staticmethod\n"\
"    def start(port: int) -> None:\n"\
"        crescent_api_internal.server_start(port=port)\n"\
"\n"\
"    @staticmethod\n"\
"    def stop() -> None:\n"\
"        crescent_api_internal.server_stop()\n"\
"\n"\
"    @staticmethod\n"\
"    def send(message: str) -> None:\n"\
"        crescent_api_internal.server_send(message=message)\n"\
"\n"\
"    @staticmethod\n"\
"    def subscribe(signal_id: str, listener_node: Node, listener_func) -> None:\n"\
"        crescent_api_internal.server_subscribe(\n"\
"            signal_id=signal_id,\n"\
"            listener_node=listener_node.entity_id,\n"\
"            listener_func=listener_func,\n"\
"        )\n"\
"\n"\
"\n"\
"class Client:\n"\
"    @staticmethod\n"\
"    def start(host: str, port: int) -> None:\n"\
"        crescent_api_internal.client_start(host=host, port=port)\n"\
"\n"\
"    @staticmethod\n"\
"    def stop() -> None:\n"\
"        crescent_api_internal.client_stop()\n"\
"\n"\
"    @staticmethod\n"\
"    def send(message: str) -> None:\n"\
"        crescent_api_internal.client_send(message=message)\n"\
"\n"\
"    @staticmethod\n"\
"    def subscribe(signal_id: str, listener_node: Node, listener_func) -> None:\n"\
"        crescent_api_internal.client_subscribe(\n"\
"            signal_id=signal_id,\n"\
"            listener_node=listener_node.entity_id,\n"\
"            listener_func=listener_func,\n"\
"        )\n"\
"\n"\
"\n"\
"class GameConfig:\n"\
"    @staticmethod\n"\
"    def save(path: str, data: dict, encryption_key=\"\") -> bool:\n"\
"        try:\n"\
"            json_text = json.dumps(data, indent=4)\n"\
"        except JSONDecodeError as e:\n"\
"            print(f\"Game Config Save Json Decode Error: {e}\")\n"\
"            return False\n"\
"        return crescent_api_internal.game_config_save(\n"\
"            path=path, data_json=json_text, encryption_key=encryption_key\n"\
"        )\n"\
"\n"\
"    @staticmethod\n"\
"    def load(path: str, encryption_key=\"\") -> dict:\n"\
"        try:\n"\
"            json_dict = json.loads(\n"\
"                crescent_api_internal.game_config_load(\n"\
"                    path=path, encryption_key=encryption_key\n"\
"                )\n"\
"            )\n"\
"        except JSONDecodeError as e:\n"\
"            print(f\"Game Config Load Json Decode Error: {e}\")\n"\
"            return {}\n"\
"        return json_dict\n"\
"\n"
