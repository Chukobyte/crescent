#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../seika/src/math/se_math.h"

typedef struct ColorRectComponent {
    Size2D size;
    Color color;
} ColorRectComponent;

ColorRectComponent* color_rect_component_create();
void color_rect_component_delete(ColorRectComponent* colorRectComponent);
ColorRectComponent* color_rect_component_copy(const ColorRectComponent* colorRectComponent);

#ifdef __cplusplus
}
#endif
