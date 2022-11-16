from utils.helper import Helper

CRE_PY_API_VERSION = "0.0.1"
CRE_PY_API_SOURCE_PATH = "../crescent_api.py"
CRE_PY_API_SOURCE_OUTPUT_PATH = (
    "../../engine/src/core/scripting/python/crescent_api_source.h"
)
CRE_PY_API_SOURCE_IMPORTER_PATH = "source_importer.py"

cre_py_source_text = f"""#pragma once

#define RBE_PY_API_SOURCE_VERSION "{CRE_PY_API_VERSION}"

#define RBE_PY_API_SOURCE_IMPORTER ""\\
"import sys\\n"\\
"import importlib\\n"\\
"import importlib.util\\n"\\
"\\n"\\
"module_name = \\"source_importer\\"\\n"\\
"source_code = \\"\\"\\"\\n"\\
{Helper.convert_py_file_to_source_c_string(CRE_PY_API_SOURCE_IMPORTER_PATH, False)}
"\\"\\"\\"\\n"\\
"\\n"\\
"# Load source importer module\\n"\\
"spec = importlib.util.spec_from_loader(module_name, loader=None, is_package=False)\\n"\\
"module = importlib.util.module_from_spec(spec)\\n"\\
"exec(source_code, module.__dict__)\\n"\\
"sys.modules[module_name] = module\\n"\\
"\\n"

#define RBE_PY_API_SOURCE ""\\
{Helper.convert_py_file_to_source_c_string(CRE_PY_API_SOURCE_PATH)}
"""

print(cre_py_source_text)

# Generate File
with open(CRE_PY_API_SOURCE_OUTPUT_PATH, "w") as c_py_source_file:
    c_py_source_file.write(cre_py_source_text)
