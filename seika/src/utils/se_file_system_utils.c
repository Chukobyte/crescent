#include "se_file_system_utils.h"

#ifdef _WIN32
#include <windows.h>
#include <corecrt.h>
#endif

#include <stdio.h>
#include <string.h>

#if defined(_MSC_VER)
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>

#endif


#include "logger.h"
#include "../memory/se_mem.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for fopen
#endif

void se_fs_get_cwd_array(char* array, size_t size) {
    if (getcwd(array, size) != NULL) {
        return;
    }
}

bool se_fs_chdir(const char* dirPath) {
#define CWD_MAX_BUFFER_SIZE 256
    char currentWorkingPath[CWD_MAX_BUFFER_SIZE];
    se_fs_get_cwd_array(currentWorkingPath, CWD_MAX_BUFFER_SIZE);
    if (strcmp(currentWorkingPath, dirPath) == 0) {
        se_logger_warn("Attempting to change to the same directory at path '%s'", currentWorkingPath);
        return false;
    }
    if (chdir(dirPath) == 0) {
        return true;
    }
    se_logger_error("Failed to change directory to path '%s'", dirPath);
    return false;
}

char* se_fs_get_cwd() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return strdup(cwd);
    }
    return NULL;
}

void se_fs_print_cwd() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        se_logger_info("Current working directory: %s\n", cwd);
    } else {
        se_logger_error("Not able to get current working directory!");
    }
}

size_t se_fs_get_file_size(const char* filePath) {
#ifdef _WIN32
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        se_logger_error("Error invalid handle value when getting file size at path '%s'", filePath);
        return 0;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size)) {
        CloseHandle(hFile);
        se_logger_error("Error getting file size at path '%s'", filePath);
        return 0;
    }

    CloseHandle(hFile);
    return (size_t) size.QuadPart;
#else
    struct stat st;
    stat(filePath, &st);
    return (size_t) st.st_size;
#endif
}

char* se_fs_read_file_contents(const char* filePath, size_t* sz) {
    char* buffer = NULL;
    FILE* fp = fopen(filePath, "rb");
    size_t readSize = 0;
    if (fp) {
        readSize = se_fs_get_file_size(filePath);
        // Update buffer
        buffer = (char*) SE_MEM_ALLOCATE_SIZE(readSize + 1);
        if (buffer != NULL) {
            fread(buffer, 1, readSize, fp);
            buffer[readSize] = '\0';
        }
        fclose(fp);
    }
    if (sz != NULL) {
        *sz = readSize;
    }
    return buffer;
}

bool se_fs_does_file_exist(const char* filePath) {
    FILE* fp = fopen(filePath, "r");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}