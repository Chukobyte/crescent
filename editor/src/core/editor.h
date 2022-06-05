#pragma once

#include <SDL2/SDL.h>
#include <glad/glad.h>

class Editor {
  public:
    bool Initialize();
    void Update();
    bool IsRunning() const;
    void Shutdown();

  private:
    int windowWidth = 800;
    int windowHeight = 600;

    bool InitializeSDL();
    bool InitializeImGui();
    void ProcessInput();
    void ProcessWindows();
    void Render();
};
