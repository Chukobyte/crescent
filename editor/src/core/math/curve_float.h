#pragma once

#include <vector>

#include "../seika/src/math/se_curve_float.h"
#include "../seika/src/utils/se_assert.h"

#include "../utils/json_helper.h"

// A c++ wrapper class for seika curve float api
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

    template<typename JsonType = nlohmann::ordered_json>
    [[nodiscard]] JsonType ToJson() const {
        JsonType json;
        json["type"] = "curve";
        JsonType controlPointsJsonArray = JsonType::array();
        for (auto* point : internalPointsCache) {
            JsonType pointJson;
            pointJson["x"] = point->x;
            pointJson["y"] = point->y;
            pointJson["tangent_in"] = point->tangentIn;
            pointJson["tangent_out"] = point->tangentOut;
            controlPointsJsonArray.emplace_back(pointJson);
        }
        json["control_points"] = controlPointsJsonArray;
        return json;
    }

    template<typename JsonType = nlohmann::ordered_json>
    void SetFromJson(const JsonType& json) {
        curve.controlPointCount = 0; // Reset control point count
        const std::string type = JsonHelper::Get<std::string>(json, "type");
        SE_ASSERT_FMT(type == "curve", "Attempted to read json that is not a curve!");
        std::vector<SECurveControlPoint> points;
        JsonType controlPointsArray = JsonHelper::Get<JsonType>(json, "control_points");
        for (const JsonType& pointJson : controlPointsArray) {
            const double x = JsonHelper::Get<double>(pointJson, "x");
            const double y = JsonHelper::Get<double>(pointJson, "y");
            const double tangentIn = JsonHelper::Get<double>(pointJson, "tangent_in");
            const double tangentOut = JsonHelper::Get<double>(pointJson, "tangent_out");
            const SECurveControlPoint point = { .x = x, .y = y, .tangentIn = tangentIn, .tangentOut = tangentOut };
            points.emplace_back(point);
        }
        se_curve_float_add_control_points(&curve, points.data(), points.size());
        UpdateInternalPointsCache();
    }

  private:
    void UpdateInternalPointsCache() {
        internalPointsCache.clear();
        for (size_t i = 0; i < curve.controlPointCount; i++) {
            internalPointsCache.emplace_back(&curve.controlPoints[i]);
        }
    }

    SECurveFloat curve = { .controlPointCount = 0 };
    std::vector<SECurveControlPoint*> internalPointsCache = {}; // To prevent having to create new vectors when getting control points
};
