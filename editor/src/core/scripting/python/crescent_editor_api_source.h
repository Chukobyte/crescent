#pragma once

#define CRE_PY_API_SOURCE_VERSION "0.0.1"

#define CRE_PY_EDITOR_API_GAME_EXPORTER_SOURCE ""\
"import os\n"\
"import platform\n"\
"import compileall\n"\
"import shutil\n"\
"import stat\n"\
"import zipfile\n"\
"from pathlib import Path, PurePath\n"\
"from typing import Optional, Callable\n"\
"\n"\
"\n"\
"class FileUtils:\n"\
"    @staticmethod\n"\
"    def remove_dir(directory: str) -> None:\n"\
"        try:\n"\
"            shutil.rmtree(directory)\n"\
"        except Exception as e:\n"\
"            pass\n"\
"\n"\
"    @staticmethod\n"\
"    def create_dir(directory: str) -> None:\n"\
"        os.mkdir(directory)\n"\
"\n"\
"    @staticmethod\n"\
"    def copy_dir(source: str, destination: str, dirs_exist_ok=True) -> None:\n"\
"        shutil.copytree(source, destination, dirs_exist_ok=dirs_exist_ok)\n"\
"\n"\
"    @staticmethod\n"\
"    def get_dir_file_paths(\n"\
"        directory: str, recursive=True, filter_func: Optional[Callable] = None\n"\
"    ):\n"\
"        if not recursive:\n"\
"            if not filter_func:\n"\
"                for file in os.listdir(directory):\n"\
"                    yield os.path.join(directory, file)\n"\
"            else:\n"\
"                for file in os.listdir(directory):\n"\
"                    if filter_func(file):\n"\
"                        yield os.path.join(directory, file)\n"\
"        else:\n"\
"            if not filter_func:\n"\
"                for root, dirs, files in os.walk(directory):\n"\
"                    for file in files:\n"\
"                        yield os.path.join(root, file)\n"\
"            else:\n"\
"                for root, dirs, files in os.walk(directory):\n"\
"                    for file in files:\n"\
"                        if filter_func(file):\n"\
"                            yield os.path.join(root, file)\n"\
"\n"\
"    @staticmethod\n"\
"    def copy_file(source: str, destination: str) -> None:\n"\
"        shutil.copyfile(source, destination)\n"\
"\n"\
"    @staticmethod\n"\
"    def delete_file(path: str) -> None:\n"\
"        if os.path.isfile(path):\n"\
"            os.remove(path)\n"\
"        elif os.path.isdir(path):\n"\
"            FileUtils.remove_dir(path)\n"\
"\n"\
"\n"\
"class PythonCompiler:\n"\
"    @staticmethod\n"\
"    def _is_valid_file_path(file_path: str) -> bool:\n"\
"        path = Path(file_path)\n"\
"        for exclusion in [\"venv\", \"__pycache__\"]:\n"\
"            if f\"/{exclusion}\" in path.as_posix():\n"\
"                return False\n"\
"        return True\n"\
"\n"\
"    @staticmethod\n"\
"    def compile_dir(\n"\
"        directory: str, legacy=True, quiet=True, delete_uncompiled_file=False\n"\
"    ) -> None:\n"\
"        for file_path in FileUtils.get_dir_file_paths(\n"\
"            directory=directory,\n"\
"            filter_func=lambda file: file.endswith(\".py\")\n"\
"            and PythonCompiler._is_valid_file_path(file),\n"\
"        ):\n"\
"            compileall.compile_file(fullname=file_path, legacy=legacy, quiet=quiet)\n"\
"            if delete_uncompiled_file:\n"\
"                os.remove(file_path)\n"\
"\n"\
"    @staticmethod\n"\
"    def remove_pyc_files(directory: str, quiet=True) -> None:\n"\
"        for file_path in FileUtils.get_dir_file_paths(\n"\
"            directory=directory,\n"\
"            filter_func=lambda file: file.endswith(\".pyc\")\n"\
"            and PythonCompiler._is_valid_file_path(file),\n"\
"        ):\n"\
"            if not quiet:\n"\
"                print(f\"Removing {file_path}\")\n"\
"            os.remove(file_path)\n"\
"\n"\
"\n"\
"class ProjectArchiver:\n"\
"    @staticmethod\n"\
"    def create_archive(name: str, source_dir: str) -> None:\n"\
"        with zipfile.ZipFile(name, \"w\") as export_zip_file:\n"\
"            current_cwd = os.getcwd()\n"\
"            os.chdir(source_dir)\n"\
"            name_path = PurePath(name)\n"\
"\n"\
"            # TODO: Clean code up\n"\
"            for root, dirs, files in os.walk(\".\"):\n"\
"                for dir in dirs:\n"\
"                    export_zip_file.write(os.path.join(root, dir))\n"\
"                for file in files:\n"\
"                    file_path_text = os.path.join(root, file)\n"\
"                    file_path = PurePath(file_path_text)\n"\
"                    if not name_path.as_posix().endswith(file_path.as_posix()):\n"\
"                        export_zip_file.write(file_path_text)\n"\
"            os.chdir(current_cwd)\n"\
"\n"\
"\n"\
"class GameExporter:\n"\
"    @staticmethod\n"\
"    def fix_game_title(game_title: str) -> str:\n"\
"        return game_title.lower().replace(\" \", \"_\").replace(\"-\", \"_\")\n"\
"\n"\
"    @staticmethod\n"\
"    def export_game(\n"\
"        game_title: str,\n"\
"        archive_name: str,\n"\
"        engine_bin_dir: str,\n"\
"        project_dir: str,\n"\
"        temp_path: str,\n"\
"    ):\n"\
"        # Fix up game title\n"\
"        game_title = GameExporter.fix_game_title(game_title)\n"\
"        # Setup\n"\
"        temp_file_path = PurePath(temp_path) / \"tmp_build\"\n"\
"        FileUtils.remove_dir(temp_file_path.as_posix())\n"\
"        FileUtils.create_dir(temp_file_path.as_posix())\n"\
"        # Copy Project Files\n"\
"        FileUtils.copy_dir(project_dir, temp_file_path.as_posix())\n"\
"        # Copy Engine Bin Files\n"\
"        engine_bin_dir_path = PurePath(engine_bin_dir)\n"\
"        engine_assets_fonts_path = engine_bin_dir_path / \"assets\" / \"fonts\"\n"\
"        engine_assets_resources_path = engine_bin_dir_path / \"assets\" / \"resources\"\n"\
"        dest_asset_path = temp_file_path / \"assets\"\n"\
"        dest_asset_fonts_path = dest_asset_path / \"fonts\"\n"\
"        dest_asset_resources_path = dest_asset_path / \"resources\"\n"\
"        FileUtils.copy_dir(\n"\
"            engine_assets_fonts_path.as_posix(), dest_asset_fonts_path.as_posix()\n"\
"        )\n"\
"        FileUtils.copy_dir(\n"\
"            engine_assets_resources_path.as_posix(),\n"\
"            dest_asset_resources_path.as_posix(),\n"\
"        )\n"\
"        # Compile python source files\n"\
"        PythonCompiler.compile_dir(\n"\
"            directory=temp_file_path.as_posix(), delete_uncompiled_file=True, quiet=True\n"\
"        )\n"\
"        # Create pack file and remove all project files\n"\
"        project_pack_path = temp_file_path / f\"{game_title}.pck\"\n"\
"        ProjectArchiver.create_archive(\n"\
"            name=project_pack_path.as_posix(),\n"\
"            source_dir=temp_file_path.as_posix(),\n"\
"        )\n"\
"        # Remove all files except .pck\n"\
"        for file in FileUtils.get_dir_file_paths(\n"\
"            temp_file_path.as_posix(),\n"\
"            recursive=False,\n"\
"        ):\n"\
"            file_to_delete_path = PurePath(file)\n"\
"            if file_to_delete_path != project_pack_path:\n"\
"                FileUtils.delete_file(file_to_delete_path.as_posix())\n"\
"\n"\
"        # Get OS type by inferring from bin path\n"\
"        export_os_type = engine_bin_dir_path.as_posix().split(\"/\")[-1]\n"\
"        engine_binary_extension = \"\"\n"\
"        if export_os_type == \"windows\":\n"\
"            # Copy dlls\n"\
"            for file in FileUtils.get_dir_file_paths(\n"\
"                engine_bin_dir_path.as_posix(),\n"\
"                filter_func=lambda file: file.endswith(\".dll\"),\n"\
"            ):\n"\
"                file_path = Path(file)\n"\
"                dest_path = temp_file_path / file_path.name\n"\
"                FileUtils.copy_dir(file, dest_path.as_posix())\n"\
"            engine_binary_extension = \".exe\"\n"\
"        else:\n"\
"            # Copy embedded python files\n"\
"            embed_python_path = Path(f\"{engine_bin_dir}/embed_python\")\n"\
"            dest_path = temp_file_path / embed_python_path.name\n"\
"            FileUtils.copy_dir(embed_python_path.as_posix(), dest_path.as_posix())\n"\
"        # Copy Engine Binary\n"\
"        engine_binary_name = f\"crescent_engine{engine_binary_extension}\"\n"\
"        engine_binary_path = engine_bin_dir_path / engine_binary_name\n"\
"        engine_binary_dest_path = (\n"\
"            temp_file_path / f\"{game_title}{engine_binary_extension}\"\n"\
"        )\n"\
"        FileUtils.copy_file(\n"\
"            engine_binary_path.as_posix(), engine_binary_dest_path.as_posix()\n"\
"        )\n"\
"\n"\
"        # Create export zip\n"\
"        ProjectArchiver.create_archive(\n"\
"            name=archive_name,\n"\
"            source_dir=temp_file_path.as_posix(),\n"\
"        )\n"\
"\n"\
"        # Clean up\n"\
"        FileUtils.remove_dir(temp_file_path.as_posix())\n"\
"\n"
