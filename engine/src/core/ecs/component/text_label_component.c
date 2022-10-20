#include "text_label_component.h"

#include <stddef.h>
#include <string.h>

#include "../seika/src/memory/rbe_mem.h"

TextLabelComponent* text_label_component_create() {
    TextLabelComponent* textLabelComponent = RBE_MEM_ALLOCATE(TextLabelComponent);
    textLabelComponent->font = NULL;
    textLabelComponent->color = rbe_color_get_white();
    textLabelComponent->text[0] = '\0';
    return textLabelComponent;
}

void text_label_component_delete(TextLabelComponent* textLabelComponent) {
    RBE_MEM_FREE(textLabelComponent);
}

TextLabelComponent* text_label_component_copy(const TextLabelComponent* textLabelComponent) {
    TextLabelComponent* copiedNode = RBE_MEM_ALLOCATE(TextLabelComponent);
    memcpy(copiedNode, textLabelComponent, sizeof(TextLabelComponent));
    return copiedNode;
}
