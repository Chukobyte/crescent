#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

// Change directory
bool rbe_fs_chdir(const char* dirPath);

// Print current working directory
void rbe_fs_print_cwd();

size_t rbe_fs_get_file_size(const char* filePath);

char* rbe_fs_read_file_contents(const char* filePath, size_t* sz);

#ifdef __cplusplus
}
#endif
