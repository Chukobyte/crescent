#pragma once

#include <string.h>

#include <cjson/cJSON.h>

#include "../seika/src/math/se_math.h"
#include "../seika/src/utils/se_assert.h"

// Utility Functions
const char* json_get_string(cJSON* json, const char* key);
const char* json_get_string_default(cJSON* json, const char* key, const char* defaultValue);
char* json_get_string_new(cJSON* json, const char* key);
char* json_get_string_new_unchecked(cJSON* json, const char* key);
char* json_get_string_default_new(cJSON* json, const char* key, const char* defaultValue);

int json_get_int(cJSON* json, const char* key);
int json_get_int_default(cJSON* json, const char* key, int defaultValue);

double json_get_double(cJSON* json, const char* key);
double json_get_double_default(cJSON* json, const char* key, double defaultValue);

bool json_get_bool(cJSON* json, const char* key);
bool json_get_bool_default(cJSON* json, const char* key, bool defaultValue);

Vector2 json_get_vec2(cJSON* json, const char* key);
Vector2 json_get_vec2_default(cJSON* json, const char* key, Vector2 defaultValue);

Size2D json_get_size2d(cJSON* json, const char* key);
Size2D json_get_size2d_default(cJSON* json, const char* key, Size2D defaultValue);

Rect2 json_get_rect2(cJSON* json, const char* key);
Rect2 json_get_rect2_default(cJSON* json, const char* key, Rect2 defaultValue);

Color json_get_linear_color(cJSON* json, const char* key);
Color json_get_linear_color_default(cJSON* json, const char* key, Color defaultValue);
