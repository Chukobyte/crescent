#pragma once

#include <vector>
#include <string>

#include "../utils/singleton.h"
#include "../engine/src/core/ecs/component/node_component.h"
#include "../engine/src/core/scripting/python/rbe_py_file_loader.h"

struct SceneNode {
    [[nodiscard]] NodeBaseInheritanceType GetInheritanceType() const {
        return node_get_type_inheritance(type);
    }

    std::string name;
    NodeBaseType type = NodeBaseType_INVALID;
    SceneNode* parent = nullptr;
    std::vector<SceneNode*> children;
};

struct SceneNodeFile {
    std::string filePath;
    SceneNode* rootNode = nullptr;
};

class SceneManager : public Singleton<SceneManager> {
  public:
    SceneManager(singleton) {}
    bool LoadSceneFromFile(const char* sceneFilePath);

    std::vector<SceneNodeFile*> loadedSceneFiles;

  private:
    static SceneNodeFile* GenerateSceneNodeFile(SceneTreeNode* rootTreeNode, const char* sceneFilePath);

    // Recursive function to load SceneTreeNode into SceneNode for usability purposes
    static SceneNode* LoadSceneTreeNode(SceneTreeNode* node, SceneNode* parent = nullptr);
};
