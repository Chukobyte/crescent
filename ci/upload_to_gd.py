import sys

from gd_service import GoogleDriveService

if len(sys.argv) >= 4:
    artifact_file_path = sys.argv[1]
    cred_file_path = sys.argv[2]
    gd_folder_id = sys.argv[3]
    new_file_name = None
    if len(sys.argv) >= 5:
        new_file_name = sys.argv[4]
    print(f"artifact_file_path = {artifact_file_path}")
    print(f"new_file_name = {new_file_name}")

    drive_service = GoogleDriveService(credential_file_path=cred_file_path)
    drive_service.upload_file(
        file_path=artifact_file_path,
        folder_id=gd_folder_id,
        new_file_name=new_file_name,
    )
else:
    raise Exception("Didn't pass in 3 or more args!")
