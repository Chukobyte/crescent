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
        // Window Width
        cJSON* resolutionWidthJson = cJSON_GetObjectItemCaseSensitive(configJson, "resolution_width");
        if (cJSON_IsNumber(resolutionWidthJson)) {
            rbe_logger_debug("Printing json project resolution width '%d'", (int) resolutionWidthJson->valuedouble);
        }
        // Window Height
        cJSON* resolutionHeightJson = cJSON_GetObjectItemCaseSensitive(configJson, "resolution_height");
        if (cJSON_IsNumber(resolutionHeightJson)) {
            rbe_logger_debug("Printing json project resolution height '%d'", (int) resolutionHeightJson->valuedouble);
        }
        // Target FPS
        cJSON* targetFPSJson = cJSON_GetObjectItemCaseSensitive(configJson, "target_fps");
        if (cJSON_IsNumber(targetFPSJson)) {
            rbe_logger_debug("Printing json project target fps '%d'", (int) targetFPSJson->valuedouble);
        }
        // Initial Node Path
        cJSON* initialNodePathJson = cJSON_GetObjectItemCaseSensitive(configJson, "initial_node_path");
        if (cJSON_IsString(initialNodePathJson) && (initialNodePathJson->valuestring != NULL)) {
            rbe_logger_debug("Printing json initial node path '%s'", initialNodePathJson->valuestring);
        }
        // Colliders Visible
        cJSON* collidersVisibleJson = cJSON_GetObjectItemCaseSensitive(configJson, "colliders_visible");
        if (cJSON_IsBool(collidersVisibleJson)) {
            rbe_logger_debug("Printing json project colliders visible '%s'", cJSON_IsTrue(collidersVisibleJson) ? "true": "false");
        }
    }

    cJSON_Delete(configJson);
    RBE_MEM_FREE(fileContent);
}

void cre_json_load_scene_file(const char* filePath) {}
