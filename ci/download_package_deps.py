import pathlib
import sys

from gd_service import GoogleDriveService

PYTHON_VERSION = "3.10.5"

WINDOWS_ENGINE_BINARY = "crescent_engine_windows.exe"
WINDOWS_ENGINE_EDITOR_BINARY = "crescent_engine_editor_windows.exe"
WINDOWS_RUN_DEPS_ZIP = "windows_run_dependencies.zip"
LINUX_ENGINE_BINARY = "crescent_engine_linux"
LINUX_ENGINE_EDITOR_BINARY = "crescent_engine_editor_linux"
LINUX_PYTHON_ZIP = f"python-{PYTHON_VERSION}-embed-linux.zip"
MACOSX_ENGINE_BINARY = "crescent_engine_macosx"
MACOSX_PYTHON_ZIP = f"python-{PYTHON_VERSION}-embed-macosx.zip"

BINARY_FILES = [
    WINDOWS_ENGINE_BINARY,
    WINDOWS_ENGINE_EDITOR_BINARY,
    LINUX_ENGINE_BINARY,
    LINUX_ENGINE_EDITOR_BINARY,
    MACOSX_ENGINE_BINARY,
]

DEP_FILES = [
    WINDOWS_RUN_DEPS_ZIP,
    LINUX_PYTHON_ZIP,
    MACOSX_PYTHON_ZIP,
]

ALL_FILES = BINARY_FILES + DEP_FILES

if len(sys.argv) >= 2:
    cred_file_path = sys.argv[1]
    drive_service = GoogleDriveService(credential_file_path=cred_file_path)
    print(f"Downloading the following binaries: {BINARY_FILES}")
    drive_service.download_files(BINARY_FILES)
    print(f"Downloading the dependency files: {DEP_FILES}")
    drive_service.download_files(DEP_FILES)
    files_not_downloaded = []
    for binary_path in ALL_FILES:
        if not pathlib.Path(binary_path).exists():
            files_not_downloaded.append(binary_path)
    if files_not_downloaded:
        raise Exception(f"Wasn't able to download file/s '{files_not_downloaded}'")
else:
    print("ERROR: Didn't pass in file cred path!")
