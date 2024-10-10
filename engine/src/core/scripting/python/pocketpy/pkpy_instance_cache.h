#pragma once

#include <pocketpy.h>

#include <seika/ecs/entity.h>

void cre_pkpy_instance_cache_init();
void cre_pkpy_instance_cache_finalize();
// Creates a new instance for the entity, if the entity already exists will return the existing instance instead
py_Ref cre_pkpy_instance_cache_add(SkaEntity entity, const char* classPath, const char* className);
// Same as 'cre_pkpy_instance_cache_add' but will figure out the class info and create a script component if nonexistant
py_Ref cre_pkpy_instance_cache_add2(SkaEntity entity);
void cre_pkpy_instance_cache_remove(SkaEntity entity);
py_Ref cre_pkpy_instance_cache_get(SkaEntity entity);
py_Ref cre_pkpy_instance_cache_get_checked(SkaEntity entity);
bool cre_pkpy_instance_cache_has(SkaEntity entity);
