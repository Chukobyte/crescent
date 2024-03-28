#pragma once

#include <string.h>

#include <cjson/cJSON.h>

#include <seika/math/math.h>
#include <seika/assert.h>

// Utility Functions
const char* json_get_string(cJSON* json, const char* key);
const char* json_get_string_default(cJSON* json, const char* key, const char* defaultValue);
char* json_get_string_new(cJSON* json, const char* key);
char* json_get_string_new_unchecked(cJSON* json, const char* key);
char* json_get_string_default_new(cJSON* json, const char* key, const char* defaultValue);

int32 json_get_int(cJSON* json, const char* key);
int32 json_get_int_default(cJSON* json, const char* key, int32 defaultValue);

f64 json_get_double(cJSON* json, const char* key);
f64 json_get_double_default(cJSON* json, const char* key, f64 defaultValue);

bool json_get_bool(cJSON* json, const char* key);
bool json_get_bool_default(cJSON* json, const char* key, bool defaultValue);

SkaVector2 json_get_vec2(cJSON* json, const char* key);
SkaVector2 json_get_vec2_default(cJSON* json, const char* key, SkaVector2 defaultValue);

SkaVector2i json_get_vec2i(cJSON* json, const char* key);
SkaVector2i json_get_vec2i_default(cJSON* json, const char* key, SkaVector2i defaultValue);

SkaSize2D json_get_size2d(cJSON* json, const char* key);
SkaSize2D json_get_size2d_default(cJSON* json, const char* key, SkaSize2D defaultValue);

SkaSize2Di json_get_size2di(cJSON* json, const char* key);
SkaSize2Di json_get_size2di_default(cJSON* json, const char* key, SkaSize2Di defaultValue);

SkaRect2 json_get_rect2(cJSON* json, const char* key);
SkaRect2 json_get_rect2_default(cJSON* json, const char* key, SkaRect2 defaultValue);

SkaColor json_get_linear_color(cJSON* json, const char* key);
SkaColor json_get_linear_color_default(cJSON* json, const char* key, SkaColor defaultValue);

SkaMinMaxVec2 json_get_minmax_vec2(cJSON* json, const char* key);
SkaMinMaxVec2 json_get_minmax_vec2_default(cJSON* json, const char* key, SkaMinMaxVec2 minmax);
