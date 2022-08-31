#pragma once

#include <vector>
#include <map>
#include <typeinfo>
#include <string>

#include "../components/component.h"
#include "../utils/singleton.h"
#include "../engine/src/core/ecs/component/node_component.h"
#include "../engine/src/core/scripting/python/rbe_py_file_loader.h"

struct SceneNode {
    SceneNode() {
        uid = GenerateUID();
    }

    [[nodiscard]] NodeBaseInheritanceType GetInheritanceType() const {
        return node_get_type_inheritance(type);
    }

    [[nodiscard]] const char* GetTypeString() const {
        return node_get_component_type_string(type);
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

    std::string name;
    NodeBaseType type = NodeBaseType_INVALID;
    SceneNode* parent = nullptr;
    std::vector<SceneNode*> children;
    std::map<const std::type_info*, EditorComponent*> components;

  private:
    unsigned int uid;

    unsigned int GenerateUID() {
        static unsigned int uidCounter = 0;
        return uidCounter++;
    }
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

    SceneNodeFile* selectedSceneFile = nullptr;
    SceneNode* selectedSceneNode = nullptr;

  private:
    static SceneNodeFile* GenerateSceneNodeFile(FileSceneNode* rootTreeNode, const char* sceneFilePath);

    // Recursive function to load SceneTreeNode into SceneNode for usability purposes
    static SceneNode* LoadSceneTreeNode(FileSceneNode* node, SceneNode* parent = nullptr);
};
