#include "editor.h"

#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include "../engine/src/core/utils/logger.h"
#include "../engine/src/core/scripting/python/rbe_py.h"

#include "editor_context.h"
#include "editor_background_tasks.h"
#include "scene/scene_manager.h"
#include "color.h"
#include "asset_manager.h"
#include "ui/imgui/imgui_handler.h"
#include "ui/imgui/imgui_styler.h"
#include "utils/file_system_helper.h"

static EditorContext* editorContext = EditorContext::Get();

bool Editor::Initialize() {
    rbe_logger_set_level(LogLevel_DEBUG);

    if (!InitializeSDL()) {
        return false;
    }
    if (!InitializeImGui()) {
        return false;
    }

    // Initialize Python Instance
    rbe_py_initialize();

    // Initialize Asset Manager
    AssetManager::Get()->Initialize();

    editorContext->initialDir = FileSystemHelper::GetCurrentDir();
    editorContext->isRunning = true;
    rbe_logger_info("Crescent Engine Editor has started!");

    // Load editor setting or create a new file if it doesn't exist
    if (!editorContext->settings.LoadSettings()) {
        editorContext->settings.SaveSettings();
    }

    mainTasks.RunManaged(EditorBackgroundTasks::Main(&mainTasks));
    return true;
}

void Editor::Update() {
    ProcessInput();
    Render();
    mainTasks.Update();
    Flush();
}

bool Editor::InitializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        rbe_logger_error(SDL_GetError());
        return false;
    }

    // OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    // Rendering
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    editorContext->window = SDL_CreateWindow(
                                "Crescent Engine Editor",
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                windowWidth,
                                windowHeight,
                                editorContext->windowFlags
                            );

    SDL_SetWindowMinimumSize(editorContext->window, windowWidth, windowHeight);

    editorContext->openGLContext = SDL_GL_CreateContext(editorContext->window);
    SDL_GL_MakeCurrent(editorContext->window, editorContext->openGLContext);

    // enable VSync
    SDL_GL_SetSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        rbe_logger_error("Couldn't initialize glad!");
        return false;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    return true;
}

bool Editor::InitializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForOpenGL(editorContext->window, editorContext->openGLContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    ImGuiStyler::ApplyStyle(ImGuiStyler::Style::Crescent);

    return true;
}

void Editor::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch(event.type) {
        case SDL_QUIT:
            editorContext->isRunning = false;
            break;
        default:
            break;
        }
    }
}

void Editor::ProcessWindows() {
    ImGuiHandler::Process();
}

void Editor::Render() {
    static EditorColor backgroundColor = EditorColor::CreateNormalizedColor(22.0f, 22.0f, 22.0f);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(editorContext->window);
    ImGui::NewFrame();

    ProcessWindows();

    // rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(editorContext->window);
}

void Editor::Flush() {
    static SceneManager* sceneManager = SceneManager::Get();
    sceneManager->FlushQueuedForDeletionNodes();
}

bool Editor::IsRunning() const {
    return editorContext->isRunning;
}

void Editor::Shutdown() {
    mainTasks.KillAllTasks();
    // IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    // SDL
    SDL_GL_DeleteContext(editorContext->openGLContext);
    SDL_DestroyWindow(editorContext->window);
    SDL_Quit();

    rbe_py_finalize();

    rbe_logger_info("Crescent Engine Editor has been shutdown!");
}
