#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct SkaTexture;
struct Particles2DComponent;

void cre_particle_ec_system_create_and_register();
void cre_particle_ec_system_create_and_register_ex(struct SkaTexture* squareTextureOverride);

// Used to update a components particles, to be used by external sources (e.g. editor)
void cre_particle_emitter_ec_system_update_component(struct Particles2DComponent* particles2DComponent, float deltaTime);

#ifdef __cplusplus
}
#endif
