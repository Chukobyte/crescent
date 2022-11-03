#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SE_LOG_BUFFER_SIZE 512

enum LogLevel {
    LogLevel_DEBUG = 0,
    LogLevel_INFO = 1,
    LogLevel_WARN = 2,
    LogLevel_ERROR = 3
};

void se_logger_set_level(enum LogLevel level);
void se_logger_error(const char* fmt, ...);
void se_logger_warn(const char* fmt, ...);
void se_logger_info(const char* fmt, ...);
void se_logger_debug(const char* fmt, ...);
void se_logger_print_err(const char* fmt, ...);

const char* se_logger_get_log_level_string(enum LogLevel level);
enum LogLevel se_logger_get_log_level_enum(const char* level);

#ifdef __cplusplus
}
#endif
