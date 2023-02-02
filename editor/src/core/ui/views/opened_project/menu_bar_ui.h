#pragma once

#include <optional>

#include "../../imgui/imgui_helper.h"
#include "../../../utils/curve_editor.h"

namespace OpenedProjectUI {
namespace MenuBar {
ImGuiHelper::MenuBar GetMenuBar();
namespace Tools {
std::optional<CurveEditor>& GetCurveEditor();
} // namespace Tools
} // namespace MenuBar
} // namespace OpenedProjectUI
