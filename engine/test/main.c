#include "unity.h"

#include <stdbool.h>

#include <SDL2/SDL_main.h>

#include "../seika/src/utils/se_file_system_utils.h"

#include "../src/core/node_event.h"
#include "../src/core/ecs/component/component.h"
#include "../src/core/ecs/component/transform2d_component.h"
#include "../src/core/ecs/component/collider2d_component.h"
#include "../src/core/ecs/component/text_label_component.h"
#include "../src/core/ecs/system/ec_system.h"
#include "../src/core/json/json_file_loader.h"
#include "../src/core/scripting/python/pocketpy/cre_py_pp_util.h"
#include "../src/core/scripting/python/pocketpy/cre_py_api.h"
#include "../src/core/scripting/python/pocketpy/entity_instance_cache.h"

void setUp() {
    cre_component_manager_initialize();
}
void tearDown() {
    cre_component_manager_finalize(); // TODO: Need to implement
}

void cre_node_event_test(void);
void cre_json_file_loader_scene_test(void);
void cre_pocketpy_test(void);

int main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(cre_node_event_test);
    RUN_TEST(cre_json_file_loader_scene_test);
    RUN_TEST(cre_pocketpy_test);
    return UNITY_END();
}

//--- Node event test ---//
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
    const CreEntity eventEntity = 1;
    const CreEntity observerEntity = 2;
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

    const CreEntity anotherObserverEntity = 3;
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

//--- Json File Loader Tests ---//
// Note: If making changes to scene file make sure cmake runs steps to copy test dependency resources

#define TEST_SCENE_1_PATH "engine/test/resources/test_scene1.cscn"

void cre_json_file_loader_scene_test(void) {
    // ROOT NODE
    JsonSceneNode* rootNode = cre_json_load_scene_file(TEST_SCENE_1_PATH);
    TEST_ASSERT_NOT_NULL(rootNode);
    TEST_ASSERT_EQUAL_STRING("Main", rootNode->name);
    TEST_ASSERT_EQUAL_INT(NodeBaseType_NODE2D, rootNode->type);
    TEST_ASSERT_NULL(rootNode->tags);
    TEST_ASSERT_NULL(rootNode->externalNodeSource);
    TEST_ASSERT_EQUAL_UINT(2, rootNode->childrenCount);
    // Root components
    TEST_ASSERT_NOT_NULL(rootNode->components[CreComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* rootTransformComp = (Transform2DComponent*) rootNode->components[CreComponentDataIndex_TRANSFORM_2D];
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
    TEST_ASSERT_NOT_NULL(playerNode->components[CreComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* playerTransformComp = (Transform2DComponent*) playerNode->components[CreComponentDataIndex_TRANSFORM_2D];
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.position.x);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.position.y);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, playerTransformComp->localTransform.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(1.0f, playerTransformComp->localTransform.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, playerTransformComp->localTransform.rotation);
    TEST_ASSERT_EQUAL_INT(1, playerTransformComp->zIndex);
    TEST_ASSERT_TRUE(playerTransformComp->isZIndexRelativeToParent);
    TEST_ASSERT_FALSE(playerTransformComp->ignoreCamera);

    // BALL NODE (External scene child)
    JsonSceneNode* ballNode = rootNode->children[1];
    TEST_ASSERT_NOT_NULL(ballNode);
    TEST_ASSERT_EQUAL_STRING("TestBall", ballNode->name);
    TEST_ASSERT_EQUAL_INT(NodeBaseType_NODE2D, ballNode->type);
    TEST_ASSERT_NULL(ballNode->tags);
    TEST_ASSERT_EQUAL_STRING("engine/test/resources/ball.cscn", ballNode->externalNodeSource);
    TEST_ASSERT_EQUAL_INT(2, ballNode->childrenCount);
    // Ball components
    TEST_ASSERT_NOT_NULL(ballNode->components[CreComponentDataIndex_TRANSFORM_2D]);
    Transform2DComponent* ballTransformComp = (Transform2DComponent*) ballNode->components[CreComponentDataIndex_TRANSFORM_2D];
    // Testing to make sure position in current scene file overrides the default position in the 'ball.cscn'
    TEST_ASSERT_EQUAL_FLOAT(100.0f, ballTransformComp->localTransform.position.x);
    TEST_ASSERT_EQUAL_FLOAT(110.0f, ballTransformComp->localTransform.position.y);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, ballTransformComp->localTransform.scale.x);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, ballTransformComp->localTransform.scale.y);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, ballTransformComp->localTransform.rotation);
    TEST_ASSERT_EQUAL_INT(0, ballTransformComp->zIndex);
    TEST_ASSERT_TRUE(ballTransformComp->isZIndexRelativeToParent);
    TEST_ASSERT_FALSE(ballTransformComp->ignoreCamera);
    // BALL NODE CHILD COLLIDER
    JsonSceneNode* ballColliderNode = ballNode->children[0];
    TEST_ASSERT_NOT_NULL(ballColliderNode);
    Transform2DComponent* ballColliderTransformComp = (Transform2DComponent*) ballColliderNode->components[CreComponentDataIndex_TRANSFORM_2D];
    TEST_ASSERT_NOT_NULL(ballColliderTransformComp);
    Collider2DComponent* ballColliderCollider2DComp = (Collider2DComponent*) ballColliderNode->components[CreComponentDataIndex_COLLIDER_2D];
    TEST_ASSERT_NOT_NULL(ballColliderCollider2DComp);
    // BALL TEXT LABEL (Not in external scene but added as a child to the local scene)
    JsonSceneNode* ballTextLabel = ballNode->children[1];
    TEST_ASSERT_NOT_NULL(ballTextLabel);
    Transform2DComponent * ballTextTransform2DComp = (Transform2DComponent*) ballTextLabel->components[CreComponentDataIndex_TRANSFORM_2D];
    TEST_ASSERT_NOT_NULL(ballTextTransform2DComp);
    TextLabelComponent* ballTextLabelComp = (TextLabelComponent*) ballTextLabel->components[CreComponentDataIndex_TEXT_LABEL];
    TEST_ASSERT_NOT_NULL(ballTextLabelComp);


    cre_json_delete_json_scene_node(rootNode);
}

//--- Pocketpy Test ---//
#define POCKET_PY_TEST_SOURCE_PY "" \
"class Test:"\
"    def __init__(self) -> None:"\
"        self.value = 10"\
""

bool print_py_error_message(pkpy_vm* vm) {
    char* errorMessage = NULL;
    if (pkpy_clear_error(vm, &errorMessage)) {
        printf("[PY ERROR]:\n'%s'", errorMessage);
        pkpy_free(errorMessage);
        return true;
    }
    return false;
}

int pocketpy_test_node_get_name(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);
    pkpy_push_string(vm, pkpy_string("Chuko"));
    return 1;
}

int pocketpy_test_node_get_children(pkpy_vm* vm) {
    int entityId;
    pkpy_to_int(vm, 0, &entityId);
    const int idsToReturn = 5;
    for (int i = 0; i < idsToReturn; i++) {
        pkpy_push_int(vm, i + 1);
    }
    return idsToReturn;
}

void cre_pocketpy_test(void) {
    pkpy_vm* vm = pkpy_new_vm(true);

    TEST_MESSAGE("Testing loading internal modules");
    cre_pypp_api_load_internal_modules(vm);
    pkpy_exec(vm, "from crescent import Node");
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    pkpy_exec(vm, "print(f\"Node(0).entity_id = {Node(0).entity_id}\")");
    TEST_ASSERT_FALSE(print_py_error_message(vm));

    TEST_MESSAGE("Testing entity instance cache");
    cre_ec_system_initialize();  // Is needed for entity id generation
    cre_pypp_entity_instance_cache_initialize(vm);
    const CreEntity entity = cre_pypp_entity_instance_cache_create_new_entity(vm, "Node");
    cre_pypp_entity_instance_cache_push_entity_instance(vm, entity);
    TEST_ASSERT_EQUAL_INT(1, pkpy_stack_size(vm));

    cre_ec_system_finalize();
    cre_pypp_entity_instance_cache_finalize(vm);

    pkpy_delete_vm(vm);
}

void cre_pocketpy_test_old(void) {
    pkpy_vm* vm = pkpy_new_vm(true);

    // Test hard coded source
    pkpy_exec(vm, POCKET_PY_TEST_SOURCE_PY);
    TEST_ASSERT_FALSE(pkpy_check_error(vm));
    pkpy_exec(vm, "print(f\"{Test().value}\")");
    TEST_ASSERT_FALSE(pkpy_check_error(vm));
    pkpy_exec(vm, "print('Hello!')");
    TEST_ASSERT_FALSE(pkpy_check_error(vm));
    TEST_ASSERT_EQUAL_INT(0, pkpy_stack_size(vm));

    // Testing adding a module
    cre_py_pp_util_create_module(vm, &(CrePPModule){
        .name = "crescent_internal",
        .functionCount = 2,
        .functions = {
                { .signature = "node_get_name(entity_id: int) -> str", .function = pocketpy_test_node_get_name },
                { .signature = "node_get_children(entity_id: int) -> Tuple[int, ...]", .function = pocketpy_test_node_get_children },
        }
    });
    TEST_ASSERT_EQUAL_INT(0, pkpy_stack_size(vm));

    // Test source from file
    const char* PYTHON_SOURCE_FILE_PATH = "engine/test/resources/pocketpy_test.py";
    pkpy_push_module(vm, "pocketpy_test");
    char* pythonSourceFromFile = se_fs_read_file_contents(PYTHON_SOURCE_FILE_PATH, NULL);
    TEST_ASSERT_NOT_NULL(pythonSourceFromFile);
    pkpy_exec_2(vm, pythonSourceFromFile, PYTHON_SOURCE_FILE_PATH, 0, "pocketpy_test");
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    SE_MEM_FREE(pythonSourceFromFile);
    pkpy_exec(vm, "from pocketpy_test import Node, NodeManager");
    TEST_ASSERT_FALSE(print_py_error_message(vm));
//    pkpy_exec(vm, "print(f\"{Node(0).get_name()}\")");
    pkpy_exec(vm, "print(f\"{Node(0).name}\")");
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    pkpy_exec(vm, "print(f\"children = {Node(24).get_children()}\")");
    TEST_ASSERT_FALSE(print_py_error_message(vm));

    pkpy_exec(vm, "print(f\"{NodeManager().test_node}\")");
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    pkpy_exec(vm, "print(f\"{NodeManager().test_add(1, 2, 3)}\")");
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    pkpy_pop_top(vm);

    TEST_ASSERT_EQUAL_INT(0, pkpy_stack_size(vm));
    pkpy_getglobal(vm, pkpy_name("Node"));
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    pkpy_push_null(vm);
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    pkpy_push_int(vm, 1);
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    pkpy_vectorcall(vm, 1);
    TEST_ASSERT_FALSE(print_py_error_message(vm));
    int nodeEntity = 0;
    pkpy_getattr(vm, pkpy_name("entity_id"));
    pkpy_to_int(vm, 0, &nodeEntity);
    printf("node entity = '%d'", nodeEntity);

    // Test creating instance and getting attr
//    pkpy_getglobal(vm, pkpy_name("Test"));
//    pkpy_push_null(vm);
//    pkpy_vectorcall(vm, 0);
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    pkpy_getattr(vm, pkpy_name("value"));
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    int valueRet0 = 0;
//    pkpy_to_int(vm, -1, &valueRet0);
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    TEST_ASSERT_EQUAL_INT(10, valueRet0);
//    TEST_ASSERT_EQUAL_INT(1, pkpy_stack_size(vm));
//    pkpy_pop_top(vm);
//    // Test setting attr on instance
//    TEST_ASSERT_EQUAL_INT(0, pkpy_stack_size(vm));
//    pkpy_getglobal(vm, pkpy_name("Test"));
//    pkpy_push_null(vm);
//    pkpy_vectorcall(vm, 0);
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//
//    TEST_ASSERT_EQUAL_INT(1, pkpy_stack_size(vm));
//    pkpy_push_int(vm, 50);
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    pkpy_rot_two(vm);
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    pkpy_setattr(vm, pkpy_name("value"));
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    TEST_ASSERT_EQUAL_INT(0, pkpy_stack_size(vm));
//
//    pkpy_push_int(vm, 50);
//    pkpy_exec(vm, "test_inst = Test()");
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    pkpy_getglobal(vm, pkpy_name("test_inst"));
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    pkpy_setattr(vm, pkpy_name("value"));
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    TEST_ASSERT_EQUAL_INT(0, pkpy_stack_size(vm));
//    pkpy_getglobal(vm, pkpy_name("test_inst"));
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    pkpy_getattr(vm, pkpy_name("value"));
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    int valueRet1 = 0;
//    pkpy_to_int(vm, -1, &valueRet1);
//    TEST_ASSERT_FALSE(pkpy_check_error(vm));
//    TEST_ASSERT_EQUAL_INT(50, valueRet1);

    pkpy_delete_vm(vm);
}
