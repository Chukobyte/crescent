#pragma once

#include "../../../imgui/imgui_helper.h"

class AnimatedSpriteComp;

// Temp
struct FuncObject {
    FuncObject(std::function<void()> func) {
        func();
    }
};

namespace OpenedProjectUI {
namespace Windows {
ImGuiHelper::Window GetDetailsWindow();

class AnimationEditor : public Singleton<AnimationEditor> {
  public:
    explicit AnimationEditor(singleton) {}
    ImGuiHelper::PopupModal& GetPopup(AnimatedSpriteComp* animatedSpriteComp);
};
} // namespace Windows
} // namespace OpenedProjectUI
