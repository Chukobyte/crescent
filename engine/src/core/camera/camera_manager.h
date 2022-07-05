#pragma once

//--- Camera Manager ---//
// Will only contain one camera for now...

struct RBECamera2D* rbe_camera_manager_get_current_camera();
struct RBECamera2D* rbe_camera_manager_get_default_camera();
void rbe_camera_manager_reset_current_camera();
