#pragma once

#include <stdbool.h>

char* se_strdup(const char* string);
const char* se_bool_to_string(bool value);
char* se_str_to_lower();
char* se_str_to_lower_and_underscore_whitespace(char* str);
