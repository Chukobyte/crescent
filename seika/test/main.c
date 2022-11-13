#include "unity.h"

#include "../src/asset/asset_file_loader.h"
#include "../src/memory/se_mem.h"
#include "../src/utils/se_string_util.h"
#include "../src/utils/se_file_system_utils.h"

void setUp() {}
void tearDown() {}

void seika_string_utils_test();
void seika_asset_file_loader_test();

int main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(seika_string_utils_test);
    RUN_TEST(seika_asset_file_loader_test);
    return UNITY_END();
}

void seika_string_utils_test() {
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

void seika_asset_file_loader_test() {
    sf_asset_file_loader_initialize();

    sf_asset_file_loader_set_read_mode(SEAssetFileLoaderReadMode_ARCHIVE);
    printf("cwd = %s\n", se_fs_get_cwd());
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
