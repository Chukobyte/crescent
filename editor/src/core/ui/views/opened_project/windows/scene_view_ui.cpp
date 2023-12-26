#include "scene_view_ui.h"

#include "../engine/src/core/scene/scene_utils.h"
#include "../engine/src/core/game_properties.h"

#include "../../../imgui/imgui_window_renderer.h"
#include "../../../../asset_manager.h"
#include "../../../../scene/scene_manager.h"

namespace WindowRenderUtils {
EntityArray OnGetSelfAndParentEntitiesFunc(CreEntity entity) {
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

SKATransform2D OnGetLocalTransformFunc(CreEntity entity, int* zIndex, bool* success) {
    static auto* sceneManager = SceneManager::Get();
    if (auto* node = sceneManager->GetNode(sceneManager->selectedSceneFile, entity)) {
        if (auto* transformComp = node->GetComponentSafe<Transform2DComp>()) {
            *success = true;
            *zIndex = transformComp->zIndex;
            return transformComp->transform2D;
        }
    }
    *success = false;
    return {};
}
} // namespace WindowRenderUtils

ImGuiHelper::Window OpenedProjectUI::Windows::GetSceneViewWindow() {
    ImGuiHelper::Window sceneViewWindow = {
        .name = "Scene View",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_NoResize,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static auto GetNodeTextureRenderTarget = [](SceneNode* node, size_t index, Transform2DComp* transformComp, bool& hasTexture) {
                static AssetManager* assetManager = AssetManager::Get();
                static SKATransformModel2D globalTransforms[CRE_MAX_ENTITIES];
                static SETexture* whiteRectTexture = se_texture_create_solid_colored_texture(1, 1, 255);
                SETexture* renderTargetTexture = nullptr;
                cre_scene_utils_update_global_transform_model(node->GetUID(), &globalTransforms[index]);
                SKARect2 sourceRect = SKA_RECT2D_ZERO;
                SKASize2D destSize = SKA_SIZE2D_ZERO;
                SKAColor color = SKA_COLOR_WHITE;
                bool flipH = false;
                bool flipV = false;
                SKAVector2 origin = SKA_VECTOR2_ZERO;
                hasTexture = true;
                if (auto* spriteComp = node->GetComponentSafe<SpriteComp>()) {
                    renderTargetTexture = assetManager->GetTexture(spriteComp->texturePath.c_str());
                    sourceRect = spriteComp->drawSource;
                    destSize = { sourceRect.w, sourceRect.h };
                    color = spriteComp->modulate;
                    flipH = spriteComp->flipH;
                    flipV = spriteComp->flipV;
                    origin = spriteComp->origin;
                } else if (auto* animSpriteComp = node->GetComponentSafe<AnimatedSpriteComp>()) {
                    if (!animSpriteComp->currentAnimationName.empty()) {
                        const EditorAnimation& anim = animSpriteComp->GetAnimationByName(animSpriteComp->currentAnimationName);
                        if (!anim.animationFrames.empty()) {
                            // TODO: Get frame from current index instead...
                            const auto& animFrame = anim.animationFrames[0];
                            renderTargetTexture = assetManager->GetTexture(animFrame.texturePath.c_str());
                            sourceRect = animFrame.drawSource;
                            destSize = { sourceRect.w, sourceRect.h };
                            color = animSpriteComp->modulate;
                            flipH = animSpriteComp->flipH;
                            flipV = animSpriteComp->flipV;
                            origin = animSpriteComp->origin;
                        }
                    }
                } else if (auto* colorSpriteComp = node->GetComponentSafe<ColorRectComp>()) {
                    renderTargetTexture = whiteRectTexture;
                    sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f };
                    destSize = colorSpriteComp->size;
                    color = colorSpriteComp->color;
//                }  else if (auto* collider2DComp = node->GetComponentSafe<Collider2DComp>()) {
//                    renderTargetTexture = whiteRectTexture;
//                    sourceRect = { 0.0f, 0.0f, 1.0f, 1.0f };
//                    destSize = collider2DComp->extents;
//                    color = collider2DComp->color;
                } else {
                    hasTexture = false;
                }
                cre_scene_utils_apply_camera_and_origin_translation(&globalTransforms[index], &origin, transformComp->ignoreCamera);
                return ImGuiHelper::TextureRenderTarget{
                    .texture = renderTargetTexture,
                    .sourceRect = sourceRect,
                    .destSize = destSize,
                    .color = color,
                    .flipH = flipH,
                    .flipV = flipV,
                    .globalTransform = &globalTransforms[index],
                    .zIndex = globalTransforms[index].zIndex
                };
            };
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
            if (sceneManager->selectedSceneFile && sceneManager->selectedSceneFile->rootNode) {
                sceneManager->IterateAllSceneNodes(sceneManager->selectedSceneFile->rootNode, [&textureRenderTargets, &fontRenderTargets](SceneNode* node, size_t i) {
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
                            bool hasTexture = false;
                            const ImGuiHelper::TextureRenderTarget renderTarget = GetNodeTextureRenderTarget(node, i, transformComp, hasTexture);
                            if (hasTexture) {
                                textureRenderTargets.emplace_back(renderTarget);
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
