#include "color_rect_component.h"

#include <string.h>

#include <seika/memory/se_mem.h>

ColorRectComponent* color_rect_component_create() {
    ColorRectComponent* colorRectComponent = SE_MEM_ALLOCATE(ColorRectComponent);
    colorRectComponent->size = (SKASize2D){ .w = 32.0f, .h = 32.0f };
    colorRectComponent->color = SKA_COLOR_WHITE;
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
