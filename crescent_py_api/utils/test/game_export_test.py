from utils.game_exporter import GameExporter


GameExporter.export_game(
    game_title="Card Test",
    archive_name="C:\\ProjectWorkspace\\C_Lang\\cmake_rbe\\temp\\card_test.zip",
    engine_bin_dir="C:\\ProjectWorkspace\\C_Lang\\cmake_rbe\\cmake-build-debug-visual-studio\\bin",
    project_dir="/",
    temp_path="/temp",
)
