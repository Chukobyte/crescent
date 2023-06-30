import os
import compileall
import json
import shutil
import zipfile
import tarfile
from pathlib import Path, PurePath
from typing import Optional, Callable


class FileUtils:
    @staticmethod
    def remove_dir(directory: str) -> None:
        def handle_remove_error(func, path, exc_info):
            # Handle specific error types
            if isinstance(exc_info[1], PermissionError):
                return

            # Reraise the exception for other errors
            raise exc_info[0](exc_info[1]).with_traceback(exc_info[2])

        try:
            for root, dirs, files in os.walk(directory, topdown=False):
                for file in files:
                    file_path = os.path.join(root, file)
                    FileUtils.delete_file(file_path)
                for child_dir in dirs:
                    dir_path = os.path.join(root, child_dir)
                    FileUtils.remove_dir(dir_path)

            shutil.rmtree(directory, onerror=handle_remove_error)
        except Exception as e:
            pass

    @staticmethod
    def create_dir(directory: str) -> None:
        os.mkdir(directory)

    @staticmethod
    def copy_dir(source: str, destination: str, dirs_exist_ok=True) -> None:
        shutil.copytree(source, destination, dirs_exist_ok=dirs_exist_ok)

    @staticmethod
    def copy_dir_exclude_dot(source: str, destination: str, dirs_exist_ok=True) -> None:
        for item in os.listdir(source):
            if not item.startswith("."):
                source_path = os.path.join(source, item)
                destination_path = os.path.join(destination, item)
                if os.path.isdir(source_path):
                    shutil.copytree(
                        source_path, destination_path, dirs_exist_ok=dirs_exist_ok
                    )
                else:
                    shutil.copy2(source_path, destination_path)

    @staticmethod
    def move_dir(source: str, destination: str):
        shutil.move(source, destination)

    @staticmethod
    def get_dir_file_paths(
        directory: str, recursive=True, filter_func: Optional[Callable] = None
    ):
        if not recursive:
            if not filter_func:
                for file in os.listdir(directory):
                    yield os.path.join(directory, file)
            else:
                for file in os.listdir(directory):
                    if filter_func(file):
                        yield os.path.join(directory, file)
        else:
            if not filter_func:
                for root, dirs, files in os.walk(directory):
                    for file in files:
                        yield os.path.join(root, file)
            else:
                for root, dirs, files in os.walk(directory):
                    for file in files:
                        if filter_func(file):
                            yield os.path.join(root, file)

    @staticmethod
    def copy_file(source: str, destination: str) -> None:
        shutil.copyfile(source, destination)

    @staticmethod
    def delete_file(path: str) -> None:
        if os.path.isfile(path):
            os.remove(path)
        elif os.path.isdir(path):
            FileUtils.remove_dir(path)

    @staticmethod
    def move_file(source: str, destination: str):
        shutil.move(source, destination)

    @staticmethod
    def write_file(path: str, content: str) -> None:
        with open(path, "w") as file:
            file.write(content)

    @staticmethod
    def write_info_plist_file(path: str, info: dict) -> None:
        xml_string = '<?xml version="1.0" encoding="UTF-8"?>\n'
        xml_string += '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">\n'
        xml_string += '<plist version="1.0">\n'
        xml_string += "<dict>\n"
        for key, value in info.items():
            xml_string += f"    <key>{key}</key>\n"
            xml_string += f"    <string>{value}</string>\n"
        xml_string += "</dict>\n"
        xml_string += "</plist>\n"

        with open(path, "w", encoding="utf-8") as file:
            file.write(xml_string)


class PythonCompiler:
    @staticmethod
    def _is_valid_file_path(file_path: str) -> bool:
        path = Path(file_path)
        for exclusion in ["venv", "__pycache__"]:
            if f"/{exclusion}" in path.as_posix():
                return False
        return True

    @staticmethod
    def compile_dir(
        directory: str, legacy=True, quiet=True, delete_uncompiled_file=False
    ) -> None:
        for file_path in FileUtils.get_dir_file_paths(
            directory=directory,
            filter_func=lambda file: file.endswith(".py")
            and PythonCompiler._is_valid_file_path(file),
        ):
            compileall.compile_file(fullname=file_path, legacy=legacy, quiet=quiet)
            if delete_uncompiled_file:
                os.remove(file_path)

    @staticmethod
    def remove_pyc_files(directory: str, quiet=True) -> None:
        for file_path in FileUtils.get_dir_file_paths(
            directory=directory,
            filter_func=lambda file: file.endswith(".pyc")
            and PythonCompiler._is_valid_file_path(file),
        ):
            if not quiet:
                print(f"Removing {file_path}")
            os.remove(file_path)


class ProjectArchiver:
    @staticmethod
    def create_archive(name: str, source_dir: str) -> None:
        # Check if tarball first
        if name.endswith(".tar.gz"):
            with tarfile.open(name, "w:gz") as tar:
                for root, dirs, files in os.walk(source_dir):
                    for file in files:
                        file_path = os.path.join(root, file)
                        tar.add(
                            file_path, arcname=os.path.relpath(file_path, source_dir)
                        )
        else:
            with zipfile.ZipFile(name, "w") as export_zip_file:
                current_cwd = os.getcwd()
                os.chdir(source_dir)
                name_path = PurePath(name)

                # TODO: Clean code up
                for root, dirs, files in os.walk("."):
                    for dir in dirs:
                        export_zip_file.write(os.path.join(root, dir))
                    for file in files:
                        file_path_text = os.path.join(root, file)
                        file_path = PurePath(file_path_text)
                        if not name_path.as_posix().endswith(file_path.as_posix()):
                            export_zip_file.write(file_path_text)
                os.chdir(current_cwd)


class GameExporter:
    @staticmethod
    def fix_game_title(game_title: str) -> str:
        return game_title.lower().replace(" ", "_").replace("-", "_")

    @staticmethod
    def export_game(
        game_title: str,
        archive_name: str,
        engine_bin_dir: str,
        project_dir: str,
        temp_path: str,
    ):
        # Fix up game title
        game_title = GameExporter.fix_game_title(game_title)
        # Setup
        temp_file_path = PurePath(temp_path) / "tmp_build"
        FileUtils.remove_dir(temp_file_path.as_posix())
        FileUtils.create_dir(temp_file_path.as_posix())
        # Copy Project Files
        FileUtils.copy_dir_exclude_dot(project_dir, temp_file_path.as_posix())
        # Copy Engine Bin Files
        engine_bin_dir_path = PurePath(engine_bin_dir)
        engine_assets_fonts_path = engine_bin_dir_path / "assets" / "fonts"
        engine_assets_resources_path = engine_bin_dir_path / "assets" / "resources"
        dest_asset_path = temp_file_path / "assets"
        dest_asset_fonts_path = dest_asset_path / "fonts"
        dest_asset_resources_path = dest_asset_path / "resources"
        FileUtils.copy_dir(
            engine_assets_fonts_path.as_posix(), dest_asset_fonts_path.as_posix()
        )
        FileUtils.copy_dir(
            engine_assets_resources_path.as_posix(),
            dest_asset_resources_path.as_posix(),
        )
        # Compile python source files
        PythonCompiler.compile_dir(
            directory=temp_file_path.as_posix(), delete_uncompiled_file=True, quiet=True
        )
        # Create pack file and remove all project files
        project_pack_path = temp_file_path / f"{game_title}.pck"
        ProjectArchiver.create_archive(
            name=project_pack_path.as_posix(),
            source_dir=temp_file_path.as_posix(),
        )
        # Remove all files except .pck
        for file in FileUtils.get_dir_file_paths(
            temp_file_path.as_posix(),
            recursive=False,
        ):
            file_to_delete_path = PurePath(file)
            if file_to_delete_path != project_pack_path:
                FileUtils.delete_file(file_to_delete_path.as_posix())

        # Get OS type by inferring from bin path
        export_os_type = engine_bin_dir_path.as_posix().split("/")[-1]
        engine_binary_extension = ""
        if export_os_type == "windows":
            # Copy dlls
            for file in FileUtils.get_dir_file_paths(
                engine_bin_dir_path.as_posix(),
                filter_func=lambda file: file.endswith(".dll"),
            ):
                file_path = Path(file)
                dest_path = temp_file_path / file_path.name
                FileUtils.copy_file(file, dest_path.as_posix())
            engine_binary_extension = ".exe"
        else:
            # Copy embedded python files
            embed_python_path = Path(f"{engine_bin_dir}/embed_python")
            dest_path = temp_file_path / embed_python_path.name
            FileUtils.copy_dir(embed_python_path.as_posix(), dest_path.as_posix())
        # Copy Engine Binary
        engine_binary_name = f"crescent_engine{engine_binary_extension}"
        engine_binary_path = engine_bin_dir_path / engine_binary_name
        engine_binary_dest_path = (
            temp_file_path / f"{game_title}{engine_binary_extension}"
        )
        FileUtils.copy_file(
            engine_binary_path.as_posix(), engine_binary_dest_path.as_posix()
        )

        # Create app bundle if macosx
        if export_os_type == "macosx":
            app_bundle_path = temp_file_path / f"{game_title}.app"
            FileUtils.create_dir(app_bundle_path.as_posix())
            contents_path = app_bundle_path / "Contents"
            FileUtils.create_dir(contents_path.as_posix())
            contents_macos_path = contents_path / "MacOS"
            FileUtils.create_dir(contents_macos_path.as_posix())
            contents_resources_path = contents_path / "Resources"
            FileUtils.create_dir(contents_resources_path.as_posix())
            contents_info_plist_path = contents_path / "Info.plist"
            # Get version from project json file
            project_config_file_path = Path(project_dir) / "project.ccfg"
            with open(project_config_file_path.as_posix(), "r") as f:
                json_data = json.load(f)
                build_version = json_data.get("version", "0.0.1")
            plist_info = {
                "CFBundleName": game_title,
                "CFBundleVersion": build_version,
                "CFBundleExecutable": f"{game_title}{engine_binary_extension}",
                "LSMinimumSystemVersion": "10.15.0",
            }
            FileUtils.write_info_plist_file(
                contents_info_plist_path.as_posix(), plist_info
            )
            # Move all files and folders to MacOS dir
            files = os.listdir(temp_file_path.as_posix())
            for file_name in files:
                full_path = os.path.join(temp_file_path.as_posix(), file_name)
                if os.path.isfile(full_path):
                    file_move_path = contents_macos_path / file_name
                    FileUtils.move_file(full_path, file_move_path.as_posix())
                elif os.path.isdir(full_path):
                    full_dir_path = Path(full_path)
                    if full_dir_path != app_bundle_path:
                        dir_move_path = contents_macos_path / file_name
                        FileUtils.move_dir(full_path, dir_move_path.as_posix())

        # Create export '.tar.gz' or '.zip'
        ProjectArchiver.create_archive(
            name=archive_name,
            source_dir=temp_file_path.as_posix(),
        )

        # Clean up
        FileUtils.remove_dir(temp_file_path.as_posix())
