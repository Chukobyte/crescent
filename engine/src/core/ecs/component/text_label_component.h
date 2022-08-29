#pragma once

#include "../../rendering/font.h"
#include "../../math/rbe_math.h"

#define TEXT_LABEL_BUFFER_SIZE 64

typedef struct TextLabelComponent {
    Font* font;
    Color color;
    char text[TEXT_LABEL_BUFFER_SIZE];
} TextLabelComponent;

TextLabelComponent* text_label_component_create();
void text_label_component_delete(TextLabelComponent* textLabelComponent);
TextLabelComponent* text_label_component_copy(const TextLabelComponent* textLabelComponent);
