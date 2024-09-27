#include <stdlib.h>

// #include <SDL3/SDL_main.h>

#include <pocketpy.h>

#include "core/core.h"


int main(int argv, char** args) {
    // if (!cre_initialize(argv, args)) {
    //     return EXIT_FAILURE;
    // }
    //
    // while (cre_is_running()) {
    //     cre_update();
    // }
    //
    // return cre_shutdown();

    py_initialize();

    py_Ref aModule = py_newmodule("a");

    py_exec("class A:\n def start(self): print('start')\n", "a.py", EXEC_MODE, aModule);

    py_exec("from a import A\n\na = A()", "main.py", EXEC_MODE, NULL);

    py_Ref self = py_getglobal(py_name("a"));
    if (py_getattr(self, py_name("start"))) {
        py_push(py_retval());
        py_pushnil();
        py_vectorcall(0, 0);
    }

    py_finalize();
    return EXIT_SUCCESS;
}
