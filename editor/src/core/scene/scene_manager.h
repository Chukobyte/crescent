#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <typeinfo>
#include <string>

#include "../components/component.h"
#include "../utils/singleton.h"
#include "../engine/src/core/ecs/component/node_component.h"
#include "../engine/src/core/scripting/python/rbe_py_file_loader.h"

class SceneNode {
  public:
    SceneNode() {
        uid = GenerateUID();
    }

    [[nodiscard]] NodeBaseInheritanceType GetInheritanceType() const {
        return node_get_type_inheritance(type);
    }

    [[nodiscard]] const char* GetTypeString() const {
        return node_get_base_type_string(type);
    }

    [[nodiscard]] unsigned int GetUID() const {
        return uid;
    }

    template <typename T, typename... TArgs>
    T& AddComponent(TArgs&&... args) {
        T* newComponent(new T(std::forward<TArgs>(args)...));
        components[&typeid(*newComponent)] = newComponent;
        return *newComponent;
    }

    template <typename T>
    void RemoveComponent() {
        if (components.count(&typeid(T)) > 0) {
            delete components[&typeid(T)];
            components.erase(&typeid(T));
        }
    }

    template <typename T>
    [[nodiscard]] T* GetComponent() {
        return static_cast<T*>(components[&typeid(T)]);
    }

    template <typename T>
    [[nodiscard]] T* GetComponentSafe() {
        if (HasComponent<T>()) {
            return static_cast<T*>(components[&typeid(T)]);
        }
        return nullptr;
    }

    template <typename T>
    bool HasComponent() const {
        return components.count(&typeid(T));
    }

    size_t GetComponentCount() const {
        return components.size();
    }

    bool HasComponents() const {
        return GetComponentCount() > 0;
    }

    void RemoveChild(SceneNode* childNode) {
        children.erase(std::remove_if(children.begin(), children.end(), [childNode](SceneNode* node) {
            return node == childNode;
        }), children.end());
    }

    std::string name;
    NodeBaseType type = NodeBaseType_INVALID;
    SceneNode* parent = nullptr;
    std::vector<SceneNode*> children;

  private:
    unsigned int uid;
    std::map<const std::type_info*, EditorComponent*> components;

    unsigned int GenerateUID() {
        static unsigned int uidCounter = 0;
        return uidCounter++;
    }
};

struct SceneNodeFile {
    std::string filePath;
    SceneNode* rootNode = nullptr;
    bool hasBeenSaved = false; // New Scene
};

namespace SceneNodeUtils {
void DisplayTreeNodeLeaf(SceneNode* sceneNode);
}

class SceneManager : public Singleton<SceneManager> {
  public:
    SceneManager(singleton) {}
    SceneNodeFile* LoadSceneFromFile(const char* sceneFilePath, bool forceLoadFromDisk = false);
    void AddDefaultNodeAsChildToSelected(NodeBaseType type);
    void QueueNodeForDeletion(SceneNode* nodeToDelete, bool recurseChildren = true);
    void FlushQueuedForDeletionNodes();

    SceneNodeFile* GenerateDefaultSceneNodeFile() const;
    SceneNodeFile* GenerateDefaultSceneNodeFile(SceneNode* rootSceneNode) const;
    void ResetCurrentSceneNodeFile();

    static std::string GetUniqueNodeName(const std::string& nameCandidate, SceneNode* parent = nullptr);

    std::unordered_map<std::string, SceneNodeFile*> loadedSceneFiles;
    SceneNodeFile* selectedSceneFile = nullptr;

    SceneNode* selectedSceneNode = nullptr;

  private:
    static SceneNodeFile* GenerateSceneNodeFileFromJson(struct JsonSceneNode* rootTreeNode, const char* sceneFilePath);

    // Recursive function to load SceneTreeNode into SceneNode for usability purposes
    static SceneNode* LoadSceneTreeJson(struct JsonSceneNode* node, SceneNode* parent = nullptr);

    std::vector<SceneNode*> nodesQueuedForDeletion;
};
