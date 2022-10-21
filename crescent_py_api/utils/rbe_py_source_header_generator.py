RBE_PY_API_VERSION = "0.0.1"
RBE_PY_API_SOURCE_PATH = "../crescent_api.py"
RBE_PY_API_SOURCE_OUTPUT_PATH = (
    "../../engine/src/core/scripting/python/crescent_api_source.h"
)


def get_py_source_text() -> str:
    py_source_text = ""
    with open(RBE_PY_API_SOURCE_PATH) as file:
        lines = file.readlines()
    for line in lines:
        formatted_line = line.rstrip().replace('"', '\\"')
        py_source_text += f'"{formatted_line}\\n"\\\n'
    return py_source_text.rstrip()


cre_py_source_text = f"""#pragma once

#define RBE_PY_API_SOURCE_VERSION "{RBE_PY_API_VERSION}"

#define RBE_PY_API_SOURCE_IMPORTER ""\\
"import sys\\n"\\
"import importlib\\n"\\
"import importlib.util\\n"\\
"\\n"\\
"module_name = \\"source_importer\\"\\n"\\
"source_code = \\"\\"\\"\\n"\\
"import sys\\n"\\
"import importlib\\n"\\
"import importlib.util\\n"\\
"\\n"\\
"class SourceImporter:\\n"\\
"   def __init__(self, modules: dict):\\n"\\
"       self._modules = modules\\n"\\
"\\n"\\
"       for module_name in self._modules:\\n"\\
"           module_src = self._modules[module_name]\\n"\\
"           self.import_from_source(module_name, module_src)\\n"\\
"\\n"\\
"   def import_from_source(self, module_name: str, source_code: str):\\n"\\
"       spec = importlib.util.spec_from_loader(module_name, loader=None, is_package=False)\\n"\\
"       module = importlib.util.module_from_spec(spec)\\n"\\
"       exec(source_code, module.__dict__)\\n"\\
"       sys.modules[module_name] = module\\n"\\
"       return module\\n"\\
"\\"\\"\\"\\n"\\
"\\n"\\
"# Load source importer module\\n"\\
"spec = importlib.util.spec_from_loader(module_name, loader=None, is_package=False)\\n"\\
"module = importlib.util.module_from_spec(spec)\\n"\\
"exec(source_code, module.__dict__)\\n"\\
"sys.modules[module_name] = module\\n"\\
"\\n"

#define RBE_PY_API_SOURCE ""\\
{get_py_source_text()}
\"\\n\"

#define RBE_PY_API_SOURCE_IMPORTER_MODULE_IMPORTS ""\\
"from source_importer import SourceImporter\\n"\\
"\\n"\\
"crescent_api_source_code = \\"\\"\\"\\n"\\
RBE_PY_API_SOURCE\\
"\\"\\"\\"\\n"\\
"modules_to_import = {{\\n"\\
"\\"crescent_api\\": crescent_api_source_code,\\n"\\
"}}\\n"\\
"\\n"\\
"game_source_importer = SourceImporter(modules=modules_to_import)\\n"\\
"\\n"
"""

print(cre_py_source_text)

# Generate File
with open(RBE_PY_API_SOURCE_OUTPUT_PATH, "w") as c_py_source_file:
    c_py_source_file.write(cre_py_source_text)
