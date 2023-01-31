#include "curve_editor.h"

#include <implot.h>

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

//--- CurveEditor ---//
namespace {
void DrawCurve(const CurveFloat& curve, const char* label) {
    // Early out when no points exist
    if (!curve.HasControlPoints()) {
        static double x[1];
        static double y[1];
        ImPlot::PlotLine(label, x, y, 0);
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

    ImPlot::PlotLine(label, samplesX.data(), samplesY.data(), numSamples);
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

            if (ImPlot::BeginPlot("Curve Float")) {
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
                        ImPlotPoint pt = ImPlot::GetPlotMousePos();
                        curve.AddControlPoint(pt.x, pt.y, 0.0, 0.0);
                    } else if (isShiftHeld && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        ImPlotPoint pt = ImPlot::GetPlotMousePos();
                        curve.RemoveControlPoint(pt.x, pt.y);
                    }
                }

                DrawCurve(curve, "Curve Float");

                // Draw movable control points
                // TODO: Need to handle case when position order changes from dragging
                ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                int pointId = 0;
                for (auto& point : curve.GetControlPointsRef()) {
                    ImPlot::DragPoint(pointId++, &point.position, &point.value, ImVec4(0.0f, 0.9f, 0.0f, 1.0f), 4, dragPointFlags);
                }
                // Draw Tangents
                if (showTangents) {
                    for (auto& point : curve.GetControlPointsRef()) {
                        // Incoming Tangent
                        double inTangentPos = point.position - 0.1;
                        double inTangentValue = point.value;
                        ImPlot::DragPoint(pointId++, &inTangentPos, &inTangentValue, ImVec4(0.75f, 0.0f, 0.25f, 1.0f), 4, dragPointFlags);
                        point.tangentIn = point.value - inTangentValue;
                        // Outgoing Tangent
                        double outTangentPos = point.position + 0.1;
                        double outTangentValue = point.value;
                        ImPlot::DragPoint(pointId++, &outTangentPos, &outTangentValue, ImVec4(0.75f, 0.0f, 0.25f, 1.0f), 4, dragPointFlags);
                        point.tangentOut = point.value + outTangentValue;
                    }
                }
                ImPlot::EndPlot();
            }
        },
        .position = ImVec2{ 200.0f, 100.0f },
        .size = ImVec2{ 300.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(curveWindow);
}
