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

    // Removes a control points.  Returns 'true' if a control point has been removed.
    bool RemoveControlPoint(double position, double value) {
        // TODO: Need to get the close point (x) and value (y)
        for (size_t i = 0; i < controlPoints.size(); i++) {
            const auto& point = controlPoints[i];
            if (se_math_is_almost_equal_double(point.position, position, 0.1) && se_math_is_almost_equal_double(point.value, value, 0.1)) {
                controlPoints.erase(controlPoints.begin() + i);
                SortControlPointsByPosition();
                return true;
            }
        }
        return false;
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

    [[nodiscard]] std::vector<ControlPoint> GetControlPoints() const {
        return controlPoints;
    }

    [[nodiscard]] std::vector<ControlPoint>& GetControlPointsRef() {
        return controlPoints;
    }

    [[nodiscard]] size_t GetPointCount() const {
        return controlPoints.size();
    }

    [[nodiscard]] bool HasControlPoints() const {
        return controlPoints.size() > 0;
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

    void SortControlPointsByPosition() {
        std::sort(controlPoints.begin(), controlPoints.end(), [](const ControlPoint& p1, const ControlPoint& p2) {
            return p1.position < p2.position;
        });
    }

  private:
    std::vector<ControlPoint> controlPoints;
};
