#pragma once

#include <stdbool.h>

// Top level class that helps simplify ecs initialization and finalization.  Will most likely remove and place in core.c in the future.
void cre_ecs_manager_initialize();
void cre_ecs_manager_finalize();
void cre_ecs_manager_enable_fps_display_entity(bool enabled);
