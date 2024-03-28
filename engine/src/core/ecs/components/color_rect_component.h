#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <seika/math/math.h>

typedef struct ColorRectComponent {
    SkaSize2D size;
    SkaColor color;
} ColorRectComponent;

ColorRectComponent* color_rect_component_create();
void color_rect_component_delete(ColorRectComponent* colorRectComponent);
ColorRectComponent* color_rect_component_copy(const ColorRectComponent* colorRectComponent);

#ifdef __cplusplus
}
#endif
