#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SE_LOG_BUFFER_SIZE 512

typedef enum SELogLevel {
    SELogLevel_DEBUG = 0,
    SELogLevel_INFO = 1,
    SELogLevel_WARN = 2,
    SELogLevel_ERROR = 3
} SELogLevel;

void se_logger_set_level(SELogLevel level);
void se_logger_error(const char* fmt, ...);
void se_logger_warn(const char* fmt, ...);
void se_logger_info(const char* fmt, ...);
void se_logger_debug(const char* fmt, ...);
void se_logger_print_err(const char* fmt, ...);

const char* se_logger_get_log_level_string(SELogLevel level);
SELogLevel se_logger_get_log_level_enum(const char* level);

#ifdef __cplusplus
}
#endif
