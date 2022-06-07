RBE_PY_API_SOURCE_PATH = "../rbe_py_api.py"
RBE_PY_API_SOURCE_OUTPUT_PATH = (
    "../../engine/src/core/scripting/python/rbe_py_api_source.h"
)

def get_py_source_text() -> str:
    py_source_text = ""
    with open(RBE_PY_API_SOURCE_PATH) as file:
        lines = file.readlines()
    for line in lines:
        formatted_line = line.rstrip().replace('"', '\\"')
        py_source_text += f'"{formatted_line}\\\\n"\\\n'
    return py_source_text.rstrip()

rbe_py_source_text = f"""#pragma once

#define RBE_PY_API_SOURCE_VERSION "0.0.1"

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
\"\\\\n\"
"""

print(rbe_py_source_text)

# Generate File
with open(RBE_PY_API_SOURCE_OUTPUT_PATH, "w") as c_py_source_file:
    c_py_source_file.write(rbe_py_source_text)
