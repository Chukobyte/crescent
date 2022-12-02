#include "unity.h"

#include <SDL2/SDL_main.h>

#include "../src/data_structures/se_array_utils.h"
#include "../src/data_structures/se_spatial_hash_map.h"
#include "../src/asset/asset_file_loader.h"
#include "../src/memory/se_mem.h"
#include "../src/utils/se_string_util.h"
#include "../src/utils/se_file_system_utils.h"

#define RESOURCES_PATH "seika/test/resources"
#define RESOURCES_PACK_PATH "seika/test/resources/test.pck"

void setUp() {}
void tearDown() {}

void seika_hash_map_test(void);
void seika_spatial_hash_map_test(void);
void seika_file_system_utils_test(void);
void seika_string_utils_test(void);
void seika_array_utils_test(void);
void seika_asset_file_loader_test(void);

int main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(seika_hash_map_test);
    RUN_TEST(seika_spatial_hash_map_test);
    RUN_TEST(seika_file_system_utils_test);
    RUN_TEST(seika_string_utils_test);
    RUN_TEST(seika_array_utils_test);
    RUN_TEST(seika_asset_file_loader_test);
    return UNITY_END();
}

void seika_hash_map_test(void) {
    SEHashMap* hashMap = se_hash_map_create(sizeof(int*), sizeof(int*), SE_HASH_MAP_MIN_CAPACITY);
    TEST_ASSERT_TRUE(hashMap != NULL);

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
    TEST_ASSERT_TRUE(spatialHashMap != NULL);

    // Create two entities and insert them into hash map
    const unsigned int entity = 1;
    SESpatialHashMapGridSpacesHandle* handle = se_spatial_hash_map_insert_or_update(spatialHashMap, entity, &(Rect2) {
        0.0f, 0.0f, 32.0f, 32.0f
    });
    TEST_ASSERT_EQUAL(handle, se_spatial_hash_map_get(spatialHashMap, entity));

    const unsigned int entityTwo = 2;
    SESpatialHashMapGridSpacesHandle* handleTwo = se_spatial_hash_map_insert_or_update(spatialHashMap, entityTwo, &(Rect2) {
        16.0f, 16.0f, 48.0f, 48.0f
    });
    TEST_ASSERT_EQUAL(handleTwo, se_spatial_hash_map_get(spatialHashMap, entityTwo));

    // An entity that should not be collided with
    const unsigned int entityNotCollided = 3;
    se_spatial_hash_map_insert_or_update(spatialHashMap, entityNotCollided, &(Rect2) {
        64.0f, 64.0f, 96.0f, 96.0f
    });

    // Test collision result to make sure the two entities collide
    const SESpatialHashMapCollisionResult collisionResult = se_spatial_hash_map_compute_collision(spatialHashMap, entity);
    TEST_ASSERT_EQUAL_INT(1, collisionResult.collisionCount);

    if (collisionResult.collisionCount > 0) {
        TEST_ASSERT_EQUAL_INT(2, collisionResult.collisions[0]);
    }

    se_spatial_hash_map_remove(spatialHashMap, entity);
    TEST_ASSERT_EQUAL(NULL, se_spatial_hash_map_get(spatialHashMap, entity));
    se_spatial_hash_map_remove(spatialHashMap, entityTwo);
    TEST_ASSERT_EQUAL(NULL, se_spatial_hash_map_get(spatialHashMap, entityTwo));

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
        array[i] = i;
    }

    TEST_ASSERT_EQUAL_INT(4, array[4]);
    se_array_utils_remove_item_uint32(array, &arraySize, 4, invalidValue);
    TEST_ASSERT_NOT_EQUAL_INT(4, array[4]);

#undef ARRAY_SIZE
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
