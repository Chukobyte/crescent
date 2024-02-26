#pragma once

#include <string.h>

#include <cjson/cJSON.h>

#include <seika/math/se_math.h>
#include <seika/utils/se_assert.h>

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

SKAVector2 json_get_vec2(cJSON* json, const char* key);
SKAVector2 json_get_vec2_default(cJSON* json, const char* key, SKAVector2 defaultValue);

SKASize2D json_get_size2d(cJSON* json, const char* key);
SKASize2D json_get_size2d_default(cJSON* json, const char* key, SKASize2D defaultValue);

SKASize2Di json_get_size2di(cJSON* json, const char* key);
SKASize2Di json_get_size2di_default(cJSON* json, const char* key, SKASize2Di defaultValue);

SKARect2 json_get_rect2(cJSON* json, const char* key);
SKARect2 json_get_rect2_default(cJSON* json, const char* key, SKARect2 defaultValue);

SKAColor json_get_linear_color(cJSON* json, const char* key);
SKAColor json_get_linear_color_default(cJSON* json, const char* key, SKAColor defaultValue);

SKAMinMaxVec2 json_get_minmax_vec2(cJSON* json, const char* key);
SKAMinMaxVec2 json_get_minmax_vec2_default(cJSON* json, const char* key, SKAMinMaxVec2 minmax);
