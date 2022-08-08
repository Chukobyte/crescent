#include "core/editor.h"

int main(int argv, char** args) {
    Editor editor;
    if (!editor.Initialize()) {
        return -1;
    }

    while (editor.IsRunning()) {
        editor.Update();
    }

    editor.Shutdown();
    return 0;
}
