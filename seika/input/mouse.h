#pragma once

#include "../math/se_math.h"

typedef struct SEMouse {
    SEVector2 position;
} SEMouse;

SEMouse* se_mouse_get();
void se_mouse_clear();
