from google.oauth2.credentials import Credentials
from googleapiclient.discovery import build
from googleapiclient.errors import HttpError
from google.oauth2 import service_account


def get_service(api_name: str, api_version, scopes: list, key_file_location: str):
    credentials = service_account.Credentials.from_service_account_file(
        key_file_location
    )

    scope_credentials = credentials.with_scopes(scopes)

    service = build(api_name, api_version, credentials=scope_credentials)

    return service


def upload_file(file_path: str):
    scope = "https://www.googleapis.com/auth/drive.metadata.readonly"
    key_file_location = "c:\\Users\\chuko\\Downloads\\crescent-376920-9612570cbb44.json"

    try:
        service = get_service(
            api_name="drive",
            api_version="v3",
            scopes=[scope],
            key_file_location=key_file_location,
        )
        results = (
            service.files()
            .list(pageSize=10, fields="nextPageToken, files(id, name)")
            .execute()
        )
        items = results.get("files", [])
        print(f"items = {items}")
    except Exception as e:
        print(f"Exception: {e}")


upload_file("test.txxt")
