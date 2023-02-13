import os
import platform
import compileall
import shutil
import zipfile
from pathlib import Path, PurePath
from typing import Optional, Callable


class FileUtils:
    @staticmethod
    def remove_dir(directory: str) -> None:
        try:
            shutil.rmtree(directory)
        except Exception as e:
            pass

    @staticmethod
    def create_dir(directory: str) -> None:
        os.mkdir(directory)

    @staticmethod
    def copy_dir(source: str, destination: str, dirs_exist_ok=True) -> None:
        shutil.copytree(source, destination, dirs_exist_ok=dirs_exist_ok)

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
        FileUtils.copy_dir(project_dir, temp_file_path.as_posix())
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

        # Copy dlls
        if "Windows" in platform.system():
            for file in FileUtils.get_dir_file_paths(
                engine_bin_dir_path.as_posix(),
                filter_func=lambda file: file.endswith(".dll"),
            ):
                file_path = Path(file)
                dest_path = temp_file_path / file_path.name
                FileUtils.copy_file(file, dest_path.as_posix())
        else:
            embed_python_path = Path(f"{engine_bin_dir}/embed_python")
            dest_path = temp_file_path / embed_python_path.name
            FileUtils.copy_file(embed_python_path.as_posix(), dest_path.as_posix())
        # Copy Engine Binary
        if "Windows" in platform.system():
            engine_binary_extension = ".exe"
        else:
            engine_binary_extension = ""
        engine_binary_name = f"crescent_engine{engine_binary_extension}"
        engine_binary_path = engine_bin_dir_path / engine_binary_name
        engine_binary_dest_path = (
            temp_file_path / f"{game_title}{engine_binary_extension}"
        )
        FileUtils.copy_file(
            engine_binary_path.as_posix(), engine_binary_dest_path.as_posix()
        )

        # Create export zip
        ProjectArchiver.create_archive(
            name=archive_name,
            source_dir=temp_file_path.as_posix(),
        )

        # Clean up
        FileUtils.remove_dir(temp_file_path.as_posix())
