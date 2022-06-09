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
"import rbe_py_api_internal\n"\
"\n"\
"import math\n"\
"\n"\
"# COLOR\n"\
"class Color:\n"\
"    def __init__(self, r=0.0, g=0.0, b=0.0, a=1.0):\n"\
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
"    def normalize(r: int, g: int, b: int, a=255):\n"\
"        return Color(r / 255.0, g / 255.0, b / 255.0, a / 255.0)\n"\
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
"    def __init__(self, name: str, values: list):\n"\
"        self.name = name\n"\
"        self.values = values\n"\
"\n"\
"\n"\
"# INPUT\n"\
"class Input:\n"\
"    @staticmethod\n"\
"    def add_action(action_name: str, value: str) -> None:\n"\
"        rbe_py_api_internal.input_add_action(action_name=action_name, value=value)\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_pressed(action_name: str) -> bool:\n"\
"        return rbe_py_api_internal.input_is_action_pressed(action_name=action_name)\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_just_pressed(action_name: str) -> bool:\n"\
"        return rbe_py_api_internal.input_is_action_just_pressed(action_name=action_name)\n"\
"\n"\
"    @staticmethod\n"\
"    def is_action_just_released(action_name: str) -> bool:\n"\
"        return rbe_py_api_internal.input_is_action_just_released(\n"\
"            action_name=action_name\n"\
"        )\n"\
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
"class ScriptComponent:\n"\
"    def __init__(self, class_path: str, class_name: str):\n"\
"        self.class_path = class_path\n"\
"        self.class_name = class_name\n"\
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
