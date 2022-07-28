#include "color_square_component.h"

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
