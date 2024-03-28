import re
import argparse


def parse_version_from_header(header_file_path: str) -> str:
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


parser = argparse.ArgumentParser(description='Parse version information from a C header file.')
parser.add_argument('header_file_path', type=str, help='Path to the C header file containing the version defines.')

args = parser.parse_args()
engine_version = parse_version_from_header(args.header_file_path)
print(f"\n{engine_version}\n")
