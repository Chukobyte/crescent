from typing import List


class SourceInput:
    def __init__(self, file_path: str, macro_name: str):
        self.file_path = file_path
        self.macro_name = macro_name


def generate_source_header_file(
    output_path: str, source_inputs: List[SourceInput]
) -> None:
    output_text = "#pragma once\n\n"
    for source_input in source_inputs:
        output_text += f'#define {source_input.macro_name} ""\\\n'
        with open(source_input.file_path) as file:
            lines = file.readlines()
            for line in lines:
                line = line.rstrip().replace('"', '\\"')
                output_text += f'"{line}\\n"\\\n'
            output_text += '"\\n"\n\n'

    with open(output_path, "w") as source_file:
        source_file.write(output_text)


generate_source_header_file(
    output_path="../../../engine/src/core/scripting/python/pocketpy/crescent_api_source.h",
    source_inputs=[
        SourceInput(file_path="../crescent.py", macro_name="CRE_PKPY_CRESCENT_SOURCE"),
        SourceInput(
            file_path="../crescent_internal_py.py",
            macro_name="CRE_PKPY_CRESCENT_INTERNAL_PY_SOURCE",
        ),
    ],
)
