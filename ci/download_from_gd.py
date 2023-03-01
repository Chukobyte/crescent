import pathlib
import sys

from gd_service import GoogleDriveService

if len(sys.argv) >= 3:
    cred_file_path = sys.argv[1]
    drive_service = GoogleDriveService(credential_file_path=cred_file_path)
    files_to_download = []
    for i, file in enumerate(sys.argv[2:], 2):
        files_to_download.append(file)

    print(f"Downloading the following files: {files_to_download}")
    drive_service.download_files(files_to_download)
    files_not_downloaded = []
    for binary_path in files_to_download:
        if not pathlib.Path(binary_path).exists():
            files_not_downloaded.append(binary_path)
    if files_not_downloaded:
        raise Exception(f"Wasn't able to download file/s '{files_not_downloaded}'")
else:
    raise Exception("Didn't pass in file cred path and/or files to download!")
