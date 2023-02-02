#pragma once

#include <vector>

#include "../seika/src/math/se_curve_float.h"
#include "../seika/src/utils/se_assert.h"

class CurveFloat {
  public:
    CurveFloat() = default;

    explicit CurveFloat(std::vector<SECurveControlPoint> points) {
        se_curve_float_add_control_points(&curve, points.data(), points.size());
        UpdateInternalPointsCache();
    }

    void AddControlPoint(double position, double value, double tangentIn, double tangentOut) {
        se_curve_float_add_control_point(
            &curve,
        { .x = position, .y = value, .tangentIn = tangentIn, .tangentOut = tangentOut }
        );
        UpdateInternalPointsCache();
    }

    [[nodiscard]] bool RemoveControlPoint(double position, double value) {
        const bool hasRemovedPoint = se_curve_float_remove_control_point(&curve, position, value);
        if (hasRemovedPoint) {
            UpdateInternalPointsCache();
        }
        return hasRemovedPoint;
    }

    [[nodiscard]] double Eval(double t) const {
        return se_curve_float_eval(&curve, t);
    }


    [[nodiscard]] std::vector<SECurveControlPoint*>& GetControlPoints() {
        return internalPointsCache;
    }

    [[nodiscard]] bool HasControlPoints() const {
        return curve.controlPointCount > 0;
    }

    [[nodiscard]] double GetFirstPosition() const {
        SE_ASSERT_FMT(curve.controlPointCount > 0, "Control points are empty!");
        return curve.controlPoints[0].x;
    }

    [[nodiscard]] double GetLastPosition() const {
        SE_ASSERT_FMT(curve.controlPointCount > 0, "Control points are empty!");
        return curve.controlPoints[curve.controlPointCount - 1].x;
    }

  private:
    void UpdateInternalPointsCache() {
        internalPointsCache.clear();
        for (size_t i; i < curve.controlPointCount; i++) {
            internalPointsCache.emplace_back(&curve.controlPoints[i]);
        }
    }

    SECurveFloat curve = { .controlPointCount = 0 };
    std::vector<SECurveControlPoint*> internalPointsCache = {}; // To prevent having to create new vectors when getting control points
};
