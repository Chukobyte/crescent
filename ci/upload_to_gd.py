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


def get_service(api_name: str, api_version: str, scopes: list, key_file_location: str):
    credentials = service_account.Credentials.from_service_account_file(
        key_file_location
    )

    scope_credentials = credentials.with_scopes(scopes)

    service = build(api_name, api_version, credentials=scope_credentials)

    return service


def get_all_files(service):
    result = []
    page_token = None
    while True:
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


def delete_all_files(service):
    files = get_all_files(service)
    try:
        for file in files:
            service.files().delete(fileId=file["id"]).execute()
            print(f"deleted file {file['name']}!")
    except Exception as e:
        print(f"Error deleting files: {e}")


def get_file(service, file_name: str):
    files = get_all_files(service)
    try:
        for file in files:
            if file["name"] == file_name:
                return GoogleDriveFile(file)
        return None
    except Exception as e:
        print(f"Error getting file '{file_name}': {e}")


def upload_file(
    file_path: str, key_file_location: str, folder_id: str, new_file_name: str = None
):
    scopes = [
        "https://www.googleapis.com/auth/drive",
    ]

    try:
        service = get_service(
            api_name="drive",
            api_version="v3",
            scopes=scopes,
            key_file_location=key_file_location,
        )

        # delete_all_files(service)

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
        existing_file = get_file(service, file_name=file_name)
        if existing_file:
            del file_metadata["parents"]
            file = (
                service.files()
                .update(fileId=existing_file.id, media_body=media, body=file_metadata)
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
    upload_file(artifact_file_path, cred_file_path, gd_folder_id, new_file_name)
else:
    print("ERROR: Didn't pass in 3 or more args!")
