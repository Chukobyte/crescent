#pragma once

// A 'world' object contains global information for a game.
// Will give user a default world to start with.
// May want to group scenes to worlds in the future or possibly change the concept of worlds

#include <seika/defines.h>

void cre_world_set_time_dilation(f32 timeDilation);
f32 cre_world_get_time_dilation();
void cre_world_set_frame_delta_time(f32 frameDeltaTime);
f32 cre_world_get_frame_delta_time();
