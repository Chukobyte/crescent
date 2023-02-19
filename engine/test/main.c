#include "unity.h"

#include <stdbool.h>

#include <SDL2/SDL_main.h>

#include "../src/core/node_event.h"
#include "../src/core/ecs/component/component.h"
#include "../src/core/ecs/component/transform2d_component.h"
#include "../src/core/json/json_file_loader.h"

void setUp() {
    component_manager_initialize();
}
void tearDown() {
    component_manager_finalize(); // TODO: Need to implement
}

void cre_node_event_test(void);
void cre_json_file_loader_scene_test(void);

int main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(cre_node_event_test);
    RUN_TEST(cre_json_file_loader_scene_test);
    return UNITY_END();
}

// Node event test
static bool hasBeenNotified = false;
static int NODE_EVENT_TEST_NUMBER = 345;

void node_event_callback(void* observerData, NodeEventNotifyPayload* notifyPayload) {
    hasBeenNotified = true;
}

void node_event_callback2(void* observerData, NodeEventNotifyPayload* notifyPayload) {
    const int number = *(int*) notifyPayload->data;
    if (number == NODE_EVENT_TEST_NUMBER) {
        hasBeenNotified = true;
    }
}

void cre_node_event_test(void) {
    const Entity eventEntity = 1;
    const Entity observerEntity = 2;
    const char* eventId = "walk";

    // Test Empty
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_count(eventEntity));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(observerEntity));

    node_event_create_event(eventEntity, eventId);
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_event_count(eventEntity));

    node_event_subscribe_to_event(eventEntity, eventId, observerEntity, node_event_callback, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_entity_observer_count(observerEntity));

    // Test notify
    node_event_notify_observers(eventEntity, eventId, &(NodeEventNotifyPayload){ .data = NULL });
    TEST_ASSERT_TRUE(hasBeenNotified);
    hasBeenNotified = false;

    const Entity anotherObserverEntity = 3;
    node_event_subscribe_to_event(eventEntity, eventId, anotherObserverEntity, node_event_callback2, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT(2, node_event_get_event_observer_count(eventEntity, eventId));
    node_event_notify_observers(eventEntity, eventId, &(NodeEventNotifyPayload){ .data = &NODE_EVENT_TEST_NUMBER });
    TEST_ASSERT_TRUE(hasBeenNotified);
    node_event_destroy_all_entity_events_and_observers(anotherObserverEntity);
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_event_observer_count(eventEntity, eventId));

    node_event_destroy_all_entity_events_and_observers(eventEntity);
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_count(eventEntity));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(observerEntity));

    // Test to make sure entity event removal also removes observer entries
    node_event_create_event(eventEntity, eventId);
    node_event_subscribe_to_event(eventEntity, eventId, observerEntity, node_event_callback, NULL, NULL);
    node_event_subscribe_to_event(eventEntity, eventId, anotherObserverEntity, node_event_callback, NULL, NULL);
    TEST_ASSERT_EQUAL_UINT(2, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_entity_observer_count(observerEntity));
    TEST_ASSERT_EQUAL_UINT(1, node_event_get_entity_observer_count(anotherObserverEntity));
    node_event_destroy_all_entity_events_and_observers(eventEntity);
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_event_observer_count(eventEntity, eventId));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(observerEntity));
    TEST_ASSERT_EQUAL_UINT(0, node_event_get_entity_observer_count(anotherObserverEntity));
}

// Json File Loader Tests
#define TEST_SCENE_1_PATH "engine/test/resources/test_scene1.cscn"

void cre_json_file_loader_scene_test(void) {
    // ROOT NODE
    JsonSceneNode* rootNode = cre_json_load_scene_file(TEST_SCENE_1_PATH);
    TEST_ASSERT_NOT_NULL(rootNode);
    TEST_ASSERT_EQUAL_STRING("Main", rootNode->name);
    TEST_ASSERT_EQUAL_INT(NodeBaseType_NODE2D, rootNode->type);
    TEST_ASSERT_NULL(rootNode->tags);
    TEST_ASSERT_NULL(rootNode->externalNodeSource);
    TEST_ASSERT_EQUAL_UINT(1, rootNode->childrenCount);
    // Root components
    TEST_ASSERT_NOT_NULL(rootNode->components[ComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* rootTransformComp = (Transform2DComponent*) rootNode->components[ComponentDataIndex_TRANSFORM_2D];
    TEST_ASSERT_EQUAL_FLOAT(0.0f, rootTransformComp->localTransform.position.x);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, rootTransformComp->localTransform.position.y);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, rootTransformComp->localTransform.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, rootTransformComp->localTransform.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, rootTransformComp->localTransform.rotation);
    TEST_ASSERT_EQUAL_INT(0, rootTransformComp->zIndex);
    TEST_ASSERT_TRUE(rootTransformComp->isZIndexRelativeToParent);
    TEST_ASSERT_FALSE(rootTransformComp->ignoreCamera);

    // PLAYER NODE (Child)
    JsonSceneNode* playerNode = rootNode->children[0];
    TEST_ASSERT_NOT_NULL(playerNode);
    TEST_ASSERT_EQUAL_STRING("Player", playerNode->name);
    TEST_ASSERT_EQUAL_INT(NodeBaseType_NODE2D, playerNode->type);
    TEST_ASSERT_NULL(playerNode->tags);
    TEST_ASSERT_NULL(playerNode->externalNodeSource);
    TEST_ASSERT_EQUAL_INT(0, playerNode->childrenCount);
    // Player components
    TEST_ASSERT_NOT_NULL(playerNode->components[ComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* playerTransformComp = (Transform2DComponent*) playerNode->components[ComponentDataIndex_TRANSFORM_2D];
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.position.x);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.position.y);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, playerTransformComp->localTransform.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, playerTransformComp->localTransform.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.rotation);
    TEST_ASSERT_EQUAL_INT(1, playerTransformComp->zIndex);
    TEST_ASSERT_TRUE(playerTransformComp->isZIndexRelativeToParent);
    TEST_ASSERT_FALSE(playerTransformComp->ignoreCamera);

    cre_json_delete_json_scene_node(rootNode);
}
