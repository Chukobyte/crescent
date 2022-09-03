#include "editor_background_tasks.h"

#include "asset_browser.h"
#include "editor_context.h"

using namespace Squid;

namespace {
Task<> ProjectManagerTasks() {
    while (true) {
        co_await Suspend();
    }
}

Task<> OpenedProjectTasks() {
    while (true) {
        co_await Suspend();
    }
}
} // namespace

Task<> EditorBackgroundTasks::Main(TaskManager* taskManager) {
    EditorContext* editorContext = EditorContext::Get();
    EditorProjectState projectState = EditorProjectState::ProjectManager;
    while (true) {
        projectState = editorContext->projectState;
        TaskHandle<> projectStateTask;

        switch (projectState) {
        case EditorProjectState::ProjectManager: {
            break;
        }
        case EditorProjectState::OpenedProject: {
            projectStateTask = taskManager->Run(AssetBrowser::Get()->UpdateFileAndDirectoryCache());
            break;
        }
        }
        co_await WaitUntil([editorContext, projectState] { return editorContext->projectState != projectState; });
    }
}
