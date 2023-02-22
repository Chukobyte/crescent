#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#define CGLM_ALL_UNALIGNED
#endif

#include <cglm/cglm.h>

#define SE_PI 3.14159265358979323846f
#define SE_RAD_2_DEG (180.0f / SE_PI)

// --- SEVector2 --- //
typedef struct SEVector2 {
    float x;
    float y;
} SEVector2;

bool se_math_vec2_equals(const SEVector2* v1, const SEVector2* v2);
SEVector2 se_math_vec2_lerp(const SEVector2* v1, const SEVector2* v2, float t);

// --- SESize2D --- //
typedef struct SESize2D {
    float w;
    float h;
} SESize2D;

// --- SERect2 --- //
typedef struct SERect2 {
    float x;
    float y;
    float w;
    float h;
} SERect2;

bool se_rect2_does_rectangles_overlap(SERect2* sourceRect, SERect2* targetRect);

//--- SETransform2D ---//
typedef struct SETransform2D {
    SEVector2 position;
    SEVector2 scale;
    float rotation; // degrees
} SETransform2D;

typedef struct SETransformModel2D {
    SEVector2 position;
    SEVector2 scale;
    float rotation; // degrees
    int zIndex;
    SEVector2 scaleSign;
    mat4 model;
} SETransformModel2D;

// --- SEVector3 --- //
typedef struct SEVector3 {
    float x;
    float y;
    float z;
} SEVector3;

// --- SEVector4 --- //
typedef struct SEVector4 {
    float x;
    float y;
    float z;
    float w;
} SEVector4;

// --- SEColor --- //
typedef struct SEColor {
    float r;
    float g;
    float b;
    float a;
} SEColor;

SEColor se_color_get_normalized_color_default_alpha(unsigned int r, unsigned int g, unsigned int b);
SEColor se_color_get_normalized_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a);
SEColor se_color_get_normalized_color_from_color(const SEColor* color);
SEColor se_color_get_white();

// --- Misc --- //
float se_math_lerpf(float a, float b, float t);
float se_math_map_to_range(float input, float inputMin, float inputMax, float outputMin, float outputMax);
float se_math_map_to_unit(float input, float inputMin, float inputMax);
double se_math_map_to_range_double(double input, double inputMin, double inputMax, double outputMin, double outputMax);
double se_math_map_to_unit_double(double input, double inputMin, double inputMax);
float se_math_signf(float value);
SEVector2 se_math_signvec2(SEVector2* value);
int se_math_clamp_int(int value, int min, int max);
bool se_math_is_almost_equal_float(float v1, float v2, float epsilon);
bool se_math_is_almost_equal_float_default(float v1, float v2);
bool se_math_is_almost_equal_double(double v1, double v2, double epsilon);
bool se_math_is_almost_equal_double_default(double v1, double v2);

#ifdef __cplusplus
}
#endif
