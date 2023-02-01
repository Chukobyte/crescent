#include "curve_editor.h"

#include <implot.h>
#include <implot_internal.h>

// TODO: Refactor once minimal functionality is met...

static const float NODE_SLOT_RADIUS = 4.0f;

ImVec2 operator+(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x + otherVector.x, thisVector.y + otherVector.y };
}

ImVec2& operator+=(ImVec2 &thisVector, const ImVec2 &otherVector) {
    thisVector.x += otherVector.x;
    thisVector.y += otherVector.y;
    return thisVector;
}

ImVec2 operator-(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x - otherVector.x, thisVector.y - otherVector.y };
}

ImVec2& operator-=(ImVec2 &thisVector, const ImVec2 &otherVector) {
    thisVector.x -= otherVector.x;
    thisVector.y -= otherVector.y;
    return thisVector;
}

ImVec2 operator*(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x * otherVector.x, thisVector.y * otherVector.y };
}

ImVec2& operator*=(ImVec2 &thisVector, const ImVec2 &otherVector) {
    thisVector.x *= otherVector.x;
    thisVector.y *= otherVector.y;
    return thisVector;
}

ImVec2 operator*(const ImVec2 &thisVector, float value) {
    return { thisVector.x * value, thisVector.y * value };
}

ImVec2 operator/(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return { thisVector.x / otherVector.x, thisVector.y / otherVector.y };
}

ImVec2 operator/(const ImVec2 &thisVector, float value) {
    return { thisVector.x / value, thisVector.y / value };
}

bool operator==(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return thisVector.x == otherVector.x && thisVector.y == otherVector.y;
}

bool operator!=(const ImVec2 &thisVector, const ImVec2 &otherVector) {
    return !(thisVector == otherVector);
}

namespace {
bool DragPointEx(int n_id, double* x, double* y, const ImVec4& col, float radius, ImPlotDragToolFlags flags, bool blockXInput = false, bool blockYInput = false) {
    ImGui::PushID("#IMPLOT_DRAG_POINT");
    IM_ASSERT_USER_ERROR(GImPlot->CurrentPlot != nullptr, "DragPoint() needs to be called between BeginPlot() and EndPlot()!");

    ImPlot::SetupLock();

    if (!ImHasFlag(flags,ImPlotDragToolFlags_NoFit) && ImPlot::FitThisFrame()) {
        ImPlot::FitPoint(ImPlotPoint(*x,*y));
    }

    const bool input = !ImHasFlag(flags, ImPlotDragToolFlags_NoInputs);
    const bool show_curs = !ImHasFlag(flags, ImPlotDragToolFlags_NoCursors);
    const bool no_delay = !ImHasFlag(flags, ImPlotDragToolFlags_Delayed);
    static const float DRAG_GRAB_HALF_SIZE = 4.0f;
    const float grab_half_size = ImMax(DRAG_GRAB_HALF_SIZE, radius);
    const ImVec4 color = ImPlot::IsColorAuto(col) ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : col;
    const ImU32 col32 = ImGui::ColorConvertFloat4ToU32(color);

    ImVec2 pos = ImPlot::PlotToPixels(*x, *y, IMPLOT_AUTO,IMPLOT_AUTO);
    const ImGuiID id = ImGui::GetCurrentWindow()->GetID(n_id);
    ImRect rect(pos.x-grab_half_size,pos.y-grab_half_size,pos.x+grab_half_size,pos.y+grab_half_size);
    bool hovered = false, held = false;

    ImGui::KeepAliveID(id);
    if (input)
        ImGui::ButtonBehavior(rect,id,&hovered,&held);

    bool dragging = false;
    if (held && ImGui::IsMouseDragging(0)) {
        if (!blockXInput) {
            *x = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).x;
        }
        if (!blockYInput) {
            *y = ImPlot::GetPlotMousePos(IMPLOT_AUTO,IMPLOT_AUTO).y;
        }
        dragging = true;
    }

    ImPlot::PushPlotClipRect();
    ImDrawList& DrawList = *ImPlot::GetPlotDrawList();
    if ((hovered || held) && show_curs)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    if (dragging && no_delay)
        pos = ImPlot::PlotToPixels(*x, *y, IMPLOT_AUTO,IMPLOT_AUTO);
    DrawList.AddCircleFilled(pos, radius, col32);
    ImPlot::PopPlotClipRect();

    ImGui::PopID();
    return dragging;
}
} // namespace

//--- CurveEditor ---//
#define CURVE_FLOAT_LABEL "##CurveFloat"

namespace {
void DrawCurve(const CurveFloat& curve) {
    // Early out when no points exist
    if (!curve.HasControlPoints()) {
        return;
    }
    const double firstPosition = curve.GetFirstPosition();
    const double lastPosition = curve.GetLastPosition();
    // Derive sample increments by calculating from positions and number of samples
    const int numSamples = 100;
    const double sampleIncrement = (lastPosition - firstPosition) / numSamples;
    std::vector<double> samplesX(numSamples);
    std::vector<double> samplesY(numSamples);
    double currentPosition = firstPosition;
    for (int i = 0; i < numSamples; i++) {
        samplesX[i] = currentPosition;
        samplesY[i] = curve.Eval(currentPosition);
        currentPosition += sampleIncrement;
    }

    ImPlot::PlotLine(CURVE_FLOAT_LABEL, samplesX.data(), samplesY.data(), numSamples);
}
} // namespace

void CurveEditor::Begin() {
    ImGuiHelper::Window curveWindow = {
        .name = "Curve Editor Window",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_None,
        .callbackFunc = [this] (ImGuiHelper::Context* context) {
            ImGuiHelper::CheckBox showTangentsCheckBox("Show Tangents", showTangents);
            ImGuiHelper::BeginCheckBox(showTangentsCheckBox);

            if (ImPlot::BeginPlot(CURVE_FLOAT_LABEL)) {
                const ImPlotAxisFlags axeFlags = ImPlotAxisFlags_None;
                ImPlot::SetupAxes("time", "value", axeFlags, axeFlags);
                ImPlotDragToolFlags dragPointFlags = ImPlotDragToolFlags_None;
                // Control Point add/remove inputs
                if (ImPlot::IsPlotHovered()) {
                    const bool isShiftHeld = ImGui::GetIO().KeyShift;
                    if (isShiftHeld) {
                        dragPointFlags = ImPlotDragToolFlags_NoInputs;
                    }
                    if (ImGui::GetIO().KeyCtrl && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        const ImPlotPoint pt = ImPlot::GetPlotMousePos();
                        curve.AddControlPoint(pt.x, pt.y, 0.0, 0.0);
                    } else if (isShiftHeld && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        const ImPlotPoint pt = ImPlot::GetPlotMousePos();
                        curve.RemoveControlPoint(pt.x, pt.y);
                    }
                }

                // Draw tangents first
                int pointId = 0;
                // TODO: Tangents should be the same position regardless of zoom level
                if (showTangents) {
                    for (auto& point : curve.GetControlPointsRef()) {
                        // Incoming Tangent
                        double inTangentPos = point.position - 0.1;
                        double inTangentValue = point.value - point.tangentIn;
                        const double prevInTangentValue = inTangentValue;
                        if (DragPointEx(pointId++, &inTangentPos, &inTangentValue, ImVec4(0.75f, 0.0f, 0.25f, 1.0f), 4, ImPlotDragToolFlags_None, true)) {
                            const double inTangentDelta = prevInTangentValue - inTangentValue;
                            point.tangentIn = inTangentDelta;
                        }
                        // Outgoing Tangent
                        double outTangentPos = point.position + 0.1;
                        double outTangentValue = point.value + point.tangentOut;
                        const double prevOutTangentValue = outTangentValue;
                        if (DragPointEx(pointId++, &outTangentPos, &outTangentValue, ImVec4(0.75f, 0.0f, 0.25f, 1.0f), 4, ImPlotDragToolFlags_None, true)) {
                            const double outTangentDelta = outTangentValue - prevOutTangentValue;
                            point.tangentOut = outTangentDelta;
                        }
                        // Draw tangent lines
                        double x[3];
                        double y[3];
                        x[0] = inTangentPos;
                        y[0] = inTangentValue;
                        x[1] = point.position;
                        y[1] = point.value;
                        x[2] = outTangentPos;
                        y[2] = outTangentValue;
                        ImPlot::PlotLine("##Tangent", x, y, 3, ImPlotLineFlags_None);
                    }
                }

                DrawCurve(curve);

                // Draw movable control points
                // TODO: Need to handle case when position order changes from dragging
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                for (auto& point : curve.GetControlPointsRef()) {
                    ImPlot::DragPoint(pointId++, &point.position, &point.value, ImVec4(0.0f, 0.9f, 0.0f, 1.0f), 4, dragPointFlags);
                }
                ImPlot::EndPlot();
            }
        },
        .position = ImVec2{ 200.0f, 100.0f },
        .size = ImVec2{ 300.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(curveWindow);
}
