#pragma once

#include <stdbool.h>

char* se_strdup(const char* string);
const char* se_bool_to_string(bool value);
char* se_str_to_lower();
char* se_str_to_lower_and_underscore_whitespace(char* str);
// Will trim text based on a delimiter and return the string before that
char* se_str_trim(const char* value, char delimiter);
// Same as 'se_str_trim' but will replace the 'delimiter' with the 'replacementValue'.
char* se_str_trim_and_replace(const char* value, char delimiter, const char* replacementValue);
