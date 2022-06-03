#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#define RBE_LOG_BUFFER_SIZE 512

enum LogLevel {
    LogLevel_DEBUG = 0,
    LogLevel_INFO = 1,
    LogLevel_WARN = 2,
    LogLevel_ERROR = 3
};

void rbe_logger_set_level(enum LogLevel level);
void rbe_logger_error(const char* fmt, ...);
void rbe_logger_warn(const char* fmt, ...);
void rbe_logger_info(const char* fmt, ...);
void rbe_logger_debug(const char* fmt, ...);
void rbe_logger_print_err(const char* fmt, ...);

#ifdef __cplusplus
}
#endif
