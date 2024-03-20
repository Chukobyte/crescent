#include "scene_view_ui.h"

#include <seika/rendering/renderer.h>

#include "../engine/src/core/scene/scene_utils.h"
#include "../engine/src/core/game_properties.h"
#include "../engine/src/core/ecs/systems/particle_emitter_ec_system.h"

#include "editors/tilemap_editor.h"
#include "../../../../editor_context.h"
#include "../../../../asset_manager.h"
#include "../../../../scene/scene_manager.h"

namespace WindowRenderUtils {
    EntityArray OnGetSelfAndParentEntitiesFunc(SkaEntity entity) {
        static auto* sceneManager = SceneManager::Get();
        EntityArray combineModelResult = { .entityCount = 0 };
        combineModelResult.entities[combineModelResult.entityCount++] = entity;
        if (auto* node = sceneManager->GetNode(sceneManager->selectedSceneFile, entity)) {
            auto* parent = node->parent;
            while (parent != nullptr) {
                combineModelResult.entities[combineModelResult.entityCount++] = parent->GetUID();
                parent = parent->parent;
            }
        }
        return combineModelResult;
    }

    SKATransform2D OnGetLocalTransformFunc(SkaEntity entity, int* zIndex, bool* success) {
        static auto* sceneManager = SceneManager::Get();
        if (auto* node = sceneManager->GetNode(sceneManager->selectedSceneFile, entity)) {
            if (auto* transformComp = node->GetComponentSafe<Transform2DComp>()) {
                *success = true;
                *zIndex = transformComp->zIndex;
                return transformComp->transform2D;
            }
        }
        *success = false;
        return SKA_TRANSFORM_IDENTITY;
    }

    std::vector<ImGuiHelper::TextureRenderTarget> GetNodeTextureRenderTargets(SceneNode* node, size_t index, Transform2DComp* transform2DComp, float deltaTime) {
        static AssetManager* assetManager = AssetManager::Get();
        static SKATransformModel2D globalTransforms[SKA_MAX_ENTITIES] = {};
        static SETexture* whiteRectTexture = se_texture_create_solid_colored_texture(1, 1, 255);
        static SKAVector2 origin = SKA_VECTOR2_ZERO;

        std::vector<ImGuiHelper::TextureRenderTarget> renderTargets;

        cre_scene_utils_update_global_transform_model(node->GetUID(), &globalTransforms[index]);
        if (auto* spriteComp = node->GetComponentSafe<SpriteComp>()) {
            ImGuiHelper::TextureRenderTarget renderTarget = {
                .texture = assetManager->GetTexture(spriteComp->texturePath.c_str()),
                .sourceRect = spriteComp->drawSource,
                .destSize = { spriteComp->drawSource.w, spriteComp->drawSource.h },
                .color = spriteComp->modulate,
                .flipH = spriteComp->flipH,
                .flipV = spriteComp->flipV,
                .zIndex = globalTransforms[index].zIndex,
                .useGlobalTransform = true,
                .globalTransform = &globalTransforms[index],
            };
            cre_scene_utils_apply_camera_and_origin_translation(&globalTransforms[index], &spriteComp->origin, transform2DComp->ignoreCamera);
            renderTargets.emplace_back(renderTarget);
        } else if (auto* animSpriteComp = node->GetComponentSafe<AnimatedSpriteComp>()) {
            if (!animSpriteComp->currentAnimationName.empty()) {
                const EditorAnimation& anim = animSpriteComp->GetAnimationByName(animSpriteComp->currentAnimationName);
                if (!anim.animationFrames.empty()) {
                    // TODO: Get frame from current index instead...
                    const auto& animFrame = anim.animationFrames[0];
                    ImGuiHelper::TextureRenderTarget renderTarget = {
                        .texture = assetManager->GetTexture(animFrame.texturePath.c_str()),
                        .sourceRect = animFrame.drawSource,
                        .destSize = { animFrame.drawSource.w, animFrame.drawSource.h },
                        .color = animSpriteComp->modulate,
                        .flipH = animSpriteComp->flipH,
                        .flipV = animSpriteComp->flipV,
                        .zIndex = globalTransforms[index].zIndex,
                        .useGlobalTransform = true,
                        .globalTransform = &globalTransforms[index],
                    };
                    cre_scene_utils_apply_camera_and_origin_translation(&globalTransforms[index], &animSpriteComp->origin, transform2DComp->ignoreCamera);
                    renderTargets.emplace_back(renderTarget);
                }
            }
        } else if (auto* colorSpriteComp = node->GetComponentSafe<ColorRectComp>()) {
            ImGuiHelper::TextureRenderTarget renderTarget = {
                .texture = whiteRectTexture,
                .sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f },
                .destSize = colorSpriteComp->size,
                .color = colorSpriteComp->color,
                .flipH = false,
                .flipV = false,
                .zIndex = globalTransforms[index].zIndex,
                .useGlobalTransform = true,
                .globalTransform = &globalTransforms[index],
            };
            cre_scene_utils_apply_camera_and_origin_translation(&globalTransforms[index], &origin, transform2DComp->ignoreCamera);
            renderTargets.emplace_back(renderTarget);
        } else if (auto* particles2dComp = node->GetComponentSafe<Particles2DComp>()) {
            if (particles2dComp->state != Particle2DComponentState_INACTIVE) {
                cre_scene_utils_apply_camera_and_origin_translation(&globalTransforms[index], &origin, transform2DComp->ignoreCamera);
                Particles2DComponent* internalParticleComp = particles2dComp->GetInternalComp();
                cre_particle_emitter_ec_system_update_component(internalParticleComp, deltaTime);
                // Draw individual particles
                SKATransform2D baseParticleTransform;
                ska_transform2d_mat4_to_transform(globalTransforms[index].model, &baseParticleTransform);

                for (int pi = 0; pi < particles2dComp->amount; pi++) {
                    CreParticle2D* particle2D = &internalParticleComp->particles[pi];
                    if (particle2D->state != Particle2DState_ACTIVE) {
                        continue;
                    }
                    // TODO: Setting inactive here so we can render
                    if (particle2D->timeActive >= particles2dComp->lifeTime) {
                        particle2D->state = Particle2DState_TIMED_WAITING_TO_BE_ACTIVE;
                    }
                    const SKATransform2D particle2DTransform = {
                        .position = { .x = baseParticleTransform.position.x + particle2D->position.x, .y = baseParticleTransform.position.y + particle2D->position.y },
                        .scale = baseParticleTransform.scale,
                        .rotation = baseParticleTransform.rotation
                    };

                    ImGuiHelper::TextureRenderTarget renderTarget = {
                        .texture = whiteRectTexture,
                        .sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f },
                        .destSize = internalParticleComp->squareSize,
                        .color = particle2D->color,
                        .flipH = false,
                        .flipV = false,
                        .zIndex = globalTransforms[index].zIndex,
                        .useGlobalTransform = false
                    };
                    renderTarget.transform2D = particle2DTransform;
                    renderTargets.emplace_back(renderTarget);
                }
            }
        } else if (auto* tilemapComp = node->GetComponentSafe<TilemapComp>()) {
            if (!tilemapComp->texturePath.empty()) {
                cre_scene_utils_apply_camera_and_origin_translation(&globalTransforms[index], &tilemapComp->origin, transform2DComp->ignoreCamera);
                // Draw individual tiles
                SKATransform2D baseTileTransform;
                ska_transform2d_mat4_to_transform(globalTransforms[index].model, &baseTileTransform);
                const auto& internalTilemapComp = tilemapComp->GetInternalComp();

                tilemapComp->ForEachActiveTile([tilemapComp, &renderTargets, &baseTileTransform, &internalTilemapComp, index](const CreTileData* tileData) {
                    const SKAVector2 tilePosition = {
                        baseTileTransform.position.x + (float)(tileData->position.x * internalTilemapComp.tilemap->tileset.tileSize.w) * baseTileTransform.scale.x,
                        baseTileTransform.position.y + (float)(tileData->position.y * internalTilemapComp.tilemap->tileset.tileSize.h) * baseTileTransform.scale.y
                    };
                    const SKATransform2D tileTransform = {
                        .position = tilePosition,
                        .scale = baseTileTransform.scale,
                        .rotation = baseTileTransform.rotation
                    };
                    const SKARect2 tileDrawSource = {
                        .x = (float)(tileData->renderCoords.x * internalTilemapComp.tilemap->tileset.tileSize.w),
                        .y = (float)(tileData->renderCoords.y * internalTilemapComp.tilemap->tileset.tileSize.h),
                        .w = (float)internalTilemapComp.tilemap->tileset.tileSize.w,
                        .h = (float)internalTilemapComp.tilemap->tileset.tileSize.h
                    };

                    ImGuiHelper::TextureRenderTarget renderTarget = {
                        .texture = assetManager->GetTexture(tilemapComp->texturePath.c_str()),
                        .sourceRect = tileDrawSource,
                        .destSize = { .w = (float)internalTilemapComp.tilemap->tileset.tileSize.w, .h = (float)internalTilemapComp.tilemap->tileset.tileSize.h },
                        .color = SKA_COLOR_WHITE,
                        .flipH = false,
                        .flipV = false,
                        .zIndex = globalTransforms[index].zIndex,
                        .useGlobalTransform = false
                    };
                    renderTarget.transform2D = tileTransform;
                    renderTargets.emplace_back(renderTarget);
                });
            }
        }

        return renderTargets;
    }
} // namespace WindowRenderUtils

ImGuiHelper::Window OpenedProjectUI::Windows::GetSceneViewWindow() {
    ImGuiHelper::Window sceneViewWindow = {
            .name = "Scene View",
            .open = nullptr,
            .windowFlags = ImGuiWindowFlags_NoResize,
            .callbackFunc = [] (ImGuiHelper::Context* context) {
                static SceneManager* sceneManager = SceneManager::Get();
                static AssetManager* assetManager = AssetManager::Get();
                std::vector<ImGuiHelper::TextureRenderTarget> textureRenderTargets;
                std::vector<ImGuiHelper::FontRenderTarget> fontRenderTargets;
                static bool hasBindedSceneUtilsFuncs = false;
                if (!hasBindedSceneUtilsFuncs) {
                    cre_scene_utils_override_on_get_local_transform_func(WindowRenderUtils::OnGetLocalTransformFunc);
                    cre_scene_utils_override_on_get_self_and_parent_entities_func(WindowRenderUtils::OnGetSelfAndParentEntitiesFunc);
                    hasBindedSceneUtilsFuncs = true;
                }
                // Loop through and render all scene nodes starting from the root
//                const float& deltaTime = EditorContext::Get()->simDeltaTime;
                const float& deltaTime = EditorContext::Get()->editorDeltaTime;
                if (sceneManager->selectedSceneFile && sceneManager->selectedSceneFile->rootNode) {
                    sceneManager->IterateAllSceneNodes(sceneManager->selectedSceneFile->rootNode, [&textureRenderTargets, &fontRenderTargets, &deltaTime](SceneNode* node, size_t i) {
                        if (auto* transformComp = node->GetComponentSafe<Transform2DComp>()) {
                            if (auto* textLabelComp = node->GetComponentSafe<TextLabelComp>()) {
                                SKATransformModel2D globalTransform = { transformComp->transform2D.position, transformComp->transform2D.scale, transformComp->transform2D.rotation };
                                cre_scene_utils_update_global_transform_model(node->GetUID(), &globalTransform);
                                static SKAVector2 textLabelOrigin = SKA_VECTOR2_ZERO;
                                cre_scene_utils_apply_camera_and_origin_translation(&globalTransform, &textLabelOrigin, transformComp->ignoreCamera);
                                const char* renderFontUID = !textLabelComp->fontUID.empty() ? textLabelComp->fontUID.c_str() : CRE_DEFAULT_FONT_ASSET.uid;
                                const ImGuiHelper::FontRenderTarget renderTarget = {
                                    .font = assetManager->GetFont(renderFontUID),
                                    .text = textLabelComp->text,
                                    .position = globalTransform.position,
                                    .scale = globalTransform.scale.x,
                                    .color = textLabelComp->color,
                                    .zIndex = globalTransform.zIndex
                                };
                                fontRenderTargets.emplace_back(renderTarget);
                            } else {
                                const auto renderTargets = WindowRenderUtils::GetNodeTextureRenderTargets(node, i, transformComp, deltaTime);
                                if (!renderTargets.empty()) {
                                    textureRenderTargets.insert(textureRenderTargets.end(), renderTargets.begin(), renderTargets.end());
                                }

                                // Temp tilemap
                                if (auto* tilemapComp = node->GetComponentSafe<TilemapComp>()) {
                                    static TilemapEditor* tilemapEditor = TilemapEditor::Get();
                                    if (tilemapEditor->IsNodeSelected(node)) {
                                        const auto tilemapRenderTargets = tilemapEditor->GetFontRenderTargets();
                                        fontRenderTargets.insert(fontRenderTargets.end(), tilemapRenderTargets.begin(), tilemapRenderTargets.end());
                                        const auto tilemapTextureRenderTarget = tilemapEditor->GetTextureRenderTargets();
                                        textureRenderTargets.insert(textureRenderTargets.end(), tilemapTextureRenderTarget.begin(), tilemapTextureRenderTarget.end());
                                    }
                                }
                            }
                        }
                    });
                }
                ImGuiHelper::WindowRenderer::Render(textureRenderTargets, fontRenderTargets);
            },
            .position = ImVec2{ 300.0f, 100.0f },
            .size = ImVec2{ 400.0f, 300.0f },
    };
    return sceneViewWindow;
}
