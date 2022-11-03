#pragma once

#define CRE_COMMAND_LINE_FLAG_WORK_DIR "-d"
#define CRE_COMMAND_LINE_FLAG_INTERNAL_ASSETS_DIR "-ia"
#define CRE_COMMAND_LINE_FLAG_LOG_LEVEL "-l"

typedef struct CommandLineFlagResult {
    char workingDirOverride[128];
    char internalAssetsDirOverride[128];
    char logLevel[8];
    int flagCount;
} CommandLineFlagResult;

CommandLineFlagResult cre_command_line_args_parse(int argv, char** args);
