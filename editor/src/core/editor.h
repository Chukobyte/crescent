#pragma once

#include <SDL2/SDL.h>
#include <glad/glad.h>

#include "SquidTasks/TaskManager.h"

class Editor {
  public:
    bool Initialize();
    void Update();
    bool IsRunning() const;
    void Shutdown();

  private:
    int windowWidth = 800;
    int windowHeight = 600;

    TaskManager mainTasks;

    bool InitializeSDL();
    bool InitializeImGui();
    void ProcessInput();
    void ProcessWindows();
    void Render();
    void Flush();
};
