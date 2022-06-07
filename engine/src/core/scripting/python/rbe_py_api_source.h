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
"\n"\
"class AudioSource:\n"\
"    def __init__(self, uid: str, file_path: str):\n"\
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
"    def __init__(self, uid: str, file_path: str, size: int):\n"\
"        self.uid = uid\n"\
"        self.file_path = file_path\n"\
"        self.size = size\n"\
"\n"\
"\n"\
"class InputAction:\n"\
"    def __init__(self, name: str, values: list):\n"\
"        self.name = name\n"\
"        self.values = values\n"\
"\n"\
"\n"\
"def configure_game(\n"\
"    game_tile=\"Test Game\",\n"\
"    window_width=800,\n"\
"    window_height=600,\n"\
"    resolution_width=800,\n"\
"    resolution_height=800,\n"\
"    target_fps=66,\n"\
"):\n"\
"    rbe_py_api_internal.configure_game(\n"\
"        game_tile=game_tile,\n"\
"        window_width=window_width,\n"\
"        window_height=window_height,\n"\
"        resolution_width=resolution_width,\n"\
"        resolution_height=resolution_height,\n"\
"        target_fps=target_fps,\n"\
"    )\n"\
"\n"\
"\n"\
"def create_assets(audio_sources=None, textures=None, fonts=None):\n"\
"    if fonts is None:\n"\
"        fonts = []\n"\
"    if textures is None:\n"\
"        textures = []\n"\
"    if audio_sources is None:\n"\
"        audio_sources = []\n"\
"    rbe_py_api_internal.create_assets(\n"\
"        audio_sources=audio_sources, textures=textures, fonts=fonts\n"\
"    )\n"\
"\n"\
"\n"\
"def configure_inputs(input_actions=None):\n"\
"    if input_actions is None:\n"\
"        input_actions = []\n"\
"    rbe_py_api_internal.configure_inputs(input_actions=input_actions)\n"\
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
