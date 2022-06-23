#include "rbe_string_util.h"

#include <stdint.h>
#include <string.h>

#include "../memory/rbe_mem.h"

char* rbe_strdup(const char* string) {
    size_t string_length = strlen(string) + 1;
    void* new_string = RBE_MEM_ALLOCATE_SIZE(string_length);

    if (new_string == NULL) {
        return NULL;
    }

    return (char*) memcpy(new_string, string, string_length);
}
