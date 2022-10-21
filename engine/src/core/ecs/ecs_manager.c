#include "ecs_manager.h"

#include <string.h>

#include "component/component.h"
#include "component/transform2d_component.h"
#include "component/text_label_component.h"
#include "system/animated_sprite_rendering_ec_system.h"
#include "system/collision_ec_system.h"
#include "system/color_rect_ec_system.h"
#include "system/ec_system.h"
#include "system/font_rendering_ec_system.h"
#include "system/script_ec_system.h"
#include "system/sprite_rendering_ec_system.h"
#include "../scene/scene_manager.h"
#include "../seika/src/utils/se_assert.h"
#include "../seika/src/asset_manager.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

static SceneTreeNode* fpsDisplayNode = NULL;

void cre_ecs_manager_initialize() {
    component_manager_initialize();
    cre_ec_system_initialize();
    // Initialize and register ec systems
    cre_ec_system_register(sprite_rendering_ec_system_create());
    cre_ec_system_register(animated_sprite_rendering_ec_system_create());
    cre_ec_system_register(font_rendering_ec_system_create());
    cre_ec_system_register(script_ec_system_create());
    cre_ec_system_register(collision_ec_system_create());
    cre_ec_system_register(color_rect_ec_system_create());
}

void cre_ecs_manager_enable_fps_display_entity(bool enabled) {
    static bool isEnabled = false;
    static Entity currentFpsEntity = NULL_ENTITY;
    // Create temp entity
    if (!isEnabled && enabled) {
        fpsDisplayNode = cre_scene_tree_create_tree_node(cre_ec_system_create_entity_uid(), NULL);
        currentFpsEntity = fpsDisplayNode->entity;
        // Transform 2D
        Transform2DComponent* transform2DComponent = transform2d_component_create();
        transform2DComponent->localTransform.position.x = 20.0f;
        transform2DComponent->localTransform.position.y = 30.0f;
        transform2DComponent->ignoreCamera = true;
        component_manager_set_component(currentFpsEntity, ComponentDataIndex_TRANSFORM_2D, transform2DComponent);
        // Text Label Component
        TextLabelComponent* textLabelComponent = text_label_component_create();
        textLabelComponent->font = se_asset_manager_get_font("verdana-32");
        strcpy(textLabelComponent->text, "FPS: ");
        component_manager_set_component(currentFpsEntity, ComponentDataIndex_TEXT_LABEL, textLabelComponent);
        // Script Component
        ScriptComponent* scriptComponent = script_component_create("main", "FpsDisplay");
        scriptComponent->contextType = ScriptContextType_NATIVE;
        component_manager_set_component(currentFpsEntity, ComponentDataIndex_SCRIPT, scriptComponent);
        // Update systems
        cre_ec_system_update_entity_signature_with_systems(currentFpsEntity);
        cre_scene_manager_queue_entity_for_creation(fpsDisplayNode);
    } else if (isEnabled && !enabled) {
        SE_ASSERT_FMT(currentFpsEntity != NULL_ENTITY, "Current fps entity is a null entity!?");
        SE_ASSERT_FMT(fpsDisplayNode != NULL, "FPS Display Node is NULL!?");
        cre_queue_destroy_tree_node_entity_all(fpsDisplayNode);
        currentFpsEntity = NULL_ENTITY;
        fpsDisplayNode = NULL;
    }
    isEnabled = enabled;
}

void cre_ecs_manager_finalize() {
    cre_ec_system_finalize();
    component_manager_finalize();
}
