import io
import pathlib
import shutil
from typing import List

from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.http import MediaFileUpload, MediaIoBaseDownload


class GoogleDriveFile:
    def __init__(self, drive_file: dict):
        self.id = drive_file["id"]
        self.name = drive_file["name"]
        self.mime_type = drive_file["mimeType"]
        self.kind = drive_file["kind"]


class GoogleDriveService:
    def __init__(self, credential_file_path: str):
        self.service = None
        self.credential_file_path = ""

        valid_credential_file_path = pathlib.Path(credential_file_path)
        if valid_credential_file_path.exists():
            self.credential_file_path = str(valid_credential_file_path)
        else:
            raise Exception(f"credential file '{credential_file_path}' not found!")

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

    def get_files(self, file_names: list) -> List[GoogleDriveFile]:
        matching_files = []
        try:
            for file in self.get_all_files():
                if file["name"] in file_names:
                    matching_files.append(GoogleDriveFile(file))
        except Exception as e:
            print(f"Error getting files '{file_names}': {e}")
        return matching_files

    def get_file(self, file_name: str):
        files = self.get_all_files()
        try:
            for file in files:
                if file["name"] == file_name:
                    return GoogleDriveFile(file)
            return None
        except Exception as e:
            print(f"Error getting file '{file_name}': {e}")

    def download_files(self, file_names: list):
        files_metadata = self.get_files(file_names)
        service = self.get_service()
        for file in files_metadata:
            request = service.files().get_media(fileId=file.id)
            fh = io.BytesIO()
            downloader = MediaIoBaseDownload(fh, request, chunksize=204800)

            done = False
            try:
                print(f"downloading file '{file.name}'...")
                while not done:
                    status, done = downloader.next_chunk()

                fh.seek(0)

                with open(file.name, "wb") as f:
                    print(f"finished downloading file '{file.name}'")
                    shutil.copyfileobj(fh, f)
            except Exception as e:
                print(f"Something went wrong downloading!: {e}")
                continue

    def download_file(self, file_name: str):
        pass

    def upload_file(
        self,
        file_path: str,
        folder_id: str,
        new_file_name: str = None,
        mime_type="application/octet-stream",
    ):
        service = self.get_service()
        try:
            valid_file_path = pathlib.Path(file_path)
            if not valid_file_path.exists():
                print(f"Error file '{file_path}' doesn't exist, not uploading!")
                raise Exception(
                    f"Error file '{file_path}' doesn't exist, not uploading!"
                )

            if new_file_name:
                file_name = new_file_name
            else:
                file_name = valid_file_path.name
            file_metadata = {"name": file_name, "parents": [folder_id]}
            media = MediaFileUpload(
                str(valid_file_path), mimetype=mime_type, resumable=True
            )

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
