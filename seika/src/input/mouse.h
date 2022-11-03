#pragma once

#include "../seika/src/math/se_math.h"

typedef struct SEMouse {
    Vector2 position;
} SEMouse;

SEMouse* se_mouse_get();
void se_mouse_clear();
