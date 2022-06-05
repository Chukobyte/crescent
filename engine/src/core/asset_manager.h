#pragma once

void rbe_asset_manager_initialize();
struct Texture* rbe_asset_manager_load_texture(const char* fileName, const char* key);
struct Texture* rbe_asset_manager_get_texture(const char* key);
void rbe_asset_manager_finalize();
