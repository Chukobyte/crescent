#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "editor_settings.h"
#include "utils/singleton.h"
#include "utils/enum_class_utils.h"

#define EDITOR_ENGINE_NAME "crescent_engine"

#ifdef _WIN32
#define EDITOR_ENGINE_EXTENSION ".exe"
#else
#define EDITOR_ENGINE_EXTENSION ""
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

    std::string GetEngineBinPath() const;
    std::string GetEngineBinPathByOS(const std::string& osType) const;
    std::string GetEngineBinaryPath() const;
    std::string GetEngineBinaryProgramArgs() const;
    std::string GetProjectExportPath() const;
    std::string GetProjectTempPath() const;

    // Returns a time used for ticking of seconds
    static float Time();

    bool isRunning = false;
    EditorSettings settings;
    EditorProjectState projectState = EditorProjectState::ProjectManager;
    // Rendering
    SDL_Window* window = nullptr;
    const SDL_WindowFlags windowFlags = (SDL_WindowFlags)(
                                            SDL_WINDOW_OPENGL
                                            | SDL_WINDOW_RESIZABLE
                                            | SDL_WINDOW_ALLOW_HIGHDPI
                                        );
    SDL_GLContext openGLContext = nullptr;
    std::string initialDir;
};
