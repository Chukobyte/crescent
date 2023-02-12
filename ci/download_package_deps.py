import pathlib
import sys

from gd_service import GoogleDriveService

WINDOWS_ENGINE_BINARY = "crescent_engine_windows.exe"
WINDOWS_ENGINE_EDITOR_BINARY = "crescent_engine_editor_windows.exe"
WINDOWS_RUN_DEPS_ZIP = "windows_run_dependencies.zip"
LINUX_ENGINE_BINARY = "crescent_engine_linux"
LINUX_ENGINE_EDITOR_BINARY = "crescent_engine_editor_linux"
LINUX_PYTHON_ZIP = "python310_linux.zip"
MACOSX_ENGINE_BINARY = "crescent_engine_macosx"
MACOSX_PYTHON_ZIP = "python310_macosx.zip"

ALL_DEP_FILES = [
    WINDOWS_ENGINE_BINARY,
    WINDOWS_ENGINE_EDITOR_BINARY,
    WINDOWS_RUN_DEPS_ZIP,
    LINUX_ENGINE_BINARY,
    LINUX_ENGINE_EDITOR_BINARY,
    LINUX_PYTHON_ZIP,
    MACOSX_ENGINE_BINARY,
    MACOSX_PYTHON_ZIP,
]

if len(sys.argv) >= 2:
    cred_file_path = sys.argv[1]
    drive_service = GoogleDriveService(credential_file_path=cred_file_path)
    drive_service.download_files(ALL_DEP_FILES)
    files_not_downloaded = []
    for binary_path in ALL_DEP_FILES:
        if not pathlib.Path(binary_path).exists():
            files_not_downloaded.append(binary_path)
    if files_not_downloaded:
        raise Exception(f"Wasn't able to download file/s '{files_not_downloaded}'")
else:
    print("ERROR: Didn't pass in file cred path!")
