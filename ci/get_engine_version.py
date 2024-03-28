import re


def parse_version_from_header(header_file_path : str) -> str:
    version_info = {}
    pattern = re.compile(r'#define CRE_VERSION_(\w+)\s+(\d+)')

    with open(header_file_path, 'r') as file:
        for line in file:
            match = pattern.match(line)
            if match:
                key, value = match.groups()
                version_info[key] = value

    version = "{MAJOR}.{MINOR}.{PATCH}".format(**version_info)
    return version


header_path = "../engine/src/core/core_info.h"
engine_version = parse_version_from_header(header_path)
print(f"\nv{engine_version}\n")
