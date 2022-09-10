#pragma once

#define RBE_COMMAND_LINE_FLAG_WORK_DIR "-d"
#define RBE_COMMAND_LINE_FLAG_INTERNAL_ASSETS_DIR "-ia"

typedef struct CommandLineFlagResult {
    char workingDirOverride[128];
    char internalAssetsDirOverride[128];
    int flagCount;
} CommandLineFlagResult;

CommandLineFlagResult rbe_command_line_args_parse(int argv, char** args);
