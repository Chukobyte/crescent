#pragma once

#include "../../math/rbe_math.h"

typedef struct ColorSquareComponent {
    Size2D size;
    Color color;
} ColorSquareComponent;

ColorSquareComponent* color_square_component_create();
void color_square_component_delete(ColorSquareComponent* colorSquareComponent);
ColorSquareComponent* color_square_component_copy(const ColorSquareComponent* colorSquareComponent);
