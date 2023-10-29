SOURCE_INPUT_PATH = "../crescent.py"

SOURCE_OUTPUT_PATH = (
    "../../../engine/src/core/scripting/python/pocketpy/crescent_api_source.h"
)

output_text = "#pragma once\n\n"
output_text += '#define CRE_PYPP_CRESCENT_SOURCE ""\\\n'

with open(SOURCE_INPUT_PATH) as file:
    lines = file.readlines()
    for line in lines:
        line = line.rstrip().replace('"', '\\"')
        output_text += f'"{line}\\n"\\\n'

output_text += '""\n'

# Generate File
with open(SOURCE_OUTPUT_PATH, "w") as source_file:
    source_file.write(output_text)
