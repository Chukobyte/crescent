#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//--- Camera Manager ---//
// Will only contain one camera for now...

struct CRECamera2D* cre_camera_manager_get_current_camera();
struct CRECamera2D* cre_camera_manager_get_default_camera();
void cre_camera_manager_reset_current_camera();

#ifdef __cplusplus
}
#endif
