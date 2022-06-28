#pragma once

// --- Generic Python Utility functions --- //

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct _object; // PyObject

// Helper functions
const char* pyh_get_string_from_obj(struct _object* obj);

const char* pyh_get_string_from_dict(struct _object* pyDict, const char* key);

const char* pyh_get_string_from_dict_default(struct _object* pyDict, const char* key, const char* defaultValue);

int pyh_get_int_from_dict(struct _object* pyDict, const char* key);

int pyh_get_int_from_dict_default(struct _object* pyDict, const char* key, int defaultValue);

const char* phy_get_string_from_var(struct _object* obj, const char* variableName);

int phy_get_int_from_var(struct _object* obj, const char* variableName);

float phy_get_float_from_var(struct _object* obj, const char* variableName);

bool phy_get_bool_from_var(struct _object* obj, const char* variableName);

bool pyh_run_python_file(const char* filePath);

// Python Cache
void pyh_cache_initialize();

void pyh_cache_finalize();

struct _object* pyh_cache_get_module(const char* modulePath);

struct _object* pyh_ref_count(struct _object* pyObject);

void pyh_ref_clean();

#ifdef __cplusplus
}
#endif
