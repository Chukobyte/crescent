#pragma once

#include <pocketpy.h>

#include <seika/ecs/entity.h>

void cre_pkpy_instance_cache_init();
void cre_pkpy_instance_cache_finalize();
py_Ref cre_pkpy_instance_cache_add(SkaEntity entity,const char* classPath, const char* className);
void cre_pkpy_instance_cache_remove(SkaEntity entity);
py_Ref cre_pkpy_instance_cache_get(SkaEntity entity);
py_Ref cre_pkpy_instance_cache_get_checked(SkaEntity entity);
