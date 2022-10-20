#include "color_rect_component.h"

#include <string.h>

#include "../seika/src/memory/se_mem.h"

ColorRectComponent* color_rect_component_create() {
    ColorRectComponent* colorRectComponent = SE_MEM_ALLOCATE(ColorRectComponent);
    colorRectComponent->size.w = 32.0f;
    colorRectComponent->size.h = 32.0f;
    colorRectComponent->color.r = 1.0f;
    colorRectComponent->color.g = 1.0f;
    colorRectComponent->color.b = 1.0f;
    colorRectComponent->color.a = 1.0f;
    return colorRectComponent;
}

void color_rect_component_delete(ColorRectComponent* colorRectComponent) {
    SE_MEM_FREE(colorRectComponent);
}

ColorRectComponent* color_rect_component_copy(const ColorRectComponent* colorRectComponent) {
    ColorRectComponent* copiedNode = SE_MEM_ALLOCATE(ColorRectComponent);
    memcpy(copiedNode, colorRectComponent, sizeof(ColorRectComponent));
    return copiedNode;
}
