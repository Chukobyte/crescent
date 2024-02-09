#include "parallax_ec_system.h"

#include <seika/ecs/ecs.h>

#include "../ecs_globals.h"
//#include "ec_system.h"
#include "../component/transform2d_component.h"
#include "../component/parallax_component.h"
#include "../../camera/camera.h"
#include "../../camera/camera_manager.h"
#include "../component/component.h"

static void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity);
static void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity);
static void fixed_update(SkaECSSystem* system, float deltaTime);

static void on_entity_transform_change(SESubjectNotifyPayload* payload);

static void parallax_system_update_entity(CreEntity entity, Transform2DComponent* transformComp, ParallaxComponent* parallaxComp, CRECamera2D* camera2D);

SEObserver parallaxOnEntityTransformChangeObserver = { .on_notify = on_entity_transform_change };

void cre_parallax_ec_system_create_and_register() {
    SkaECSSystemTemplate systemTemplate = ska_ecs_system_create_default_template("Parallax");
    systemTemplate.on_entity_entered_scene_func = on_entity_entered_scene;
    systemTemplate.on_entity_unregistered_func = on_entity_unregistered;
    systemTemplate.fixed_update_func = fixed_update;
    SKA_ECS_SYSTEM_REGISTER_FROM_TEMPLATE(&systemTemplate, Transform2DComponent, ParallaxComponent);
}

void on_entity_entered_scene(SkaECSSystem* system, SkaEntity entity) {
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    ParallaxComponent* parallaxComp = (ParallaxComponent*)ska_ecs_component_manager_get_component(entity, PARTICLES2D_COMPONENT_INDEX);
    parallaxComp->cachedScrollSpeed = parallaxComp->scrollSpeed;
    parallaxComp->cachedLocalPosition = transformComp->localTransform.position;
    parallax_system_update_entity(entity, transformComp, parallaxComp, cre_camera_manager_get_current_camera());
    // Add observer
    se_event_register_observer(&transformComp->onTransformChanged, &parallaxOnEntityTransformChangeObserver);
}

void on_entity_unregistered(SkaECSSystem* system, SkaEntity entity) {
    // Remove observer
    Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
    se_event_unregister_observer(&transformComp->onTransformChanged, &parallaxOnEntityTransformChangeObserver);
}

void fixed_update(SkaECSSystem* system, float deltaTime) {
    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
    for (size_t i = 0; i < system->entity_count; i++) {
        const CreEntity entity = system->entities[i];
        Transform2DComponent* transformComp = (Transform2DComponent*)ska_ecs_component_manager_get_component(entity, TRANSFORM2D_COMPONENT_INDEX);
        ParallaxComponent* parallaxComp = (ParallaxComponent*)ska_ecs_component_manager_get_component(entity, PARALLAX_COMPONENT_INDEX);
        parallax_system_update_entity(entity, transformComp, parallaxComp, camera2D);
    }
}

void parallax_system_update_entity(CreEntity entity, Transform2DComponent* transformComp, ParallaxComponent* parallaxComp, CRECamera2D* camera2D) {
    const SKAVector2 offset = {
        .x = (parallaxComp->cachedLocalPosition.x - (camera2D->viewport.x + camera2D->offset.x)) * camera2D->zoom.x * parallaxComp->scrollSpeed.x,
        .y = (parallaxComp->cachedLocalPosition.y - (camera2D->viewport.y + camera2D->offset.y)) * camera2D->zoom.y * parallaxComp->scrollSpeed.y
    };
    transformComp->localTransform.position.x = parallaxComp->cachedLocalPosition.x + offset.x;
    transformComp->localTransform.position.y = parallaxComp->cachedLocalPosition.y + offset.y;
}

// Observer callbacks
void on_entity_transform_change(SESubjectNotifyPayload* payload) {
    CreComponentEntityUpdatePayload* updatePayload = (CreComponentEntityUpdatePayload*)payload->data;
    Transform2DComponent* transform2DComponent = (Transform2DComponent*)updatePayload->component;
    const CreEntity entity = updatePayload->entity;

    ParallaxComponent* parallaxComp = (ParallaxComponent*)ska_ecs_component_manager_get_component(entity, PARALLAX_COMPONENT_INDEX);

    parallaxComp->cachedLocalPosition = transform2DComponent->localTransform.position;
    parallax_system_update_entity(entity, transform2DComponent, parallaxComp, cre_camera_manager_get_current_camera());
}

//void parallax_system_on_entity_entered_scene(CreEntity entity);
//void parallax_system_on_entity_unregistered(CreEntity entity);
//void parallax_system_physics_process(float deltaTime);
//void parallax_system_on_ec_system_destroy();
//
//void parallax_on_entity_transform_change(SESubjectNotifyPayload* payload);
//
//void parallax_system_update_entity(CreEntity entity, Transform2DComponent* transformComp, ParallaxComponent* parallaxComp, CRECamera2D* camera2D);
//
//CreEntitySystem* parallaxSystem = NULL;
//
//SEObserver parallaxOnEntityTransformChangeObserver = { .on_notify = parallax_on_entity_transform_change };
//
//struct CreEntitySystem* cre_parallax_ec_system_create() {
//    SE_ASSERT(parallaxSystem == NULL);
//    parallaxSystem = cre_ec_system_create();
//    parallaxSystem->name = se_strdup("Parallax");
//    parallaxSystem->on_entity_entered_scene_func = parallax_system_on_entity_entered_scene;
//    parallaxSystem->on_entity_unregistered_func = parallax_system_on_entity_unregistered;
//    parallaxSystem->fixed_update_func = parallax_system_physics_process;
//    parallaxSystem->on_ec_system_destroy = parallax_system_on_ec_system_destroy;
//    parallaxSystem->component_signature = CreComponentType_TRANSFORM_2D | CreComponentType_PARALLAX;
//    return parallaxSystem;
//}
//
//void parallax_system_on_entity_entered_scene(CreEntity entity) {
//    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component_unchecked(entity,
//                                          CreComponentDataIndex_TRANSFORM_2D);
//    ParallaxComponent* parallaxComp = (ParallaxComponent*) cre_component_manager_get_component_unchecked(entity,
//                                      CreComponentDataIndex_PARALLAX);
//    SE_ASSERT(transformComp != NULL);
//    SE_ASSERT(parallaxComp != NULL);
//    parallaxComp->cachedScrollSpeed = parallaxComp->scrollSpeed;
//    parallaxComp->cachedLocalPosition = transformComp->localTransform.position;
//    parallax_system_update_entity(entity, transformComp, parallaxComp, cre_camera_manager_get_current_camera());
//    // Add observer
//    se_event_register_observer(&transformComp->onTransformChanged, &parallaxOnEntityTransformChangeObserver);
//}
//
//void parallax_system_on_entity_unregistered(CreEntity entity) {
//    // Remove observer
//    Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component_unchecked(entity,
//                                          CreComponentDataIndex_TRANSFORM_2D);
//    SE_ASSERT(transformComp != NULL);
//    se_event_unregister_observer(&transformComp->onTransformChanged, &parallaxOnEntityTransformChangeObserver);
//}
//
//void parallax_system_physics_process(float deltaTime) {
//    CRECamera2D* camera2D = cre_camera_manager_get_current_camera();
//    for (size_t i = 0; i < parallaxSystem->entity_count; i++) {
//        const CreEntity entity = parallaxSystem->entities[i];
//        Transform2DComponent* transformComp = (Transform2DComponent*) cre_component_manager_get_component(entity,CreComponentDataIndex_TRANSFORM_2D);
//        ParallaxComponent* parallaxComp = (ParallaxComponent*) cre_component_manager_get_component(entity,CreComponentDataIndex_PARALLAX);
//        parallax_system_update_entity(entity, transformComp, parallaxComp, camera2D);
//    }
//}
//
//void parallax_system_update_entity(CreEntity entity, Transform2DComponent* transformComp, ParallaxComponent* parallaxComp, CRECamera2D* camera2D) {
//    const SKAVector2 offset = {
//        .x = (parallaxComp->cachedLocalPosition.x - (camera2D->viewport.x + camera2D->offset.x)) * camera2D->zoom.x * parallaxComp->scrollSpeed.x,
//        .y = (parallaxComp->cachedLocalPosition.y - (camera2D->viewport.y + camera2D->offset.y)) * camera2D->zoom.y * parallaxComp->scrollSpeed.y
//    };
//    transformComp->localTransform.position.x = parallaxComp->cachedLocalPosition.x + offset.x;
//    transformComp->localTransform.position.y = parallaxComp->cachedLocalPosition.y + offset.y;
//}
//
//void parallax_system_on_ec_system_destroy() {
//    SE_ASSERT(parallaxSystem != NULL);
//    parallaxSystem = NULL;
//}
//
//// Observer callbacks
//void parallax_on_entity_transform_change(SESubjectNotifyPayload* payload) {
//    CreComponentEntityUpdatePayload* updatePayload = (CreComponentEntityUpdatePayload*) payload->data;
//    Transform2DComponent* transform2DComponent = (Transform2DComponent*) updatePayload->component;
//    CreEntity entity = updatePayload->entity;
//
//    ParallaxComponent* parallaxComp = (ParallaxComponent*) cre_component_manager_get_component(entity, CreComponentDataIndex_PARALLAX);
//    SE_ASSERT(parallaxComp != NULL);
//
//    parallaxComp->cachedLocalPosition = transform2DComponent->localTransform.position;
//    parallax_system_update_entity(entity, transform2DComponent, parallaxComp, cre_camera_manager_get_current_camera());
//}
