import sys
import plistlib


def generate_plist_info_file(path: str, title: str, version: str, executable: str):
    with open(path, "wb") as file:
        info_dict = {
            "CFBundleName": title,
            "CFBundleVersion": version,
            "CFBundleExecutable": executable,
            "LSMinimumSystemVersion": "10.15.0",
        }
        plistlib.dump(info_dict, file)


if len(sys.argv) >= 5:
    _path = sys.argv[1]
    _title = sys.argv[2]
    _version = sys.argv[3]
    _executable = sys.argv[4]
    generate_plist_info_file(_path, _title, _version, _executable)
else:
    raise Exception("Not enough args passed in, expecting 4!")
