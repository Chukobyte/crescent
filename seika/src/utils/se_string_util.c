#include "se_string_util.h"

#include <stdint.h>
#include <string.h>

#include "../memory/se_mem.h"

char* se_strdup(const char* string) {
    size_t string_length = strlen(string) + 1;
    void* new_string = SE_MEM_ALLOCATE_SIZE(string_length);

    if (new_string == NULL) {
        return NULL;
    }

    return (char*) memcpy(new_string, string, string_length);
}

const char* se_bool_to_string(bool value) {
    return value == true ? "true" : "false";
}
