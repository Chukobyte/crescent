#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <seika/defines.h>

struct SkaTexture;

// Top level class that helps simplify ecs initialization and finalization.  Will most likely remove and place in core.c in the future.
void cre_ecs_manager_initialize();
void cre_ecs_manager_initialize_ex(struct SkaTexture* colorRectTexture, struct SkaTexture* particle2DSquareTexture);
void cre_ecs_manager_finalize();
void cre_ecs_manager_enable_fps_display_entity(bool enabled, const char* fontUID, f32 positionX, f32 positionY);

// Made for editor
void cre_ecs_manager_initialize_editor();
void cre_ecs_manager_finalize_editor();


#ifdef __cplusplus
}
#endif
