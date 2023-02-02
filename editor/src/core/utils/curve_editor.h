#pragma once

#include "../math/curve_float.h"
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

    std::string label = "Curve Editor";
    ImVec2 editorSize = { 200.0f, 200.0f };
    CurveFloat curve;
    bool showTangents = false;
    bool isGlobalCurveEditor = false;

  private:
    bool hasBeenSaved = false;
};
