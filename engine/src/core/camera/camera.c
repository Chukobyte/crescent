#include "camera.h"

#include "../game_properties.h"

void rbe_camera2d_clamp_viewport_to_boundary(RBECamera2D* camera2D) {
    RBEGameProperties* gameProperties = rbe_game_props_get();
    if (camera2D->viewport.x < camera2D->boundary.x) {
        camera2D->viewport.x = camera2D->boundary.x;
    } else if (camera2D->viewport.x > camera2D->boundary.w - (float) gameProperties->resolutionWidth) {
        camera2D->viewport.x = camera2D->boundary.w - (float) gameProperties->resolutionWidth;
    }
    if (camera2D->viewport.y < camera2D->boundary.y) {
        camera2D->viewport.y = camera2D->boundary.y;
    } else if (camera2D->viewport.y > camera2D->boundary.h - (float) gameProperties->resolutionHeight) {
        camera2D->viewport.y = camera2D->boundary.h - (float) gameProperties->resolutionHeight;
    }
}
