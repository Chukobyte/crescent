#pragma once

#include "../math/bezier_curve.h"
#include "../ui/imgui/imgui_helper.h"
#include "enum_class_utils.h"
#include "imgui_internal.h"

enum class CurveEditorFlags {
    NO_TANGENTS = 1 << 0,
    SHOW_GRID = 1 << 1,
    ALL = NO_TANGENTS | SHOW_GRID,
};
ENUM_CLASS_FLAG_OPERATORS(CurveEditorFlags)

class CurveEditor {
  public:
    void Begin();

    std::string label;
    CurveEditorFlags flags = CurveEditorFlags::ALL;
    ImVec2 editorSize;
    CubicBezierCurve curve;

  private:
};
