#pragma once

#include "../ui/imgui/imgui_helper.h"
#include "imgui_internal.h"
#include "enum_class_utils.h"

enum class CurveEditorFlags {
    NO_TANGENTS = 1 << 0,
    SHOW_GRID = 1 << 1,
    ALL = NO_TANGENTS | SHOW_GRID,
};
ENUM_CLASS_FLAG_OPERATORS(CurveEditorFlags)

class CurveEditor {
  public:
    int Begin(int* newCount = nullptr);

    std::string label;
    CurveEditorFlags flags = CurveEditorFlags::ALL;
    float* values = nullptr;
    int valueCount = 0;
    ImVec2 editorSize;

  private:
    int BeginInternal(int* newCount);

    ImVec2 startPan;
    // Testing out selected curve
    int selectedId = -1;
    float selectedCurveTime = 0.0f;
    float selectedCurveValue = 0.0f;
};
