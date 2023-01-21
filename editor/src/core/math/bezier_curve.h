#pragma once

#include <vector>

#include "../seika/src/math/se_math.h"

using BezierPoint = Vector2;

struct CubicBezierCurve {
    BezierPoint p0 = { 0.0f, 0.0f }; // Anchor Point 1
    BezierPoint p1 = { 0.0f, 0.0f }; // Control Point 1
    BezierPoint p2 = { 0.0f, 0.0f }; // Control Point 2
    BezierPoint p3 = { 0.0f, 0.0f }; // Anchor Point 2

    [[nodiscard]] BezierPoint Eval(float t) const;
};

class BezierSpline {
  public:
    [[nodiscard]] const std::vector<CubicBezierCurve>& GetPoints() const {
        return points;
    }

    void AddPoint(const CubicBezierCurve& curvePoint) {
        points.emplace_back(curvePoint);
    }

    void ClearPoints() {
        points.clear();
    }

  private:
    std::vector<CubicBezierCurve> points = {};
};
