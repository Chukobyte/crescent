#include "color_square_component.h"

#include <string.h>

#include "../../memory/rbe_mem.h"

ColorSquareComponent* color_square_component_create() {
    ColorSquareComponent* colorSquareComponent = RBE_MEM_ALLOCATE(ColorSquareComponent);
    colorSquareComponent->size.w = 32.0f;
    colorSquareComponent->size.h = 32.0f;
    colorSquareComponent->color.r = 1.0f;
    colorSquareComponent->color.g = 1.0f;
    colorSquareComponent->color.b = 1.0f;
    colorSquareComponent->color.a = 1.0f;
    return colorSquareComponent;
}

void color_square_component_delete(ColorSquareComponent* colorSquareComponent) {
    RBE_MEM_FREE(colorSquareComponent);
}

ColorSquareComponent* color_square_component_copy(const ColorSquareComponent* colorSquareComponent) {
    ColorSquareComponent* copiedNode = RBE_MEM_ALLOCATE(ColorSquareComponent);
    memcpy(copiedNode, colorSquareComponent, sizeof(ColorSquareComponent));
    return copiedNode;
}
