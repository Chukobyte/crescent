#pragma once

#include "../../../imgui/imgui_helper.h"

struct AnimatedSpriteComp;

namespace OpenedProjectUI {
namespace Windows {
class AnimationEditor : public Singleton<AnimationEditor> {
  public:
    explicit AnimationEditor(singleton) {}
    ImGuiHelper::PopupModal& GetPopup(AnimatedSpriteComp* animatedSpriteComp);
};
} // namespace Windows
} // namespace OpenedProjectUI
