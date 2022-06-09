#pragma once

void rbe_py_cache_initialize();
void rbe_py_cache_finalize();
struct _object* rbe_py_cache_get_module(const char* modulePath);
struct _object* rbe_py_cache_get_class(const char* modulePath, const char* classPath);
