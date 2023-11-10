#include "mouse.h"
#include "../memory/se_mem.h"

static SEMouse* globalMouse = NULL;

SEMouse* se_mouse_get() {
    if (globalMouse == NULL) {
        globalMouse = SE_MEM_ALLOCATE(SEMouse);
        globalMouse->position.x = 0.0f;
        globalMouse->position.y = 0.0f;
    }
    return globalMouse;
}

void se_mouse_clear() {
    if (globalMouse != NULL) {
        SE_MEM_FREE(globalMouse);
        globalMouse = NULL;
    }
}
