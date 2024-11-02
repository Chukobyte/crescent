#include "text_label_component.h"

#include <stddef.h>
#include <string.h>

#include <seika/memory.h>

TextLabelComponent* text_label_component_create() {
    TextLabelComponent* textLabelComponent = SKA_ALLOC_ZEROED(TextLabelComponent);
    textLabelComponent->color = SKA_COLOR_WHITE;
    textLabelComponent->text[0] = '\0';
    return textLabelComponent;
}

void text_label_component_delete(TextLabelComponent* textLabelComponent) {
    SKA_FREE(textLabelComponent);
}

TextLabelComponent* text_label_component_copy(const TextLabelComponent* textLabelComponent) {
    TextLabelComponent* copiedNode = SKA_ALLOC(TextLabelComponent);
    memcpy(copiedNode, textLabelComponent, sizeof(TextLabelComponent));
    return copiedNode;
}
