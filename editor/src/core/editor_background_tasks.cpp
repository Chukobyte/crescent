#include "editor_background_tasks.h"

#include "asset_browser.h"
#include "editor_context.h"

using namespace Squid;

Task<> EditorBackgroundTasks::Main(TaskManager* taskManager) {
    TASK_NAME("EditorBackgroundTasks::Main");

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
            projectStateTask = taskManager->Run(AssetBrowser::Get()->UpdateFileSystemCache());
            break;
        }
        }
        co_await WaitUntil([editorContext, &projectState]() -> bool { return editorContext->projectState != projectState; });
    }
}
