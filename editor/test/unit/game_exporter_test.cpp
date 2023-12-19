#include <catch.hpp>

#include "../../src/core/game_exporter.h"
#include "../../src/core/editor_context.h"

TEST_CASE("Game Exporter", "[game_exporter]") {
    SECTION("Test Export") {
        GameExporter::Export({
            .gameTitle = "Test Game",
            .exportArchivePath = "/test",
            .projectPath = "test_games/pkpy_test",
            .binPath = "bin/" + std::string(CRESCENT_OS_BIN_DIR),
            .tempPath = ".",
            .platform = GameExporter::Platform::Windows
        });
        REQUIRE(true);
    }
}
