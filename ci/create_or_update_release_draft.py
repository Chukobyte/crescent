import json
import os
import sys

from github import Github
from github.GithubException import UnknownObjectException


class GithubRepoInstance:
    def __init__(self, token: str, repo_name: str):
        self.token = token
        self.repo_name = repo_name
        self._client = Github(login_or_token=self.token)
        self.repo = self._client.get_repo(self.repo_name)

    def get_release(self, release_name: str):
        try:
            return self.repo.get_release(release_name)
        except UnknownObjectException:
            pass
        return None

    def does_full_release_exist(self, release_name: str) -> bool:
        return self.get_release(release_name) is not None

    def get_release_draft(self, release_name: str):
        for release in self.repo.get_releases():
            if release_name == release.title and release.draft:
                return release
        return None


def create_release_draft_stub(token: str, release_version: str, files_to_upload: list):
    repo_instance = GithubRepoInstance(token=token, repo_name="Chukobyte/crescent")

    tag_name = f"v{release_version}"

    if repo_instance.does_full_release_exist(release_name=tag_name):
        print(
            "Release already exists, skipping!  Consider incrementing the version number in core_info.h!"
        )
        return None

    current_release = repo_instance.get_release_draft(tag_name)
    if not current_release:
        current_release = repo_instance.repo.create_git_release(
            tag_name,
            tag_name,
            f"Auto generated pre-release for {tag_name}, update with proper release notes!",
            draft=True,
            prerelease=True,
        )
        print("Release doesn't exist, creating...")
    else:
        print("Release already created, just updating instead")

    if not current_release.draft:
        print(f"{tag_name} is not a pre-release draft, not updating!")
        return None

    # We have verified this is a pre-release, now upload artifacts
    assets = current_release.get_assets()

    for file_path in files_to_upload:
        with open(file_path, "rb") as f:
            file_name = os.path.basename(file_path)
            # Delete previous asset if it exists
            for asset in assets:
                if file_name == asset.name:
                    print(f"Removing previous asset {asset.name}")
                    asset.delete_asset()
                    break
            # Now upload new asset
            print(f"Uploading {file_name} to release {tag_name}")
            current_release.upload_asset(
                file_path, content_type="application/octet-stream", name=file_name
            )


if len(sys.argv) >= 4:
    github_token = sys.argv[1]
    release_version = sys.argv[2]
    files_to_upload = []
    for i, file in enumerate(sys.argv[3:], 3):
        files_to_upload.append(file)
    create_release_draft_stub(token=github_token, release_version=release_version, files_to_upload=files_to_upload)
else:
    raise Exception("Didn't pass in another args!")
