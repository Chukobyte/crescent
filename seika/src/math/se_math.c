#include "se_math.h"

// --- Vector2 --- //
bool se_math_vec2_equals(const Vector2* v1, const Vector2* v2) {
    return v1->x == v2->x && v1->y == v2->y;
}

Vector2 se_math_vec2_lerp(const Vector2* v1, const Vector2* v2, float t) {
    return (Vector2) {
        .x = se_math_lerpf(v1->x, v2->x, t),
        .y = se_math_lerpf(v1->y, v2->y, t)
    };
}

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
float se_math_lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

float se_math_map_to_range(float input, float inputMin, float inputMax, float outputMin, float outputMax) {
    return (((input - inputMin) / (inputMax - inputMin)) * (outputMax - outputMin) + outputMin);
}

float se_math_map_to_unit(float input, float inputMin, float inputMax) {
    return se_math_map_to_range(input, inputMin, inputMax, 0.0f, 1.0f);
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

bool se_math_is_almost_equal_float(float v1, float v2, float epsilon) {
    return fabsf(v1 - v2) <= epsilon;
}

bool se_math_is_almost_equal_float_default(float v1, float v2) {
    static const double epsilon = 0.001f;
    return fabsf(v1 - v2) <= epsilon;
}

bool se_math_is_almost_equal_double(double v1, double v2, double epsilon) {
    return fabs(v1 - v2) <= epsilon;
}

bool se_math_is_almost_equal_double_default(double v1, double v2) {
    static const double epsilon = 0.001;
    return fabs(v1 - v2) <= epsilon;
}
