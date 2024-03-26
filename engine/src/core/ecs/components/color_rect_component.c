#include "color_rect_component.h"

#include <string.h>

#include <seika/memory.h>

ColorRectComponent* color_rect_component_create() {
    ColorRectComponent* colorRectComponent = SKA_MEM_ALLOCATE(ColorRectComponent);
    colorRectComponent->size = (SkaSize2D){ .w = 32.0f, .h = 32.0f };
    colorRectComponent->color = SKA_COLOR_WHITE;
    return colorRectComponent;
}

void color_rect_component_delete(ColorRectComponent* colorRectComponent) {
    SKA_MEM_FREE(colorRectComponent);
}

ColorRectComponent* color_rect_component_copy(const ColorRectComponent* colorRectComponent) {
    ColorRectComponent* copiedNode = SKA_MEM_ALLOCATE(ColorRectComponent);
    memcpy(copiedNode, colorRectComponent, sizeof(ColorRectComponent));
    return copiedNode;
}
