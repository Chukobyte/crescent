#include "game_exporter.h"

#include "../engine/src/core/scripting/python/cre_py.h"

#include "scripting/python/crescent_editor_api_source.h"

namespace {
void GameExporterInitialize() {
    static bool hasInitialized = false;
    if (!hasInitialized) {
        cre_py_import_module_source("game_exporter", CRE_PY_EDITOR_API_GAME_EXPORTER_SOURCE);
    }
}
} // namespace

void GameExporter::Export(const GameExporter::ExportProperties& props) {
    GameExporterInitialize();
    cre_py_export_game_project(
        props.gameTitle.c_str(),
        props.exportArchivePath.c_str(),
        props.binPath.c_str(),
        props.projectPath.c_str(),
        props.tempPath.c_str()
    );
}
