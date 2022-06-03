#include "rbe_file_system_utils.h"

#include <unistd.h>

#include "logger.h"

bool rbe_fs_chdir(const char* dirPath) {
    if (chdir(dirPath) == 0) {
        return true;
    }
    rbe_logger_error("Failed to change directory to path '%s'", dirPath);
    return false;
}

void rbe_fs_print_cwd() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        rbe_logger_info("Current working directory: %s\n", cwd);
    } else {
        rbe_logger_error("Not able to get current working directory!");
    }
}
