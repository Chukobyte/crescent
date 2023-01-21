#pragma once

#ifdef _MSC_VER
#define CGLM_ALL_UNALIGNED
#endif

#include <cglm/cglm.h>

#define SE_PI 3.14159265358979323846f
#define SE_RAD_2_DEG (180.0f / SE_PI)

// --- Vector2 --- //
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

bool se_math_vec2_equals(const Vector2* v1, const Vector2* v2);
Vector2 se_math_vec2_lerp(const Vector2* v1, const Vector2* v2, float t);

// --- Size2D --- //
typedef struct Size2D {
    float w;
    float h;
} Size2D;

// --- Rect2 --- //
typedef struct Rect2 {
    float x;
    float y;
    float w;
    float h;
} Rect2;

bool se_rect2_does_rectangles_overlap(Rect2* sourceRect, Rect2* targetRect);

//--- Transform2D ---//
typedef struct Transform2D {
    Vector2 position;
    Vector2 scale;
    float rotation; // degrees
} Transform2D;

typedef struct TransformModel2D {
    Vector2 position;
    Vector2 scale;
    float rotation; // degrees
    int zIndex;
    Vector2 scaleSign;
    mat4 model;
} TransformModel2D;

// --- Vector3 --- //
typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

// --- Vector4 --- //
typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

// --- Color --- //
typedef struct Color {
    float r;
    float g;
    float b;
    float a;
} Color;

Color se_color_get_normalized_color_default_alpha(unsigned int r, unsigned int g, unsigned int b);
Color se_color_get_normalized_color(unsigned int r, unsigned int g, unsigned int b, unsigned int a);
Color se_color_get_normalized_color_from_color(const Color* color);
Color se_color_get_white();

// --- Misc --- //
float se_math_lerpf(float a, float b, float t);
float se_math_map_to_range(float input, float inputMin, float inputMax, float outputMin, float outputMax);
float se_math_map_to_unit(float input, float inputMin, float inputMax);
float se_math_signf(float value);
Vector2 se_math_signvec2(Vector2* value);
int se_math_clamp_int(int value, int min, int max);
