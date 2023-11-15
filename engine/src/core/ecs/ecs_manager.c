#include "ecs_manager.h"

#include <string.h>

#include "seika/rendering/renderer.h"
#include "seika/utils/se_assert.h"
#include "seika/asset/asset_manager.h"

#include "component/component.h"
#include "component/transform2d_component.h"
#include "component/text_label_component.h"
#include "system/animated_sprite_rendering_ec_system.h"
#include "system/collision_ec_system.h"
#include "system/color_rect_ec_system.h"
#include "system/ec_system.h"
#include "system/font_rendering_ec_system.h"
#include "system/parallax_ec_system.h"
#include "system/script_ec_system.h"
#include "system/sprite_rendering_ec_system.h"
#include "../scene/scene_manager.h"
#include "../game_properties.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

static SceneTreeNode* fpsDisplayNode = NULL;

void cre_ecs_manager_initialize() {
    cre_component_manager_initialize();
    cre_ec_system_initialize();
    // Initialize and register ec systems
    cre_ec_system_register(cre_sprite_rendering_ec_system_create());
    cre_ec_system_register(cre_animated_sprite_rendering_ec_system_create());
    cre_ec_system_register(cre_font_rendering_ec_system_create());
    cre_ec_system_register(cre_script_ec_system_create());
    cre_ec_system_register(cre_collision_ec_system_create());
    cre_ec_system_register(cre_color_rect_ec_system_create());
    cre_ec_system_register(cre_parallax_ec_system_create());
}

void cre_ecs_manager_initialize_ex(struct SETexture* colorRectTexture) {
    cre_component_manager_initialize();
    cre_ec_system_initialize();
    // Initialize and register ec systems
    cre_ec_system_register(cre_sprite_rendering_ec_system_create());
    cre_ec_system_register(cre_animated_sprite_rendering_ec_system_create());
    cre_ec_system_register(cre_font_rendering_ec_system_create());
    cre_ec_system_register(cre_script_ec_system_create());
    cre_ec_system_register(cre_collision_ec_system_create());
    cre_ec_system_register(cre_color_rect_ec_system_create_ex(colorRectTexture));
    cre_ec_system_register(cre_parallax_ec_system_create());
}

void cre_ecs_manager_enable_fps_display_entity(bool enabled, const char* fontUID, float positionX, float positionY) {
    static bool isEnabled = false;
    static CreEntity currentFpsEntity = CRE_NULL_ENTITY;
    // Create temp entity
    if (!isEnabled && enabled) {
        fpsDisplayNode = cre_scene_tree_create_tree_node(cre_ec_system_create_entity_uid(), NULL);
        currentFpsEntity = fpsDisplayNode->entity;
        // Transform 2D
        Transform2DComponent* transform2DComponent = transform2d_component_create();
        transform2DComponent->localTransform.position.x = positionX;
        transform2DComponent->localTransform.position.y = positionY;
        transform2DComponent->ignoreCamera = true;
        transform2DComponent->zIndex = SE_RENDERER_MAX_Z_INDEX;
        cre_component_manager_set_component(currentFpsEntity, CreComponentDataIndex_TRANSFORM_2D, transform2DComponent);
        // Text Label Component
        TextLabelComponent* textLabelComponent = text_label_component_create();
        textLabelComponent->font = se_asset_manager_get_font(fontUID != NULL ? fontUID : CRE_DEFAULT_FONT_ASSET.uid);
        strcpy(textLabelComponent->text, "FPS: ");
        cre_component_manager_set_component(currentFpsEntity, CreComponentDataIndex_TEXT_LABEL, textLabelComponent);
        // Script Component
        ScriptComponent* scriptComponent = script_component_create("main", "FpsDisplay");
        scriptComponent->contextType = ScriptContextType_NATIVE;
        cre_component_manager_set_component(currentFpsEntity, CreComponentDataIndex_SCRIPT, scriptComponent);
        // Update systems
        cre_ec_system_update_entity_signature_with_systems(currentFpsEntity);
        cre_scene_manager_queue_node_for_creation(fpsDisplayNode);
    } else if (isEnabled && !enabled) {
        SE_ASSERT_FMT(currentFpsEntity != CRE_NULL_ENTITY, "Current fps entity is a null entity!?");
        SE_ASSERT_FMT(fpsDisplayNode != NULL, "FPS Display Node is NULL!?");
        cre_queue_destroy_tree_node_entity_all(fpsDisplayNode);
        currentFpsEntity = CRE_NULL_ENTITY;
        fpsDisplayNode = NULL;
    }
    isEnabled = enabled;
}

void cre_ecs_manager_finalize() {
    cre_ec_system_finalize();
    cre_component_manager_finalize();
}
