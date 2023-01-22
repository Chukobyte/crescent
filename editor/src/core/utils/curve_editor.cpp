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

//--- SimpleCurveEditor ---//
namespace {
void DrawCurve(const CurveFloat& curve, const char* label, float min, float max) {
//    float xData[4];
//    float yData[4];
//    for (size_t i = 0; i < curve.GetPointCount(); i++) {
//        const auto& point = points[i];
//        xData[i] = point.position;
//        yData[i] = point.value;
//    }

//    const int numSamples = 100;
//    std::vector<float> samplesX(numSamples);
//    std::vector<float> samplesY(numSamples);
//    for (int i = 0; i < numSamples; i++) {
//        float position = (float)i / (float)(numSamples - 1);
//        samplesX[i] = position * (max - min) + min;
//        samplesY[i] = curve.Eval(position);
//    }

    std::vector<float> samplesX;
    std::vector<float> samplesY;
    const auto points = curve.GetControlPoints();
    int i = 0;
    for (auto& point : curve.GetControlPoints()) {
//        samplesX.emplace_back(point.position);
//        samplesY.emplace_back(point.value);
        ImPlot::DragPoint(i, (double*) &point.position, (double*) &point.value, ImVec4(0,0.9f,0,1));
    }
//    ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
//    ImPlot::PlotLine(label, samplesX.data(), samplesY.data(), 200);
}

} // namespace

void SimpleCurveEditor::Begin() {
    ImGuiHelper::Window curveWindow = {
        .name = "Curve Editor Window",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_None,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            // Control Point Drag Floats
//            ImGuiHelper::DragFloat2 p0DragFloat2("p0", (float*) &p0);
//            ImGuiHelper::DragFloat2 p1DragFloat2("p1", (float*) &p1);
//            ImGuiHelper::DragFloat2 p2DragFloat2("p2", (float*) &p2);
//            ImGuiHelper::DragFloat2 p3DragFloat2("p3", (float*) &p3);
//            ImGuiHelper::BeginDragFloat2(p0DragFloat2);
//            ImGuiHelper::BeginDragFloat2(p1DragFloat2);
//            ImGuiHelper::BeginDragFloat2(p2DragFloat2);
//            ImGuiHelper::BeginDragFloat2(p3DragFloat2);
//
//            const ImVec2 windowPos = ImGui::GetWindowPos();
//            ImDrawList* drawList = ImGui::GetWindowDrawList();
//            const ImVec2 windowP0 = {p0.x + windowPos.x, p0.y + windowPos.y };
//            const ImVec2 windowP1 = {p1.x + windowPos.x, p1.y + windowPos.y };
//            const ImVec2 windowP2 = {p2.x + windowPos.x, p2.y + windowPos.y };
//            const ImVec2 windowP3 = {p3.x + windowPos.x, p3.y + windowPos.y };
//            drawList->AddBezierCubic(windowP0, windowP1, windowP2, windowP3, ImColor(255, 0, 0), 10, 0);
//            ImGui::BeginChild("Curve", ImGui::GetContentRegionAvail(), false, ImGuiWindowFlags_HorizontalScrollbar);
            static CurveFloat curveFloat = CurveFloat( {
                { .position = 0.0f, .value = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
                { .position = 5.0f, .value = 20.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
                { .position = 10.0f, .value = 30.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
                { .position = 12.0f, .value = 0.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            });

            if (ImPlot::BeginPlot("Curve Float")) {
                const ImPlotAxisFlags axeFlags = ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoTickMarks;
                ImPlot::SetupAxes("time", "value", axeFlags, axeFlags);
//                ImPlot::SetupAxesLimits(0, 1, 0, 1);
                if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl) {
                    ImPlotPoint pt = ImPlot::GetPlotMousePos();
                    curveFloat.AddControlPoint((float) pt.x, (float) pt.y, 0.0f, 0.0f);
                }

                DrawCurve(curveFloat, "Curve Float", 0.0f, 30.0f);
                ImPlot::EndPlot();
            }
//            ImGui::EndChild();
        },
        .position = ImVec2{ 200.0f, 100.0f },
        .size = ImVec2{ 300.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(curveWindow);
}
