#include "se_math.h"

// --- Rect2 --- //
bool se_rect2_does_rectangles_overlap(Rect2* sourceRect, Rect2* targetRect) {
    return (sourceRect->x + sourceRect->w >= targetRect->x) &&
           (targetRect->x + targetRect->w >= sourceRect->x) &&
           (sourceRect->y + sourceRect->h >= targetRect->y) &&
           (targetRect->y + targetRect->h >= sourceRect->y);
}

// --- Color --- //
Color se_color_get_normalized_color_default_alpha(unsigned int r, unsigned int g, unsigned int b) {
    Color color = {
        .r = (float) r / 255.0f,
        .g = (float) g / 255.0f,
        .b = (float) b / 255.0f,
        .a = 1.0f
    };
    return color;
}

Color se_color_get_normalized_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
    Color color = {
        .r = (float) r / 255.0f,
        .g = (float) g / 255.0f,
        .b = (float) b / 255.0f,
        .a = (float) a / 255.0f
    };
    return color;
}

Color se_color_get_normalized_color_from_color(const Color* color) {
    Color newColor = {
        .r = color->r / 255.0f,
        .g = color->g / 255.0f,
        .b = color->b / 255.0f,
        .a = color->a / 255.0f
    };
    return newColor;
}

Color se_color_get_white() {
    Color white = { 1.0f, 1.0f, 1.0f, 1.0f};
    return white;
}

// --- Misc --- //
float se_math_map_to_range(float input, float inputLow, float inputHigh, float outputLow, float outputHigh) {
    return (((input - inputLow) / (inputHigh - inputLow)) * (outputHigh - outputLow) + outputLow);
}

float se_math_map_to_unit(float input, float inputLow, float inputHigh) {
    return se_math_map_to_range(input, inputLow, inputHigh, 0.0f, 1.0f);
}

float se_math_signf(float value) {
    if (value > 0.0f) {
        return 1.0f;
    } else if(value < 0.0f) {
        return -1.0f;
    }
    return 0.0f;
}

Vector2 se_math_signvec2(Vector2* value) {
    Vector2 sign_vec = {
        .x = se_math_signf(value->x),
        .y = se_math_signf(value->y)
    };
    return sign_vec;
}

int se_math_clamp_int(int value, int min, int max) {
    return value < min ? min : (value > max ? max : value);
}
