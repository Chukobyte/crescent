#include "command_line_args_util.h"

#include <string.h>

#include <seika/logger.h>
#include <seika/string.h>
#include <seika/assert.h>

#define CRE_DIR_OVERRIDE_CAPACITY  128
#define CRE_LOG_LEVEL_CAPACITY  8

CommandLineFlagResult cre_command_line_args_parse(int32 argv, char** args) {
    SKA_ASSERT_FMT(argv > 0, "Starting arguments are 0 or less!?");
    CommandLineFlagResult flagResult;
    memset(flagResult.workingDirOverride, 0, CRE_DIR_OVERRIDE_CAPACITY);
    memset(flagResult.internalAssetsDirOverride, 0, CRE_DIR_OVERRIDE_CAPACITY);
    memset(flagResult.logLevel, 0, CRE_LOG_LEVEL_CAPACITY);
    flagResult.flagCount = 0;
    if (argv <= 1) {
        ska_logger_debug("No command line arguments passed!  single arg = '%s'", args[0]);
        return flagResult;
    }
    for (int32 argumentIndex = 1; argumentIndex < argv; argumentIndex++) {
        // Can process single argument if needed
        const char* argument = args[argumentIndex];
        // Process arg value
        const int32 nextArgumentIndex = argumentIndex + 1;
        if (nextArgumentIndex >= argv) {
            break;
        }
        if (strcmp(argument, CRE_COMMAND_LINE_FLAG_WORK_DIR) == 0) {
            const char* workingDirectoryOverride = args[nextArgumentIndex];
            ska_strcpy(flagResult.workingDirOverride, workingDirectoryOverride);
            argumentIndex++;
            flagResult.flagCount++;
        } else if (strcmp(argument, CRE_COMMAND_LINE_FLAG_INTERNAL_ASSETS_DIR) == 0) {
            const char* internalAssetsDirectoryOverride = args[nextArgumentIndex];
            ska_strcpy(flagResult.internalAssetsDirOverride, internalAssetsDirectoryOverride);
            argumentIndex++;
            flagResult.flagCount++;
        } else if (strcmp(argument, CRE_COMMAND_LINE_FLAG_LOG_LEVEL) == 0) {
            const char* logLevelOverride = args[nextArgumentIndex];
            ska_strcpy(flagResult.logLevel, logLevelOverride);
            argumentIndex++;
            flagResult.flagCount++;
        }
    }
    return flagResult;
}
