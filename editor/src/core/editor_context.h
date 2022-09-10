#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "utils/singleton.h"
#include "utils/enum_class_utils.h"

enum class EditorProjectState : int {
    ProjectManager = 0,
    OpenedProject = 1,
};
//GENERATE_ENUM_CLASS_OPERATORS(EditorProjectState)

class EditorContext : public Singleton<EditorContext> {
  public:
    EditorContext(singleton);

    std::string GetEngineBinaryPath() const;

    static float Time();

    bool isRunning = false;
    EditorProjectState projectState = EditorProjectState::ProjectManager;
    // Rendering
    SDL_Window* window = nullptr;
    const SDL_WindowFlags windowFlags = (SDL_WindowFlags)(
                                            SDL_WINDOW_OPENGL
                                            | SDL_WINDOW_RESIZABLE
                                            | SDL_WINDOW_ALLOW_HIGHDPI
                                        );
    SDL_GLContext openGLContext;
    std::string initialDir;
};