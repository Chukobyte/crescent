#include "unity.h"

#include "../src/memory/se_mem.h"
#include "../src/utils/se_string_util.h"

void setUp() {}
void tearDown() {}

void seika_string_utils_test();

int main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(seika_string_utils_test);
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
