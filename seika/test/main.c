#include "unity.h"

#include <SDL2/SDL_main.h>

#include "../src/data_structures/se_array_utils.h"
#include "../src/data_structures/se_array_list.h"
#include "../src/data_structures/se_spatial_hash_map.h"
#include "../src/asset/asset_file_loader.h"
#include "../src/utils/se_string_util.h"
#include "../src/utils/se_file_system_utils.h"
#include "../src/utils/observer.h"
#include "../src/utils/se_profile_code.h"
#include "../src/math/se_curve_float.h"
#include "../src/rendering/shader/shader_instance.h"
#include "../src/rendering/shader/shader_file_parser.h"

#define RESOURCES_PATH "seika/test/resources"
#define RESOURCES_PACK_PATH "seika/test/resources/test.pck"

void setUp() {}
void tearDown() {}

void seika_hash_map_test(void);
void seika_spatial_hash_map_test(void);
void seika_file_system_utils_test(void);
void seika_string_utils_test(void);
void seika_array_utils_test(void);
void seika_array_list_test(void);
void seika_asset_file_loader_test(void);
void seika_observer_test(void);
void seika_curve_float_test(void);
void seika_shader_instance_test(void);
void seika_shader_file_parser_test(void);

int main(int argv, char** args) {
    UNITY_BEGIN();
//    RUN_TEST(seika_hash_map_test);
//    RUN_TEST(seika_spatial_hash_map_test);
//    RUN_TEST(seika_file_system_utils_test);
//    RUN_TEST(seika_string_utils_test);
//    RUN_TEST(seika_array_utils_test);
//    RUN_TEST(seika_array_list_test);
//    RUN_TEST(seika_asset_file_loader_test);
//    RUN_TEST(seika_observer_test);
//    RUN_TEST(seika_curve_float_test);
//    RUN_TEST(seika_shader_instance_test);
    RUN_TEST(seika_shader_file_parser_test);
    return UNITY_END();
}

void seika_hash_map_test(void) {
    SEHashMap* hashMap = se_hash_map_create(sizeof(int), sizeof(int), SE_HASH_MAP_MIN_CAPACITY);
    TEST_ASSERT_NOT_NULL(hashMap);

    int key1 = 0;
    int value1 = 11;
    se_hash_map_add(hashMap, &key1, &value1);
    TEST_ASSERT_EQUAL_INT(1, hashMap->size);
    int returnedValue1 = *(int*) se_hash_map_get(hashMap, &key1);
    TEST_ASSERT_EQUAL_INT(value1, returnedValue1);

    int key2 = 1;
    int value2 = 22;
    se_hash_map_add(hashMap, &key2, &value2);
    TEST_ASSERT_EQUAL_INT(2, hashMap->size);

    // Iterator test
    int iterCount = 0;
    for (SEHashMapIterator iterator = se_hash_map_iter_create(hashMap); se_hash_map_iter_is_valid(hashMap, &iterator); se_hash_map_iter_advance(hashMap, &iterator)) {
        iterCount++;
    }
    TEST_ASSERT_EQUAL_INT(2, iterCount);
    // Iter Macro test
    iterCount = 0;
    SE_HASH_MAP_FOR_EACH(hashMap, iter) {
        iterCount++;
    }
    TEST_ASSERT_EQUAL_INT(2, iterCount);

    se_hash_map_destroy(hashMap);
}

void seika_spatial_hash_map_test(void) {
    const int maxSpriteSize = 32;
    SESpatialHashMap* spatialHashMap = se_spatial_hash_map_create(maxSpriteSize * 2);
    TEST_ASSERT_NOT_NULL(spatialHashMap);

    // Create two entities and insert them into hash map
    const unsigned int entity = 1;
    SESpatialHashMapGridSpacesHandle* handle = se_spatial_hash_map_insert_or_update(spatialHashMap, entity, &(SERect2) {
        0.0f, 0.0f, 32.0f, 32.0f
    });
    TEST_ASSERT_EQUAL(handle, se_spatial_hash_map_get(spatialHashMap, entity));

    const unsigned int entityTwo = 2;
    SESpatialHashMapGridSpacesHandle* handleTwo = se_spatial_hash_map_insert_or_update(spatialHashMap, entityTwo, &(SERect2) {
        16.0f, 16.0f, 48.0f, 48.0f
    });
    TEST_ASSERT_EQUAL(handleTwo, se_spatial_hash_map_get(spatialHashMap, entityTwo));

    // An entity that should not be collided with
    const unsigned int entityNotCollided = 3;
    se_spatial_hash_map_insert_or_update(spatialHashMap, entityNotCollided, &(SERect2) {
        64.0f, 64.0f, 96.0f, 96.0f
    });

    // Test collision result to make sure the two entities collide
    const SESpatialHashMapCollisionResult collisionResult = se_spatial_hash_map_compute_collision(spatialHashMap, entity);
    TEST_ASSERT_EQUAL_INT(1, collisionResult.collisionCount);

    if (collisionResult.collisionCount > 0) {
        TEST_ASSERT_EQUAL_INT(2, collisionResult.collisions[0]);
    }

    se_spatial_hash_map_remove(spatialHashMap, entity);
    TEST_ASSERT_NULL(se_spatial_hash_map_get(spatialHashMap, entity));
    se_spatial_hash_map_remove(spatialHashMap, entityTwo);
    TEST_ASSERT_NULL(se_spatial_hash_map_get(spatialHashMap, entityTwo));

    se_spatial_hash_map_destroy(spatialHashMap);
}

void seika_file_system_utils_test(void) {
    // File tests
    TEST_ASSERT_TRUE(se_fs_does_file_exist(RESOURCES_PACK_PATH));
    TEST_ASSERT_FALSE(se_fs_does_file_exist("somemadeupfile.txt"));
    // Directory tests
    TEST_ASSERT_TRUE(se_fs_does_dir_exist(RESOURCES_PATH));
    TEST_ASSERT_FALSE(se_fs_does_dir_exist("something/somemadeupath"));
}

void seika_string_utils_test(void) {
    // Test string dup
    char* filePath = se_strdup("project.cscn");
    TEST_ASSERT_EQUAL_STRING("project.cscn", filePath);
    // Test trim
    char* filePathWithoutExtension = se_str_trim(filePath, '.');
    TEST_ASSERT_EQUAL_STRING("project", filePathWithoutExtension);
    // Testing trim that doesn't trim
    char* dupFilePathWithoutExtension = se_str_trim(filePathWithoutExtension, '.');
    TEST_ASSERT_EQUAL_STRING("project", dupFilePathWithoutExtension);
    // Test trim replacement
    char* filePathZip = se_str_trim_and_replace(filePath, '.', ".zip");
    TEST_ASSERT_EQUAL_STRING("project.zip", filePathZip);
    // Clean up
    SE_MEM_FREE(filePath);
    SE_MEM_FREE(filePathWithoutExtension);
    SE_MEM_FREE(dupFilePathWithoutExtension);
    SE_MEM_FREE(filePathZip);
}

void seika_array_utils_test(void) {
#define ARRAY_SIZE 6

    const uint32_t invalidValue = 999999;
    size_t arraySize = ARRAY_SIZE;
    uint32_t array[ARRAY_SIZE];

    for (size_t i = 0; i < arraySize; i++) {
        array[i] = (uint32_t)i;
    }

    TEST_ASSERT_EQUAL_INT(4, array[4]);
    se_array_utils_remove_item_uint32(array, &arraySize, 4, invalidValue);
    TEST_ASSERT_NOT_EQUAL_INT(4, array[4]);
    TEST_ASSERT_EQUAL_INT(ARRAY_SIZE - 1, arraySize);

#undef ARRAY_SIZE
}

bool array_list_test_sort(void* a, void* b) {
    int numA = (int) *(int*) a;
    int numB = (int) *(int*) b;
    return numA > numB;
}

void seika_array_list_test(void) {
    SEArrayList* list = se_array_list_create(sizeof(int));
    // Test prepend
    int num1 = 9;
    int num2 = 7;
    se_array_list_prepend(list, &num1);
    const int returnedNum = (int) *(int*) se_array_list_get_front(list);
    TEST_ASSERT_EQUAL_INT(1, list->size);
    TEST_ASSERT_EQUAL_INT(num1, returnedNum);
    se_array_list_prepend(list, &num2);
    TEST_ASSERT_EQUAL_INT(2, list->size);
    se_array_list_remove(list, &num1);
    TEST_ASSERT_EQUAL_INT(1, list->size);
    int* returnedNum2 = (int*) se_array_list_pop_front(list);
    TEST_ASSERT_EQUAL_INT(7, *returnedNum2);
    TEST_ASSERT_TRUE(se_array_list_is_empty(list));
    SE_MEM_FREE(returnedNum2);

    int numbers[5];
    for (int i = 0; i < 5; i++) {
        numbers[i] = i;
        se_array_list_append(list, &i);
    }
    TEST_ASSERT_EQUAL_INT(5, list->size);

    // For each loop test
    SEArrayListNode* node = NULL;
    int loopIndex = 0;
    SE_ARRAY_LIST_FOR_EACH(list, node) {
        const int indexNum = (int) *(int*) node->value;
        TEST_ASSERT_EQUAL_INT(indexNum, numbers[loopIndex]);
        loopIndex++;
    }

    const int indexThreeNum = (int) *(int*) se_array_list_get(list, 3);
    TEST_ASSERT_EQUAL_INT(3, indexThreeNum);

    // Insert Test
    int insertNum1 = 10;
    int insertNum2 = 20;
    se_array_list_insert(list, &insertNum1, 3);
    TEST_ASSERT_EQUAL_INT(6, list->size);
    se_array_list_insert(list, &insertNum2, 1);
    TEST_ASSERT_EQUAL_INT(7, list->size);
    const int insertNumThree = (int) *(int*) se_array_list_get(list, 3);
    TEST_ASSERT_EQUAL_INT(10, insertNumThree);
    const int insertNumOne = (int) *(int*) se_array_list_get(list, 1);
    TEST_ASSERT_EQUAL_INT(20, insertNumOne);

    se_array_list_sort(list, array_list_test_sort);
    const int smallestNum = (int) *(int*) se_array_list_get_front(list);
    const int highestNum = (int) *(int*) se_array_list_get_back(list);
    TEST_ASSERT_EQUAL_INT(0, smallestNum);
    TEST_ASSERT_EQUAL_INT(20, highestNum);

    // Clear test
    se_array_list_clear(list);
    TEST_ASSERT_EQUAL_INT(0, list->size);

    se_array_list_destroy(list);
}

void seika_asset_file_loader_test(void) {
    sf_asset_file_loader_initialize();

    sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode_ARCHIVE);
    const bool hasLoaded = sf_asset_file_loader_load_archive("seika/test/resources/test.pck");
    TEST_ASSERT_TRUE(hasLoaded);

    // File exists in archive
    SEArchiveFileAsset existingFileAsset = sf_asset_file_loader_get_asset("test.txt");
    TEST_ASSERT_TRUE(sf_asset_file_loader_is_asset_valid(&existingFileAsset));
    // File doesn't exist
    SEArchiveFileAsset nonExistingFileAsset = sf_asset_file_loader_get_asset("test.png");
    TEST_ASSERT_TRUE(!sf_asset_file_loader_is_asset_valid(&nonExistingFileAsset));

    sf_asset_file_loader_finalize();
}

// Observer Test
static bool hasObserved = false;

void observer_func1(SESubjectNotifyPayload* payload) {
    hasObserved = true;
}

void observer_func2(SESubjectNotifyPayload* payload) {
    const int dataValue = *(int*) payload->data;
    if (dataValue == 3 && payload->type == 127) {
        hasObserved = true;
    }
}

void seika_observer_test(void) {
    SEEvent* event = se_event_new();
    // Test 1 - Simple test with passing a NULL payload
    SEObserver* observer = se_observer_new(observer_func1);
    se_event_register_observer(event, observer);
    TEST_ASSERT_EQUAL_INT(1, event->observerCount);
    se_event_notify_observers(event, NULL);
    TEST_ASSERT(hasObserved);
    se_event_unregister_observer(event, observer);
    TEST_ASSERT_EQUAL_INT(0, event->observerCount);
    hasObserved = false;

    // Test 2 - A slightly more complicated example filling out the payload
    se_observer_delete(observer);
    observer = se_observer_new(observer_func2);
    se_event_register_observer(event, observer);
    int dataValue = 3;
    se_event_notify_observers(event, &(SESubjectNotifyPayload) {
        .data = &dataValue, .type = 127
    });
    TEST_ASSERT(hasObserved);

    // Clean up
    se_event_delete(event);
    se_observer_delete(observer);
}

void seika_curve_float_test(void) {
    SECurveFloat curve = { .controlPointCount = 0 };
    SECurveControlPoint point1 = { .x = 0.0, .y = 0.0, .tangentIn = 0.0, .tangentOut = 0.0 };
    se_curve_float_add_control_point(&curve, point1);
    TEST_ASSERT_EQUAL_UINT(1, curve.controlPointCount);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, se_curve_float_eval(&curve, 1.0));
    SECurveControlPoint point2 = { .x = 1.0, .y = 1.0, .tangentIn = 0.0, .tangentOut = 0.0 };
    se_curve_float_add_control_point(&curve, point2);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, se_curve_float_eval(&curve, 0.0));
    TEST_ASSERT_EQUAL_DOUBLE(0.5, se_curve_float_eval(&curve, 0.5));
    TEST_ASSERT_EQUAL_DOUBLE(1.0, se_curve_float_eval(&curve, 1.0));
    se_curve_float_remove_control_point(&curve, point2.x, point2.y);
    TEST_ASSERT_EQUAL_UINT(1, curve.controlPointCount);

    // TODO: Write performance tests
//    SE_PROFILE_CODE(
//            for (int i = 0; i < 10000000; i++) {}
//    )
//
//    double cpu_time_used;
//    SE_PROFILE_CODE_WITH_VAR(cpu_time_used, for (int i = 0; i < 10000000; i++) {})
//    printf("Time taken: %f seconds\n", cpu_time_used);
}

void seika_shader_instance_test(void) {
    // Shader instance param tests
    SEShaderInstance shaderInstance = { .shader = NULL, .paramMap = se_string_hash_map_create_default_capacity() };

    se_shader_instance_param_create_bool(&shaderInstance, "is_active", false);
    TEST_ASSERT_FALSE(se_shader_instance_param_get_bool(&shaderInstance, "is_active"));
    se_shader_instance_param_update_bool(&shaderInstance, "is_active", true);
    TEST_ASSERT_TRUE(se_shader_instance_param_get_bool(&shaderInstance, "is_active"));

    // Clean up
    SE_STRING_HASH_MAP_FOR_EACH(shaderInstance.paramMap, iter) {
        StringHashMapNode* node = iter.pair;
        SEShaderParam* param = (SEShaderParam*) node->value;
        SE_MEM_FREE(param->name);
    }
    se_string_hash_map_destroy(shaderInstance.paramMap);
}

void seika_shader_file_parser_test(void) {
    char shader[] =
        "shader_type screen;\n"
        "\n"
        "uniform float time;\n"
        "uniform vec2 size;\n"
        "uniform float brightness = 1.0f;\n"
        "uniform int spriteCount = 1;\n"
        "\n"
        "void testFunc() {}\n"
        "\n"
        "void vertex() {\n"
        "    VERTEX.x += 0.1f;"
        "}\n"
        "\n"
        "void fragment() {\n"
        "    COLOR *= brightness;\n"
        "}\n"
        "\n";
    SEShaderFileParseResult result = se_shader_file_parser_parse_shader(shader);
    // Shouldn't be an error message
    const bool hasErrorMessage = strlen(result.errorMessage) > 0;
    if (hasErrorMessage) {
        printf("Shader parse error = '%s'\n", result.errorMessage);
    }
    TEST_ASSERT_FALSE(hasErrorMessage);
    se_shader_file_parse_clear_parse_result(&result);
}
