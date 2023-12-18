#include <catch.hpp>

#include "../../src/core/game_exporter.h"

TEST_CASE("Game Exporter", "[game_exporter]") {
    SECTION("Test Export") {
        GameExporter::Export({
            .gameTitle = "Test Game",
            .exportArchivePath = "/test",
            .projectPath = "test_games/pkpy_test",
            .binPath = "/test",
            .tempPath = ".",
        });
        REQUIRE(true);
    }
}