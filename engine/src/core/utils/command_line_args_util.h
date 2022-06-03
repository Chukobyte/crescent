#pragma once

#define RBE_COMMAND_LINE_FLAG_WORK_DIR "-d"

typedef struct CommandLineFlagResult {
    char workingDirOverride[128];
    int flagCount;
} CommandLineFlagResult;

CommandLineFlagResult rbe_command_line_args_parse(int argv, char** args);
