#include "text_label_component.h"

#include "../../memory/rbe_mem.h"

TextLabelComponent* text_label_component_create() {
    TextLabelComponent* textLabelComponent = RBE_MEM_ALLOCATE(TextLabelComponent);
    textLabelComponent->font = NULL;
    textLabelComponent->color = rbe_color_get_white();
    textLabelComponent->text[0] = '\0';
    return NULL;
}
