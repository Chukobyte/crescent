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

#define RBE_API_SOURCE_VERSION "0.0.1"

#define RBE_PY_API_SOURCE ""\\
{get_py_source_text()}
\"\\\\n\"
"""

print(rbe_py_source_text)

# Generate File
with open(RBE_PY_API_SOURCE_OUTPUT_PATH, "w") as c_py_source_file:
    c_py_source_file.write(rbe_py_source_text)
