#include "command_line_args_util.h"

#include <string.h>

#include "logger.h"

CommandLineFlagResult rbe_command_line_args_parse(int argv, char** args) {
    CommandLineFlagResult flagResult;
    memset(flagResult.workingDirOverride, 0, 128);
    flagResult.flagCount = 0;
    if (argv <= 1) {
        return flagResult;
    }
    for (int argumentIndex = 1; argumentIndex < argv; argumentIndex++) {
        // Can process single argument if needed
        const char* argument = args[argumentIndex];
        rbe_logger_debug("command line argument = '%s'", argument);
        // Process arg value
        const int nextArgumentIndex = argumentIndex + 1;
        if (nextArgumentIndex >= argv) {
            break;
        }
        if (strcmp(argument, RBE_COMMAND_LINE_FLAG_WORK_DIR) == 0) {
            const char* workingDirectoryOverride = args[nextArgumentIndex];
            strcpy(flagResult.workingDirOverride, workingDirectoryOverride);
            rbe_logger_debug("working directory override = '%s'", flagResult.workingDirOverride);
            argumentIndex++;
        }
    }
    return flagResult;
}
