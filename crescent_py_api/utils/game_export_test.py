from crescent_game_exporter import GameExporter
from helper import Helper


# GameExporter.export_game(
#     game_title="Card Test",
#     archive_name="C:\\ProjectWorkspace\\C_Lang\\cmake_rbe\\temp\\card_test.zip",
#     engine_bin_dir="C:\\ProjectWorkspace\\C_Lang\\cmake_rbe\\cmake-build-debug-visual-studio\\bin",
#     project_dir="C:\\ProjectWorkspace\\C_Lang\\cmake_rbe\\test_games\\card_test",
#     temp_path="C:\\ProjectWorkspace\\C_Lang\\cmake_rbe\\temp",
# )

py_source = Helper.convert_py_file_to_source_c_string(
    "C:/ProjectWorkspace/C_Lang/cmake_rbe/crescent_py_api/utils/source_importer.py"
)
print(f"py_source = {py_source}")
