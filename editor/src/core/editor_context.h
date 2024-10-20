#pragma once

#include <string>

#include <SDL3/SDL.h>

#include "editor_settings.h"
#include "utils/singleton.h"
#include "utils/enum_class_utils.h"

#define EDITOR_ENGINE_NAME "crescent_engine"

#ifdef _WIN32
#define EDITOR_ENGINE_EXTENSION ".exe"
#else
#define EDITOR_ENGINE_EXTENSION ""
#endif

#ifdef _WIN32
#define CRESCENT_OS_BIN_DIR "windows"
#elif __APPLE__
#define CRESCENT_OS_BIN_DIR "macosx"
#else
#define CRESCENT_OS_BIN_DIR "linux"
#endif

#define EDITOR_ENGINE_BINARY_NAME EDITOR_ENGINE_NAME EDITOR_ENGINE_EXTENSION

enum class EditorProjectState : int {
    ProjectManager = 0,
    OpenedProject = 1,
};
//GENERATE_ENUM_CLASS_OPERATORS(EditorProjectState)

class EditorContext : public Singleton<EditorContext> {
  public:
    EditorContext(singleton);

    [[nodiscard]] std::string GetEngineBinRoot() const;
    [[nodiscard]] std::string GetEngineBinPath() const;
    [[nodiscard]] std::string GetEngineBinPathByOS(const std::string& osType) const;
    [[nodiscard]] std::string GetEngineBinaryPath() const;
    [[nodiscard]] std::string GetEngineBinaryProgramArgs() const;
    [[nodiscard]] std::string GetProjectTempPath() const;
    [[nodiscard]] std::string GetEngineDefaultFontPath() const;


    // Returns a time used for ticking of seconds
    static float Time();

    bool isRunning = false;
    EditorSettings settings;
    EditorProjectState projectState = EditorProjectState::ProjectManager;
    // Rendering
    SDL_Window* window = nullptr;
    const uint32_t windowFlags = SDL_WINDOW_OPENGL |SDL_WINDOW_RESIZABLE;
    SDL_GLContext openGLContext = nullptr;
    std::string initialDir;

    // stats
    float currentFPS = 0.0f;
    float editorDeltaTime = 0.0f;
    float simDeltaTime = 0.0f;
};
