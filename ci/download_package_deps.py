import pathlib
import sys

from gd_service import GoogleDriveService

WINDOWS_ENGINE_BINARY = "crescent_engine_windows.exe"
LINUX_ENGINE_BINARY = "crescent_engine_linux"
MACOSX_ENGINE_BINARY = "crescent_engine_macos"

if len(sys.argv) >= 2:
    cred_file_path = sys.argv[1]
    drive_service = GoogleDriveService(credential_file_path=cred_file_path)
    drive_service.download_files(
        [WINDOWS_ENGINE_BINARY, LINUX_ENGINE_BINARY, MACOSX_ENGINE_BINARY]
    )
    files_not_downloaded = []
    for binary_path in [WINDOWS_ENGINE_BINARY, LINUX_ENGINE_BINARY, MACOSX_ENGINE_BINARY]:
        if not pathlib.Path(binary_path).exists():
            files_not_downloaded.append(binary_path)
    if files_not_downloaded:
        raise Exception(f"Wasn't able to download file/s '{files_not_downloaded}'")
else:
    print("ERROR: Didn't pass in file cred path!")
