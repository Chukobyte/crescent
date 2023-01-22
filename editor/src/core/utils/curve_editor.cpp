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
void DrawCurve(const CurveFloat& curve, const char* label) {
    const float firstPosition = curve.GetFirstPosition();
    const float lastPosition = curve.GetLastPosition();
    SE_ASSERT_FMT(firstPosition != lastPosition, "first position '%f' equals last position '%f'", firstPosition, lastPosition);
    // Derive sample increments by calculating from positions and number of samples
    const int numSamples = 100;
    const float sampleIncrement = (lastPosition - firstPosition) / numSamples;
    std::vector<float> samplesX(numSamples);
    std::vector<float> samplesY(numSamples);
    float currentPosition = firstPosition;
    for (int i = 0; i < numSamples; i++) {
        samplesX[i] = currentPosition;
        samplesY[i] = curve.Eval(currentPosition);
        currentPosition += sampleIncrement;
    }

    ImPlot::PlotLine(label, samplesX.data(), samplesY.data(), numSamples);
}

} // namespace

void SimpleCurveEditor::Begin() {
    ImGuiHelper::Window curveWindow = {
        .name = "Curve Editor Window",
        .open = nullptr,
        .windowFlags = ImGuiWindowFlags_None,
        .callbackFunc = [] (ImGuiHelper::Context* context) {
            static CurveFloat curveFloat = CurveFloat( {
                { .position = 0.0f, .value = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
                { .position = 5.0f, .value = 20.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
                { .position = 10.0f, .value = 30.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
                { .position = 12.0f, .value = 0.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            });

            if (ImPlot::BeginPlot("Curve Float")) {
                const ImPlotAxisFlags axeFlags = ImPlotAxisFlags_None;
                ImPlot::SetupAxes("time", "value", axeFlags, axeFlags);
                if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::GetIO().KeyCtrl) {
                    ImPlotPoint pt = ImPlot::GetPlotMousePos();
                    curveFloat.AddControlPoint((float) pt.x, (float) pt.y, 0.0f, 0.0f);
                }

                DrawCurve(curveFloat, "Curve Float");
                ImPlot::EndPlot();
            }
        },
        .position = ImVec2{ 200.0f, 100.0f },
        .size = ImVec2{ 300.0f, 300.0f },
    };
    ImGuiHelper::BeginWindowWithEnd(curveWindow);
}
