#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy and strncat
#endif

static enum LogLevel currentLogLevel = LogLevel_ERROR;

void rbe_logger_set_level(enum LogLevel level) {
    currentLogLevel = level;
}

void rbe_logger_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= LogLevel_ERROR) {
        char str[RBE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[ERROR] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vfprintf(stderr, str, args);
    }
    va_end(args);
}

void rbe_logger_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= LogLevel_WARN) {
        char str[RBE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[WARN] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vprintf(str, args);
    }
    va_end(args);
}

void rbe_logger_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= LogLevel_INFO) {
        char str[RBE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[INFO] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vprintf(str, args);
    }
    va_end(args);
}

void rbe_logger_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= LogLevel_DEBUG) {
        char str[RBE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[DEBUG] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vprintf(str, args);
    }
    va_end(args);
}

void rbe_logger_print_err(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char str[RBE_LOG_BUFFER_SIZE];
    memset(str, 0, RBE_LOG_BUFFER_SIZE);
    strncat(str,  fmt, (sizeof(str) - strlen(str) - 1) );
    strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
    vfprintf(stderr, str, args);
    va_end(args);
}
