#pragma once

#include "../../math/rbe_math.h"
#include "../../rendering/font.h"

#define TEXT_LABEL_BUFFER_SIZE 64

typedef struct TextLabelComponent {
    Font* font;
    Color color;
    char text[TEXT_LABEL_BUFFER_SIZE];
} TextLabelComponent;

TextLabelComponent* text_label_component_create();
