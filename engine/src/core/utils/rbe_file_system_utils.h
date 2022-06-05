#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// Change directory
bool rbe_fs_chdir(const char* dirPath);
// Print current working directory
void rbe_fs_print_cwd();

#ifdef __cplusplus
}
#endif
