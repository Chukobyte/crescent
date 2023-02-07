import sys
import pathlib

from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.http import MediaFileUpload


class GoogleDriveFile:
    def __init__(self, file: dict):
        self.id = file["id"]
        self.name = file["name"]
        self.mime_type = file["mimeType"]
        self.kind = file["kind"]


class GoogleDriveService:
    def __init__(self, credential_file_path: str):
        self.credential_file_path = credential_file_path
        self.service = None

    def get_service(self):
        if not self.service:
            scopes = [
                "https://www.googleapis.com/auth/drive",
            ]
            credentials = service_account.Credentials.from_service_account_file(
                self.credential_file_path
            )
            scope_credentials = credentials.with_scopes(scopes)
            self.service = build("drive", "v3", credentials=scope_credentials)
        return self.service

    def get_all_files(self) -> list:
        result = []
        page_token = None
        while True:
            service = self.get_service()
            try:
                param = {}
                if page_token:
                    param["pageToken"] = page_token
                files = service.files().list(**param).execute()

                # print(f"files = {files}")
                # result.extend(files['items'])
                # result.extend(files)
                result.extend(files["files"])
                page_token = files.get("nextPageToken")
                if not page_token:
                    break
            except Exception as error:
                print(f"An error occurred: {error}")
                break
        return result

    def delete_all_files(self):
        service = self.get_service()
        files = self.get_all_files()
        try:
            for file in files:
                service.files().delete(fileId=file["id"]).execute()
                print(f"deleted file {file['name']}!")
        except Exception as e:
            print(f"Error deleting files: {e}")

    def get_file(self, file_name: str):
        service = self.get_service()
        files = self.get_all_files()
        try:
            for file in files:
                if file["name"] == file_name:
                    return GoogleDriveFile(file)
            return None
        except Exception as e:
            print(f"Error getting file '{file_name}': {e}")

    def upload_file(self, file_path: str, folder_id: str, new_file_name: str = None):
        service = self.get_service()
        try:
            valid_file_path = pathlib.Path(file_path)
            if not valid_file_path.exists():
                print(f"Error file '{file_path}' doesn't exist, not uploading!")
                return None

            if new_file_name:
                file_name = new_file_name
            else:
                file_name = valid_file_path.name
            file_metadata = {"name": file_name, "parents": [folder_id]}
            media = MediaFileUpload(file_path, mimetype="text/plain")

            # Update existing file if it exists
            existing_file = self.get_file(file_name=file_name)
            if existing_file:
                del file_metadata["parents"]
                file = (
                    service.files()
                    .update(
                        fileId=existing_file.id, media_body=media, body=file_metadata
                    )
                    .execute()
                )
            else:
                file = (
                    service.files()
                    .create(body=file_metadata, media_body=media, fields="id")
                    .execute()
                )
            print(f"File ID: {file.get('id')}")
        except Exception as e:
            print(f"Exception: {e}")


if len(sys.argv) >= 4:
    artifact_file_path = sys.argv[1]
    cred_file_path = sys.argv[2]
    gd_folder_id = sys.argv[3]
    new_file_name = None
    if len(sys.argv) >= 5:
        new_file_name = sys.argv[4]

    drive_service = GoogleDriveService(credential_file_path=cred_file_path)
    # drive_service.delete_all_files()
    drive_service.upload_file(
        file_path=artifact_file_path,
        folder_id=gd_folder_id,
        new_file_name=new_file_name,
    )
else:
    print("ERROR: Didn't pass in 3 or more args!")
