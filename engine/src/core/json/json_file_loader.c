#include "json_file_loader.h"

#include <cjson/cJSON.h>

#include "../utils/rbe_file_system_utils.h"
#include "../utils/logger.h"
#include "../memory/rbe_mem.h"

void cre_json_load_config_file(const char* filePath) {
    char* fileContent = rbe_fs_read_file_contents(filePath, NULL);

    cJSON* configJson = cJSON_Parse(fileContent);
    if (configJson != NULL) {
        // Get Name
        cJSON* nameJson = cJSON_GetObjectItemCaseSensitive(configJson, "name");
        if (cJSON_IsString(nameJson) && (nameJson->valuestring != NULL)) {
            rbe_logger_debug("Printing json project name '%s'", nameJson->valuestring);
        }
        // Window Width
        cJSON* windowWidthJson = cJSON_GetObjectItemCaseSensitive(configJson, "window_width");
        if (cJSON_IsNumber(windowWidthJson)) {
            rbe_logger_debug("Printing json project window width '%d'", (int) windowWidthJson->valuedouble);
        }
        // Window Height
        cJSON* windowHeightJson = cJSON_GetObjectItemCaseSensitive(configJson, "window_height");
        if (cJSON_IsNumber(windowHeightJson)) {
            rbe_logger_debug("Printing json project window height '%d'", (int) windowHeightJson->valuedouble);
        }
    }

    cJSON_Delete(configJson);
    RBE_MEM_FREE(fileContent);
}

void cre_json_load_scene_file(const char* filePath) {}
