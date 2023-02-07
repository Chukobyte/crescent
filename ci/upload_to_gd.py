import sys
import pathlib

from googleapiclient.discovery import build
from google.oauth2 import service_account
from googleapiclient.http import MediaFileUpload


def get_service(api_name: str, api_version: str, scopes: list, key_file_location: str):
    credentials = service_account.Credentials.from_service_account_file(
        key_file_location
    )

    scope_credentials = credentials.with_scopes(scopes)

    service = build(api_name, api_version, credentials=scope_credentials)

    return service


def upload_file(file_path: str, key_file_location: str, folder_id: str):
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

        file_name = pathlib.Path(file_path)
        file_metadata = {"name": file_name.name, "parents": [folder_id]}
        media = MediaFileUpload(file_path, mimetype="text/plain")

        file = (
            service.files()
            .create(body=file_metadata, media_body=media, fields="id")
            .execute()
        )
        print(f"File ID: {file.get('id')}")
    except Exception as e:
        print(f"Exception: {e}")


if len(sys.argv) >= 4:
    print("Hey")
    artifact_file_path = sys.argv[1]
    cred_file_path = sys.argv[2]
    gd_folder_id = sys.argv[3]
    upload_file(artifact_file_path, cred_file_path, gd_folder_id)
else:
    print("ERROR: Didn't pass in 3 args!")
