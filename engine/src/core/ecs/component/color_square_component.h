#pragma once

#include "../../math/rbe_math.h"

typedef struct ColorSquareComponent {
    Size2D size;
    Color color;
} ColorSquareComponent;

ColorSquareComponent* color_square_component_create();
