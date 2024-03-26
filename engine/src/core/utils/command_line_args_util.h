#pragma once

#include <seika/defines.h>

#define CRE_COMMAND_LINE_FLAG_WORK_DIR "-d"
#define CRE_COMMAND_LINE_FLAG_INTERNAL_ASSETS_DIR "-ia"
#define CRE_COMMAND_LINE_FLAG_LOG_LEVEL "-l"

typedef struct CommandLineFlagResult {
    char workingDirOverride[256];
    char internalAssetsDirOverride[256];
    char logLevel[8];
    int32 flagCount;
} CommandLineFlagResult;

CommandLineFlagResult cre_command_line_args_parse(int32 argv, char** args);
