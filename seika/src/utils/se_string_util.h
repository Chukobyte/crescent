#pragma once

#include <stddef.h>
#include <stdbool.h>

// Copies string and allocated new memory
char* se_strdup(const char* string);
// Copies string from a void pointer and allocated new memory
char* se_strdup_from_memory(void* data, size_t size);
const char* se_bool_to_string(bool value);
char* se_str_to_lower();
char* se_str_to_lower_and_underscore_whitespace(char* str);
// Will trim text based on a delimiter and return the string before that
char* se_str_trim(const char* value, char delimiter);
// Same as 'se_str_trim' but will replace the 'delimiter' with the 'replacementValue'.
char* se_str_trim_and_replace(const char* value, char delimiter, const char* replacementValue);
// Removes instances of '\r' from a string
void se_str_remove_char(char* string, char charToRemove);
