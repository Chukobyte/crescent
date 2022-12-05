#pragma once

// A 'world' object contains global information for a game.
// Will give user a default world to start with.
// May want to group scenes to worlds in the future or possibly change the concept of worlds

void cre_world_set_time_dilation(float timeDilation);
float cre_world_get_time_dilation();
