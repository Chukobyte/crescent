#include "core/editor.h"
#include "../engine/src/core/utils/logger.h"

#include "SquidTasks/Task.h"

using namespace Squid;

Task<> TestTask() {
    TASK_NAME(__FUNCTION__ );

    int number = 0;

    while (true) {
//        rbe_logger_info("number = %d", number++);
        co_await Suspend();
    }
}

int main(int argv, char** args) {
    Editor editor;
    if (!editor.Initialize()) {
        return -1;
    }

    auto TestTaskHandle = TestTask();

    while (editor.IsRunning()) {
        editor.Update();
        TestTaskHandle.Resume();
    }

    editor.Shutdown();
    return 0;
}
