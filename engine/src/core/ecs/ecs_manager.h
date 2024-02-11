#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct SETexture;

// Top level class that helps simplify ecs initialization and finalization.  Will most likely remove and place in core.c in the future.
void cre_ecs_manager_initialize();
void cre_ecs_manager_initialize_ex(struct SETexture* colorRectTexture, struct SETexture* particle2DSquareTexture);
void cre_ecs_manager_finalize();
void cre_ecs_manager_enable_fps_display_entity(bool enabled, const char* fontUID, float positionX, float positionY);

// Made for editor
void cre_ecs_manager_initialize_editor();
void cre_ecs_manager_finalize_editor();


#ifdef __cplusplus
}
#endif
