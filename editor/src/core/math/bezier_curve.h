#pragma once

#include <vector>

#include "../seika/src/math/se_math.h"

using BezierPoint = Vector2;

namespace BezierMath {
BezierPoint Lerp(const BezierPoint& p1, const BezierPoint& p2, float t);
BezierPoint Linear(const BezierPoint& p1, const BezierPoint& p2, float t);
BezierPoint Quadratic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, float t);
BezierPoint Cubic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, const BezierPoint& p3, float t);
} // BezierMath

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
