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
"class SourceImporter:\n"\
"   def __init__(self, modules: dict):\n"\
"       self._modules = modules\n"\
"\n"\
"       for module_name in self._modules:\n"\
"           module_src = self._modules[module_name]\n"\
"           self.import_from_source(module_name, module_src)\n"\
"\n"\
"   def import_from_source(self, module_name: str, source_code: str):\n"\
"       spec = importlib.util.spec_from_loader(module_name, loader=None, is_package=False)\n"\
"       module = importlib.util.module_from_spec(spec)\n"\
"       exec(source_code, module.__dict__)\n"\
"       sys.modules[module_name] = module\n"\
"       return module\n"\
"\"\"\"\n"\
"\n"\
"# Load source importer module\n"\
"spec = importlib.util.spec_from_loader(module_name, loader=None, is_package=False)\n"\
"module = importlib.util.module_from_spec(spec)\n"\
"exec(source_code, module.__dict__)\n"\
"sys.modules[module_name] = module\n"\
"\n"

#define RBE_PY_API_SOURCE ""\
"from enum import Enum\n"\
"from typing import Optional\n"\
"\n"\
"import rbe_py_api_internal\n"\
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
"    def normalized(self):\n"\
"        mag = self.magnitude()\n"\
"        self.x = self.x / mag\n"\
"        self.y = self.y / mag\n"\
"        return self\n"\
"\n"\
"    def direction_to(self, target):\n"\
"        return (target - self).normalized()\n"\
"\n"\
"    @staticmethod\n"\
"    def lerp(source, destination, amount: float) -> float:\n"\
"        return source + (destination - source) * Vector2(amount, amount)\n"\
"\n"\
"    @staticmethod\n"\
"    def ZERO():\n"\
"        return Vector2(0.0, 0.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def LEFT():\n"\
"        return Vector2(-1.0, 0.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def RIGHT():\n"\
"        return Vector2(1.0, 0.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def UP():\n"\
"        return Vector2(0.0, -1.0)\n"\
"\n"\
"    @staticmethod\n"\
"    def DOWN():\n"\
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
"\n"\
"# ASSETS\n"\
"class AudioSource:\n"\
"    def __init__(self, file_path: str):\n"\
"        self.file_path = file_path\n"\
"\n"\
"\n"\
"class Texture:\n"\
"    def __init__(\n"\
"        self, file_path: str, wrap_s: str, wrap_t: str, filter_min: str, filter_mag: str\n"\
"    ):\n"\
"        self.file_path = file_path\n"\
"        self.wrap_s = wrap_s\n"\
"        self.wrap_t = wrap_t\n"\
"        self.filter_min = filter_min\n"\
"        self.filter_mag = filter_mag\n"\
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
"# ENGINE\n"\
"class Engine:\n"\
"    @staticmethod\n"\
"    def exit(code=0) -> None:\n"\
"        rbe_py_api_internal.engine_exit(code=code)\n"\
"\n"\
"    @staticmethod\n"\
"    def set_target_fps(fps: int) -> None:\n"\
"        rbe_py_api_internal.engine_set_target_fps(fps=fps)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_target_fps() -> int:\n"\
"        return rbe_py_api_internal.engine_get_target_fps()\n"\
"\n"\
"    @staticmethod\n"\
"    def get_average_fps() -> float:\n"\
"        return rbe_py_api_internal.engine_get_average_fps()\n"\
"\n"\
"    @staticmethod\n"\
"    def set_fps_display_enabled(enabled: bool) -> None:\n"\
"        rbe_py_api_internal.engine_set_fps_display_enabled(enabled=enabled)\n"\
"\n"\
"\n"\
"# INPUT\n"\
"class Input:\n"\
"    @staticmethod\n"\
"    def add_action(name: str, value: str, device_id=0) -> None:\n"\
"        rbe_py_api_internal.input_add_action(\n"\
"            name=name, value=value, device_id=device_id\n"\
"        )\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_pressed(name: str) -> bool:\n"\
"        return rbe_py_api_internal.input_is_action_pressed(name=name)\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_just_pressed(name: str) -> bool:\n"\
"        return rbe_py_api_internal.input_is_action_just_pressed(name=name)\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_just_released(name: str) -> bool:\n"\
"        return rbe_py_api_internal.input_is_action_just_released(name=name)\n"\
"\n"\
"    class Mouse:\n"\
"        LEFT_BUTTON = \"mb_left\"\n"\
"        RIGHT_BUTTON = \"mb_right\"\n"\
"\n"\
"        @staticmethod\n"\
"        def get_position() -> Vector2:\n"\
"            mouse_x, mouse_y = rbe_py_api_internal.mouse_get_position()\n"\
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
"# ASSETS\n"\
"class AnimationFrame:\n"\
"    def __init__(self, frame: int, texture_path: str, draw_source: Rect2):\n"\
"        self.frame = frame\n"\
"        self.texture_path = texture_path\n"\
"        self.draw_source = draw_source\n"\
"\n"\
"\n"\
"class Animation:\n"\
"    def __init__(self, name: str, speed: int, loops: bool, frames: list):\n"\
"        self.name = name\n"\
"        self.speed = speed\n"\
"        self.loops = loops\n"\
"        self.frames = frames\n"\
"\n"\
"\n"\
"# COMPONENTS\n"\
"class Transform2DComponent:\n"\
"    def __init__(\n"\
"        self,\n"\
"        position: Vector2,\n"\
"        scale: Vector2,\n"\
"        rotation: float,\n"\
"        z_index: int,\n"\
"        z_index_relative_to_parent: bool,\n"\
"        ignore_camera: bool,\n"\
"    ):\n"\
"        self.position = position\n"\
"        self.scale = scale\n"\
"        self.rotation = rotation\n"\
"        self.z_index = z_index\n"\
"        self.z_index_relative_to_parent = z_index_relative_to_parent\n"\
"        self.ignore_camera = ignore_camera\n"\
"\n"\
"\n"\
"class SpriteComponent:\n"\
"    def __init__(\n"\
"        self,\n"\
"        texture_path: str,\n"\
"        draw_source: Rect2,\n"\
"        flip_x: bool,\n"\
"        flip_y: bool,\n"\
"        modulate: Color,\n"\
"    ):\n"\
"        self.texture_path = texture_path\n"\
"        self.draw_source = draw_source\n"\
"        self.flip_x = flip_x\n"\
"        self.flip_y = flip_y\n"\
"        self.modulate = modulate\n"\
"\n"\
"\n"\
"class AnimatedSpriteComponent:\n"\
"    def __init__(\n"\
"        self,\n"\
"        current_animation_name: str,\n"\
"        is_playing: bool,\n"\
"        animations: list,\n"\
"        flip_x=False,\n"\
"        flip_y=False,\n"\
"    ):\n"\
"        self.current_animation_name = current_animation_name\n"\
"        self.is_playing = is_playing\n"\
"        self.animations = animations\n"\
"        self.flip_x = flip_x\n"\
"        self.flip_y = flip_y\n"\
"\n"\
"\n"\
"class TextLabelComponent:\n"\
"    def __init__(\n"\
"        self,\n"\
"        uid: str,\n"\
"        text: str,\n"\
"        color: Color,\n"\
"    ):\n"\
"        self.uid = uid\n"\
"        self.text = text\n"\
"        self.color = color\n"\
"\n"\
"\n"\
"class ScriptComponent:\n"\
"    def __init__(self, class_path: str, class_name: str):\n"\
"        self.class_path = class_path\n"\
"        self.class_name = class_name\n"\
"\n"\
"\n"\
"class Collider2DComponent:\n"\
"    def __init__(self, rect: Rect2, color: Color):\n"\
"        self.rect = rect\n"\
"        self.color = color\n"\
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
"    def get_child(self, name: str):\n"\
"        node = rbe_py_api_internal.node_get_child(\n"\
"            entity_id=self.entity_id, child_name=name\n"\
"        )\n"\
"        return self.parse_scene_node_from_engine(scene_node=node)\n"\
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
"    def new(cls):\n"\
"        return rbe_py_api_internal.node_new(\n"\
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
"        print(\"Error with python api function 'parse_scene_node_from_engine'\")\n"\
"        return None\n"\
"\n"\
"    def add_child(self, child_node) -> None:\n"\
"        rbe_py_api_internal.node_add_child(\n"\
"            parent_entity_id=self.entity_id, child_entity_id=child_node.entity_id\n"\
"        )\n"\
"\n"\
"    def get_node(self, name: str):\n"\
"        node = rbe_py_api_internal.node_get_node(name=name)\n"\
"        if not node:\n"\
"            return None\n"\
"        return self.parse_scene_node_from_engine(scene_node=node)\n"\
"\n"\
"    def queue_deletion(self) -> None:\n"\
"        rbe_py_api_internal.node_queue_deletion(entity_id=self.entity_id)\n"\
"\n"\
"    def create_signal(self, signal_id: str) -> None:\n"\
"        rbe_py_api_internal.node_signal_create(\n"\
"            entity_id=self.entity_id, signal_id=signal_id\n"\
"        )\n"\
"\n"\
"    def connect_signal(self, signal_id: str, listener_node, function_name: str) -> None:\n"\
"        rbe_py_api_internal.node_signal_connect(\n"\
"            entity_id=self.entity_id,\n"\
"            signal_id=signal_id,\n"\
"            listener_entity_id=listener_node.entity_id,\n"\
"            function_name=function_name,\n"\
"        )\n"\
"\n"\
"    def emit_signal(self, signal_id: str, args=[]) -> None:\n"\
"        rbe_py_api_internal.node_signal_emit(\n"\
"            entity_id=self.entity_id, signal_id=signal_id, args=args\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def name(self) -> str:\n"\
"        return rbe_py_api_internal.node_get_name(entity_id=self.entity_id)\n"\
"\n"\
"    @property\n"\
"    def tags(self) -> list:\n"\
"        return rbe_py_api_internal.node_get_tags(entity_id=self.entity_id)\n"\
"\n"\
"    @tags.setter\n"\
"    def tags(self, value: list) -> None:\n"\
"        rbe_py_api_internal.node_set_tags(entity_id=self.entity_id, tags=value)\n"\
"\n"\
"    def show(self) -> None:\n"\
"        rbe_py_api_internal.node_show(entity_id=self.entity_id)\n"\
"\n"\
"    def hide(self) -> None:\n"\
"        rbe_py_api_internal.node_hide(entity_id=self.entity_id)\n"\
"\n"\
"    @property\n"\
"    def visibility(self) -> bool:\n"\
"        return rbe_py_api_internal.node_is_visible(entity_id=self.entity_id)\n"\
"\n"\
"    @visibility.setter\n"\
"    def visibility(self, value: bool) -> None:\n"\
"        if value:\n"\
"            self.show()\n"\
"        else:\n"\
"            self.hide()\n"\
"\n"\
"    def get_parent(self):\n"\
"        parent_node = rbe_py_api_internal.node_get_parent(entity_id=self.entity_id)\n"\
"        if parent_node:\n"\
"            return parent_node\n"\
"        return self.parse_scene_node_from_engine(scene_node=parent_node)\n"\
"\n"\
"    def get_children(self) -> list:\n"\
"        children_nodes = []\n"\
"        children = rbe_py_api_internal.node_get_children(entity_id=self.entity_id)\n"\
"        for child_node in children:\n"\
"            children_nodes.append(\n"\
"                self.parse_scene_node_from_engine(scene_node=child_node)\n"\
"            )\n"\
"        return children_nodes\n"\
"\n"\
"\n"\
"# 2D\n"\
"class Node2D(Node):\n"\
"    def set_position(self, value: Vector2) -> None:\n"\
"        rbe_py_api_internal.node2D_set_position(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def add_to_position(self, value: Vector2) -> None:\n"\
"        rbe_py_api_internal.node2D_add_to_position(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    def get_position(self) -> Vector2:\n"\
"        px, py = rbe_py_api_internal.node2D_get_position(entity_id=self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @property\n"\
"    def position(self) -> Vector2:\n"\
"        px, py = rbe_py_api_internal.node2D_get_position(entity_id=self.entity_id)\n"\
"        return Vector2(px, py)\n"\
"\n"\
"    @position.setter\n"\
"    def position(self, value: Vector2) -> None:\n"\
"        rbe_py_api_internal.node2D_set_position(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def rotation(self) -> float:\n"\
"        return rbe_py_api_internal.node2D_get_rotation(entity_id=self.entity_id)\n"\
"\n"\
"    @rotation.setter\n"\
"    def rotation(self, value: float) -> None:\n"\
"        rbe_py_api_internal.node2D_set_rotation(\n"\
"            entity_id=self.entity_id, rotation=value\n"\
"        )\n"\
"\n"\
"    @property\n"\
"    def z_index(self) -> int:\n"\
"        return rbe_py_api_internal.node2D_get_z_index(entity_id=self.entity_id)\n"\
"\n"\
"    @z_index.setter\n"\
"    def z_index(self, value: int) -> None:\n"\
"        rbe_py_api_internal.node2D_set_z_index(entity_id=self.entity_id, z_index=value)\n"\
"\n"\
"\n"\
"class Sprite(Node2D):\n"\
"    @property\n"\
"    def flip_h(self) -> bool:\n"\
"        return rbe_py_api_internal.sprite_get_flip_h(entity_id=self.entity_id)\n"\
"\n"\
"    @flip_h.setter\n"\
"    def flip_h(self, value: bool) -> None:\n"\
"        rbe_py_api_internal.sprite_set_flip_h(entity_id=self.entity_id, flip_h=value)\n"\
"\n"\
"    @property\n"\
"    def flip_v(self) -> bool:\n"\
"        return rbe_py_api_internal.sprite_get_flip_v(entity_id=self.entity_id)\n"\
"\n"\
"    @flip_v.setter\n"\
"    def flip_v(self, value: bool) -> None:\n"\
"        rbe_py_api_internal.sprite_set_flip_v(entity_id=self.entity_id, flip_h=value)\n"\
"\n"\
"    @property\n"\
"    def modulate(self) -> Color:\n"\
"        red, green, blue, alpha = rbe_py_api_internal.sprite_get_modulate(\n"\
"            entity_id=self.entity_id\n"\
"        )\n"\
"        return Color(r=red, g=green, b=blue, a=alpha)\n"\
"\n"\
"    @modulate.setter\n"\
"    def modulate(self, color: Color) -> None:\n"\
"        rbe_py_api_internal.sprite_set_modulate(\n"\
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
"        ) = rbe_py_api_internal.sprite_get_texture(entity_id=self.entity_id)\n"\
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
"        rbe_py_api_internal.sprite_set_texture(\n"\
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
"        ) = rbe_py_api_internal.sprite_get_draw_source(entity_id=self.entity_id)\n"\
"        return Rect2(x=x, y=y, w=w, h=h)\n"\
"\n"\
"    @draw_source.setter\n"\
"    def draw_source(self, value: Rect2) -> None:\n"\
"        rbe_py_api_internal.sprite_set_draw_source(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y, w=value.w, h=value.h\n"\
"        )\n"\
"\n"\
"\n"\
"class TextLabel(Node2D):\n"\
"    @property\n"\
"    def text(self) -> str:\n"\
"        return rbe_py_api_internal.text_label_get_text(entity_id=self.entity_id)\n"\
"\n"\
"    @text.setter\n"\
"    def text(self, value: str) -> None:\n"\
"        rbe_py_api_internal.text_label_set_text(entity_id=self.entity_id, text=value)\n"\
"\n"\
"    def get_text(self) -> str:\n"\
"        return rbe_py_api_internal.text_label_get_text(entity_id=self.entity_id)\n"\
"\n"\
"    def set_text(self, text: str) -> None:\n"\
"        rbe_py_api_internal.text_label_set_text(entity_id=self.entity_id, text=text)\n"\
"\n"\
"    @property\n"\
"    def color(self) -> Color:\n"\
"        r, g, b, a = rbe_py_api_internal.text_label_get_color(entity_id=self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    @color.setter\n"\
"    def color(self, value: Color) -> None:\n"\
"        rbe_py_api_internal.text_label_set_color(\n"\
"            entity_id=self.entity_id, r=value.r, g=value.g, b=value.b, a=value.a\n"\
"        )\n"\
"\n"\
"    def get_color(self) -> Color:\n"\
"        r, g, b, a = rbe_py_api_internal.text_label_get_color(entity_id=self.entity_id)\n"\
"        return Color(r, g, b, a)\n"\
"\n"\
"    def set_color(self, color: Color) -> None:\n"\
"        rbe_py_api_internal.text_label_set_color(\n"\
"            entity_id=self.entity_id, r=color.r, g=color.g, b=color.b, a=color.a\n"\
"        )\n"\
"\n"\
"\n"\
"class Collider2D(Node2D):\n"\
"    def get_rect(self) -> Rect2:\n"\
"        x, y, w, h = rbe_py_api_internal.collider2d_get_rect(entity_id=self.entity_id)\n"\
"        return Rect2(x=x, y=y, w=w, h=h)\n"\
"\n"\
"    def set_rect(self, value: Rect2) -> None:\n"\
"        rbe_py_api_internal.collider2d_set_rect(\n"\
"            entity_id=self.entity_id, x=value.x, y=value.y, w=value.w, h=value.h\n"\
"        )\n"\
"\n"\
"\n"\
"# SCENE TREE\n"\
"class SceneTree:\n"\
"    @staticmethod\n"\
"    def change_scene(path: str) -> None:\n"\
"        rbe_py_api_internal.scene_tree_change_scene(path=path)\n"\
"\n"\
"\n"\
"# AUDIO MANAGER\n"\
"class AudioManager:\n"\
"    @staticmethod\n"\
"    def play_sound(path: str, loops=False):\n"\
"        rbe_py_api_internal.audio_manager_play_sound(path=path, loops=loops)\n"\
"\n"\
"    @staticmethod\n"\
"    def stop_sound(path: str):\n"\
"        rbe_py_api_internal.audio_manager_stop_sound(path=path)\n"\
"\n"\
"\n"\
"# NETWORK\n"\
"class Network:\n"\
"    @staticmethod\n"\
"    def is_server() -> bool:\n"\
"        return rbe_py_api_internal.network_is_server()\n"\
"\n"\
"\n"\
"class Server:\n"\
"    @staticmethod\n"\
"    def start(port: int) -> None:\n"\
"        rbe_py_api_internal.server_start(port=port)\n"\
"\n"\
"    @staticmethod\n"\
"    def stop() -> None:\n"\
"        rbe_py_api_internal.server_stop()\n"\
"\n"\
"    @staticmethod\n"\
"    def send(message: str) -> None:\n"\
"        rbe_py_api_internal.server_send(message=message)\n"\
"\n"\
"    @staticmethod\n"\
"    def subscribe(signal_id: str, listener_node: Node, listener_func) -> None:\n"\
"        rbe_py_api_internal.server_subscribe(\n"\
"            signal_id=signal_id,\n"\
"            listener_node=listener_node.entity_id,\n"\
"            listener_func=listener_func,\n"\
"        )\n"\
"\n"\
"\n"\
"class Client:\n"\
"    @staticmethod\n"\
"    def start(host: str, port: int) -> None:\n"\
"        rbe_py_api_internal.client_start(host=host, port=port)\n"\
"\n"\
"    @staticmethod\n"\
"    def stop() -> None:\n"\
"        rbe_py_api_internal.client_stop()\n"\
"\n"\
"    @staticmethod\n"\
"    def send(message: str) -> None:\n"\
"        rbe_py_api_internal.client_send(message=message)\n"\
"\n"\
"    @staticmethod\n"\
"    def subscribe(signal_id: str, listener_node: Node, listener_func) -> None:\n"\
"        rbe_py_api_internal.client_subscribe(\n"\
"            signal_id=signal_id,\n"\
"            listener_node=listener_node.entity_id,\n"\
"            listener_func=listener_func,\n"\
"        )\n"\
"\n"\
"\n"\
"# CONFIGURATION\n"\
"def configure_game(\n"\
"    game_tile=\"Test Game\",\n"\
"    window_width=800,\n"\
"    window_height=600,\n"\
"    resolution_width=800,\n"\
"    resolution_height=600,\n"\
"    target_fps=66,\n"\
"    initial_node_path=\"\",\n"\
") -> None:\n"\
"    rbe_py_api_internal.configure_game(\n"\
"        game_tile=game_tile,\n"\
"        window_width=window_width,\n"\
"        window_height=window_height,\n"\
"        resolution_width=resolution_width,\n"\
"        resolution_height=resolution_height,\n"\
"        target_fps=target_fps,\n"\
"        initial_node_path=initial_node_path,\n"\
"    )\n"\
"\n"\
"\n"\
"def configure_assets(audio_sources=None, textures=None, fonts=None) -> None:\n"\
"    if fonts is None:\n"\
"        fonts = []\n"\
"    if textures is None:\n"\
"        textures = []\n"\
"    if audio_sources is None:\n"\
"        audio_sources = []\n"\
"    rbe_py_api_internal.configure_assets(\n"\
"        audio_sources=audio_sources, textures=textures, fonts=fonts\n"\
"    )\n"\
"\n"\
"\n"\
"def configure_inputs(input_actions=None) -> None:\n"\
"    if input_actions is None:\n"\
"        input_actions = []\n"\
"    rbe_py_api_internal.configure_inputs(input_actions=input_actions)\n"\
"\n"\
"\n"\
"# STAGE\n"\
"def create_stage_nodes(stage_nodes: list) -> None:\n"\
"    rbe_py_api_internal.create_stage_nodes(stage_nodes=stage_nodes)\n"\
"\n"

#define RBE_PY_API_SOURCE_IMPORTER_MODULE_IMPORTS ""\
"from source_importer import SourceImporter\n"\
"\n"\
"rbe_py_api_source_code = \"\"\"\n"\
RBE_PY_API_SOURCE\
"\"\"\"\n"\
"modules_to_import = {\n"\
"\"rbe_py_api\": rbe_py_api_source_code,\n"\
"}\n"\
"\n"\
"game_source_importer = SourceImporter(modules=modules_to_import)\n"\
"\n"
