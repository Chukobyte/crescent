#include "logger.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy and strncat
#endif

static SELogLevel currentLogLevel = SELogLevel_ERROR;

void se_logger_set_level(SELogLevel level) {
    currentLogLevel = level;
}

void se_logger_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= SELogLevel_ERROR) {
        char str[SE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[ERROR] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vfprintf(stderr, str, args);
    }
    va_end(args);
}

void se_logger_warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= SELogLevel_WARN) {
        char str[SE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[WARN] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vprintf(str, args);
    }
    va_end(args);
}

void se_logger_info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= SELogLevel_INFO) {
        char str[SE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[INFO] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vprintf(str, args);
    }
    va_end(args);
}

void se_logger_debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    if (currentLogLevel <= SELogLevel_DEBUG) {
        char str[SE_LOG_BUFFER_SIZE];
        const char* logLevelText = "[DEBUG] ";
        strcpy(str, logLevelText);
        strncat(str, fmt, (sizeof(str) - strlen(str) - 1) );
        strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
        vprintf(str, args);
    }
    va_end(args);
}

void se_logger_print_err(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char str[SE_LOG_BUFFER_SIZE];
    memset(str, 0, SE_LOG_BUFFER_SIZE);
    strncat(str,  fmt, (sizeof(str) - strlen(str) - 1) );
    strncat(str, "\n", (sizeof(str) - strlen(str) - 1) );
    vfprintf(stderr, str, args);
    va_end(args);
}

const char* se_logger_get_log_level_string(SELogLevel level) {
    switch (level) {
    case SELogLevel_DEBUG:
        return "debug";
    case SELogLevel_INFO:
        return "info";
    case SELogLevel_WARN:
        return "warn";
    case SELogLevel_ERROR:
        return "error";
    }
    return NULL;
}

SELogLevel se_logger_get_log_level_enum(const char* level) {
    if (strcmp(level, "debug") == 0) {
        return SELogLevel_DEBUG;
    } else if (strcmp(level, "info") == 0) {
        return SELogLevel_INFO;
    } else if (strcmp(level, "warn") == 0) {
        return SELogLevel_WARN;
    } else if (strcmp(level, "error") == 0) {
        return SELogLevel_ERROR;
    }
    return SELogLevel_ERROR;
}
