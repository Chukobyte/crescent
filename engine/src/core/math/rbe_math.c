#include "rbe_math.h"

Color rbe_color_get_normalized_color_default_alpha(unsigned int r, unsigned int g, unsigned int b) {
    Color color = {
        .r = (float) r / 255.0f,
        .g = (float) g / 255.0f,
        .b = (float) b / 255.0f,
        .a = 1.0f
    };
    return color;
}

Color rbe_color_get_normalized_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a) {
    Color color = {
        .r = (float) r / 255.0f,
        .g = (float) g / 255.0f,
        .b = (float) b / 255.0f,
        .a = (float) a / 255.0f
    };
    return color;
}

// --- Misc --- //
float rbe_math_map_to_range(float input, float inputLow, float inputHigh, float outputLow, float outputHigh) {
    return (((input - inputLow) / (inputHigh - inputLow)) * (outputHigh - outputLow) + outputLow);
}

float rbe_math_map_to_unit(float input, float inputLow, float inputHigh) {
    return rbe_math_map_to_range(input, inputLow, inputHigh, 0.0f, 1.0f);
}
