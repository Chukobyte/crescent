#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../seika/src/rendering/font.h"
#include "../seika/src/math/se_math.h"

#define TEXT_LABEL_BUFFER_SIZE 64

typedef struct TextLabelComponent {
    Font* font;
    Color color;
    char text[TEXT_LABEL_BUFFER_SIZE];
} TextLabelComponent;

TextLabelComponent* text_label_component_create();
void text_label_component_delete(TextLabelComponent* textLabelComponent);
TextLabelComponent* text_label_component_copy(const TextLabelComponent* textLabelComponent);

#ifdef __cplusplus
}
#endif
