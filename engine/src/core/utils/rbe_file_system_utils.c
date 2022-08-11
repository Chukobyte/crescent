#include "rbe_file_system_utils.h"

#ifdef _WIN32
#include "windows.h"
#include <corecrt.h>
#endif

#include <stdio.h>

#if defined(_MSC_VER)
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>

#endif


#include "logger.h"
#include "../memory/rbe_mem.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for fopen
#endif

bool rbe_fs_chdir(const char* dirPath) {
    if (chdir(dirPath) == 0) {
        return true;
    }
    rbe_logger_error("Failed to change directory to path '%s'", dirPath);
    return false;
}

char* rbe_fs_get_cwd() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return strdup(cwd);
    }
    return NULL;
}

void rbe_fs_print_cwd() {
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        rbe_logger_info("Current working directory: %s\n", cwd);
    } else {
        rbe_logger_error("Not able to get current working directory!");
    }
}

size_t rbe_fs_get_file_size(const char* filePath) {
#ifdef _WIN32
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        rbe_logger_error("Error invalid handle value when getting file size at path '%s'", filePath);
        return 0;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(hFile, &size)) {
        CloseHandle(hFile);
        rbe_logger_error("Error getting file size at path '%s'", filePath);
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

char* rbe_fs_read_file_contents(const char* filePath, size_t* fileSize) {
    char* buffer = NULL;
    FILE* fp = fopen(filePath, "rb");
    size_t readSize = 0;
    if (fp) {
        readSize = rbe_fs_get_file_size(filePath);
        // Update buffer
        buffer = (char*) RBE_MEM_ALLOCATE_SIZE(readSize + 1);
        if (buffer != NULL) {
            fread(buffer, 1, readSize, fp);
            buffer[readSize] = '\0';
        }
        fclose(fp);
    }
    if (fileSize != NULL) {
        *fileSize = readSize;
    }
    return buffer;
}
