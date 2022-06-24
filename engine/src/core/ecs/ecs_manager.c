#include "ecs_manager.h"

#include <string.h>

#include "component/component.h"
#include "component/transform2d_component.h"
#include "component/text_label_component.h"
#include "system/animated_sprite_rendering_ec_system.h"
#include "system/collision_ec_system.h"
#include "system/ec_system.h"
#include "system/font_rendering_ec_system.h"
#include "system/script_ec_system.h"
#include "system/sprite_rendering_ec_system.h"
#include "../scene/scene_manager.h"
#include "../utils/rbe_assert.h"
#include "../asset_manager.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

void rbe_ecs_manager_initialize() {
    component_manager_initialize();
    rbe_ec_system_initialize();
    // Initialize and register ec systems
    rbe_ec_system_register(sprite_rendering_ec_system_create());
    rbe_ec_system_register(animated_sprite_rendering_ec_system_create());
    rbe_ec_system_register(font_rendering_ec_system_create());
    rbe_ec_system_register(script_ec_system_create());
    rbe_ec_system_register(collision_ec_system_create());
}

void rbe_ecs_manager_enable_fps_display_entity(bool enabled) {
    static bool isEnabled = false;
    static Entity currentFpsEntity = NULL_ENTITY;
    // Create temp entity
    if (!isEnabled && enabled) {
        SceneTreeNode* node = rbe_scene_tree_create_tree_node(rbe_ec_system_create_entity(), NULL_ENTITY);
        currentFpsEntity = node->entity;
        // Transform 2D
        Transform2DComponent* transform2DComponent = transform2d_component_create();
        transform2DComponent->position.x = 20.0f;
        transform2DComponent->position.y = 30.0f;
        component_manager_set_component(currentFpsEntity, ComponentDataIndex_TRANSFORM_2D, transform2DComponent);
        // Text Label Component
        TextLabelComponent* textLabelComponent = text_label_component_create();
        textLabelComponent->font = rbe_asset_manager_get_font("verdana-32");
        strcpy(textLabelComponent->text, "FPS: ");
        component_manager_set_component(currentFpsEntity, ComponentDataIndex_TEXT_LABEL, textLabelComponent);
        // Script Component
        ScriptComponent* scriptComponent = script_component_create();
        scriptComponent->contextType = ScriptContextType_NATIVE;
        scriptComponent->classPath = "main";
        scriptComponent->className = "FpsDisplay";
        component_manager_set_component(currentFpsEntity, ComponentDataIndex_SCRIPT, scriptComponent);
        // Update systems
        rbe_ec_system_update_entity_signature_with_systems(currentFpsEntity);
        rbe_scene_manager_queue_entity_for_creation(node);
    } else if (isEnabled && !enabled) {
        RBE_ASSERT(currentFpsEntity != NULL_ENTITY);
        rbe_scene_manager_queue_entity_for_deletion(currentFpsEntity);
        currentFpsEntity = NULL_ENTITY;
    }
    isEnabled = enabled;
}

void rbe_ecs_manager_finalize() {
    rbe_ec_system_finalize();
    component_manager_finalize();
}
