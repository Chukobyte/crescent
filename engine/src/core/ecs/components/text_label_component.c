#include "text_label_component.h"

#include <stddef.h>
#include <string.h>

#include <seika/memory.h>

TextLabelComponent* text_label_component_create() {
    TextLabelComponent* textLabelComponent = SKA_MEM_ALLOCATE(TextLabelComponent);
    textLabelComponent->color = SKA_COLOR_WHITE;
    textLabelComponent->text[0] = '\0';
    return textLabelComponent;
}

void text_label_component_delete(TextLabelComponent* textLabelComponent) {
    SKA_MEM_FREE(textLabelComponent);
}

TextLabelComponent* text_label_component_copy(const TextLabelComponent* textLabelComponent) {
    TextLabelComponent* copiedNode = SKA_MEM_ALLOCATE(TextLabelComponent);
    memcpy(copiedNode, textLabelComponent, sizeof(TextLabelComponent));
    return copiedNode;
}
