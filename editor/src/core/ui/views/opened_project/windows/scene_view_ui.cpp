#include "scene_view_ui.h"

#include "../engine/src/core/scene/scene_utils.h"

#include "../../../imgui/imgui_window_renderer.h"
#include "../../../../asset_manager.h"
#include "../../../../scene/scene_manager.h"

namespace WindowRenderUtils {
EntityArray OnGetSelfAndParentEntitiesFunc(Entity entity) {
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

SETransform2D OnGetLocalTransformFunc(Entity entity, int* zIndex, bool* success) {
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
                static SETransformModel2D globalTransforms[MAX_ENTITIES];
                static SETexture* whiteRectTexture = se_texture_create_solid_colored_texture(1, 1, 255);
                SETexture* renderTargetTexture = nullptr;
                cre_scene_utils_update_global_transform_model(node->GetUID(), &globalTransforms[index]);
                SERect2 sourceRect = {0.0f, 0.0f, 0.0f, 0.0f };
                SESize2D destSize = {0.0f, 0.0f };
                SEColor color = {1.0f, 1.0f, 1.0f, 1.0f };
                bool flipX = false;
                bool flipY = false;
                SEVector2 origin = {0.0f, 0.0f };
                hasTexture = true;
                if (auto* spriteComp = node->GetComponentSafe<SpriteComp>()) {
                    renderTargetTexture = assetManager->GetTexture(spriteComp->texturePath.c_str());
                    sourceRect = spriteComp->drawSource;
                    destSize = { sourceRect.w, sourceRect.h };
                    color = spriteComp->modulate;
                    flipX = spriteComp->flipX;
                    flipY = spriteComp->flipY;
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
                            flipX = animSpriteComp->flipX;
                            flipY = animSpriteComp->flipY;
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
                    .flipX = flipX,
                    .flipY = flipY,
                    .globalTransform = &globalTransforms[index]
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
                            SETransformModel2D globalTransform = {transformComp->transform2D.position, transformComp->transform2D.scale, transformComp->transform2D.rotation };
                            cre_scene_utils_update_global_transform_model(node->GetUID(), &globalTransform);
                            static SEVector2 textLabelOrigin = {0.0f, 0.0f };
                            cre_scene_utils_apply_camera_and_origin_translation(&globalTransform, &textLabelOrigin, transformComp->ignoreCamera);
                            const ImGuiHelper::FontRenderTarget renderTarget = {
                                .font = assetManager->GetFont(textLabelComp->fontUID.c_str()),
                                .text = textLabelComp->text,
                                .position = globalTransform.position,
                                .scale = globalTransform.scale.x,
                                .color = textLabelComp->color
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
