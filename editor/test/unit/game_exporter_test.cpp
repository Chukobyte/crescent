#include <catch.hpp>

#include "../../src/core/game_exporter.h"
#include "../../src/core/editor_context.h"

TEST_CASE("Game Exporter", "[game_exporter]") {
    SECTION("Test Export") {
        GameExporter::Export({
            .gameTitle = "Test Game",
            .exportArchivePath = "test_game_windows.zip",
            .projectPath = "test_games/pkpy_test",
            .binPath = "bin/" + std::string(CRESCENT_OS_BIN_DIR),
            .tempPath = ".",
            .platform = GameExporter::GetPlatformFromString(CRESCENT_OS_BIN_DIR)
        });
        REQUIRE(true);
    }
}
