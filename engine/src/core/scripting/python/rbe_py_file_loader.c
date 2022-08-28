#include "rbe_py_file_loader.h"

#include "py_helper.h"
#include "../../utils/rbe_assert.h"

RBEGameProperties* rbe_py_load_game_properties(const char* filePath) {
    bool load_success = pyh_run_python_file(filePath);
    RBE_ASSERT_FMT(load_success == true, "Failed to load config at '%s'", filePath);
    return rbe_game_props_get();
}

SceneTreeNode* rbe_py_load_scene(const char* filePath) {
    bool load_success = pyh_run_python_file(filePath);
    RBE_ASSERT_FMT(load_success == true, "Failed to scene file at '%s'", filePath);
    SceneTreeNode* rootNode = rbe_scene_manager_get_active_scene_root();
    RBE_ASSERT_FMT(rootNode != NULL, "Root node is NULL for file path at '%s'", filePath);
    return rootNode;
}
