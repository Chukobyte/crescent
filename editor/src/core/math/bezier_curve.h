#pragma once

#include <vector>

#include "../seika/src/utils/se_assert.h"

#include "../seika/src/math/se_math.h"

// Curve Float
class CurveFloat {
  public:
    struct ControlPoint {
        float position; // x
        float value; // y
        float tangentIn;
        float tangentOut;
    };

    CurveFloat() = default;

    explicit CurveFloat(std::vector<ControlPoint> points) : controlPoints(std::move(points)) {}

    void AddControlPoint(float position, float value, float tangentIn, float tangentOut) {
        ControlPoint point = { position, value, tangentIn, tangentOut };
        controlPoints.push_back(point);
    }

    [[nodiscard]] float Eval(float position) const {
        const int numPoints = (int) controlPoints.size();
        if (numPoints == 0) {
            return 0.0f;
        } else if (numPoints == 1) {
            return controlPoints[0].value;
        }

        int leftIndex = 0;
        int rightIndex = numPoints - 1;
        while (rightIndex - leftIndex > 1) {
            int midIndex = (leftIndex + rightIndex) / 2;
            if (position < controlPoints[midIndex].position) {
                rightIndex = midIndex;
            } else {
                leftIndex = midIndex;
            }
        }

        const ControlPoint& leftPoint = controlPoints[leftIndex];
        const ControlPoint& rightPoint = controlPoints[rightIndex];

        const float t = (position - leftPoint.position) / (rightPoint.position - leftPoint.position);
        const float t2 = t * t;
        const float t3 = t2 * t;

        const float a = 2.0f * t3 - 3.0f * t2 + 1.0f;
        const float b = -2.0f * t3 + 3.0f * t2;
        const float c = t3 - 2.0f * t2 + t;
        const float d = t3 - t2;

        return a * leftPoint.value + b * rightPoint.value + c * leftPoint.tangentOut + d * rightPoint.tangentIn;
    }

  private:
    std::vector<ControlPoint> controlPoints;
};

// Bezier

using BezierPoint = Vector2;

namespace BezierMath {
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
    [[nodiscard]] const std::vector<BezierPoint>& GetPoints() const {
        return points;
    }

    void AddPoint(const BezierPoint& curvePoint) {
        points.emplace_back(curvePoint);
    }

    bool RemovePoint(const BezierPoint& curvePoint) {
        for (int i = 0; i < points.size(); i++) {
            const auto& point = points[i];
            if (point.x == curvePoint.x && point.y == curvePoint.y) {
                points.erase(points.begin() + i);
                return true;
            }
        }
        return false;
    }

    void ClearPoints() {
        points.clear();
    }

    BezierPoint Eval(float t) {
#define POINT_STRIDE 3
        SE_ASSERT(points.size() >= 4);
        int numCurves = points.size() / POINT_STRIDE;
        for (int i = 0; i < numCurves; i++) {
            const float pointIndex = (float) i;
            // t is within the range of this curve, so we evaluate it
            if (t >= pointIndex && t < pointIndex + 1) {
                const BezierPoint& p0 = points[i * POINT_STRIDE];
                const BezierPoint& p1 = points[i * POINT_STRIDE + 1];
                const BezierPoint& p2 = points[i * POINT_STRIDE + 2];
                const BezierPoint& p3 = points[i * POINT_STRIDE + 3];
                return BezierMath::Cubic(p0, p1, p2, p3, t - pointIndex);
            }
        }
        return { 0.0f, 0.0f };
#undef POINT_STRIDE
    }

  private:
    std::vector<BezierPoint> points = {};
};
