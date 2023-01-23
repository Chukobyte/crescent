#pragma once

#include <vector>
#include <algorithm>

#include "../seika/src/utils/se_assert.h"
#include "../seika/src/math/se_math.h"

// Curve Float
class CurveFloat {
  public:
    struct ControlPoint {
        double position; // x
        double value; // y
        double tangentIn;
        double tangentOut;
    };

    CurveFloat() = default;

    explicit CurveFloat(std::vector<ControlPoint> points) : controlPoints(std::move(points)) {
        SortControlPointsByPosition();
    }

    void AddControlPoint(double position, double value, double tangentIn, double tangentOut) {
        ControlPoint point = { position, value, tangentIn, tangentOut };
        controlPoints.push_back(point);
        SortControlPointsByPosition();
    }

    [[nodiscard]] double Eval(double position) const {
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

        const double t = (position - leftPoint.position) / (rightPoint.position - leftPoint.position);
        const double t2 = t * t;
        const double t3 = t2 * t;

        const double a = 2.0f * t3 - 3.0f * t2 + 1.0f;
        const double b = -2.0f * t3 + 3.0f * t2;
        const double c = t3 - 2.0f * t2 + t;
        const double d = t3 - t2;

        return a * leftPoint.value + b * rightPoint.value + c * leftPoint.tangentOut + d * rightPoint.tangentIn;
    }

    std::vector<ControlPoint> GetControlPoints() const {
        return controlPoints;
    }

    std::vector<ControlPoint>& GetControlPointsRef() {
        return controlPoints;
    }

    [[nodiscard]] size_t GetPointCount() const {
        return controlPoints.size();
    }

    [[nodiscard]] double GetFirstPosition() const {
        SE_ASSERT_FMT(controlPoints.size() > 0, "Control points are empty!");
        double smallestPosition = 9999999.0f;
        for (const auto& point : controlPoints) {
            if (point.position < smallestPosition) {
                smallestPosition = point.position;
            }
        }
        return smallestPosition;
    }

    [[nodiscard]] double GetLastPosition() const {
        SE_ASSERT_FMT(controlPoints.size() > 0, "Control points are empty!");
        double largestPosition = 0.0f;
        for (const auto& point : controlPoints) {
            if (point.position > largestPosition) {
                largestPosition = point.position;
            }
        }
        return largestPosition;
    }

  private:
    void SortControlPointsByPosition() {
        std::sort(controlPoints.begin(), controlPoints.end(), [](const ControlPoint& p1, const ControlPoint& p2) {
            return p1.position < p2.position;
        });
    }

    std::vector<ControlPoint> controlPoints;
};

// Bezier

using BezierPoint = Vector2;

namespace BezierMath {
BezierPoint Linear(const BezierPoint& p1, const BezierPoint& p2, double t);
BezierPoint Quadratic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, double t);
BezierPoint Cubic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, const BezierPoint& p3, double t);
} // BezierMath

struct CubicBezierCurve {
    BezierPoint p0 = { 0.0f, 0.0f }; // Anchor Point 1
    BezierPoint p1 = { 0.0f, 0.0f }; // Control Point 1
    BezierPoint p2 = { 0.0f, 0.0f }; // Control Point 2
    BezierPoint p3 = { 0.0f, 0.0f }; // Anchor Point 2

    [[nodiscard]] BezierPoint Eval(double t) const;
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
        for (size_t i = 0; i < points.size(); i++) {
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

    BezierPoint Eval(double t) {
#define POINT_STRIDE 3
        SE_ASSERT(points.size() >= 4);
        const int numCurves = points.size() / POINT_STRIDE;
        for (int i = 0; i < numCurves; i++) {
            const double pointIndex = (double) i;
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
