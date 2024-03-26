#include <catch.hpp>

#include "../../src/core/math/curve_float.h"

TEST_CASE("Curve Float", "[curve_float]") {
    SECTION("Curve Float no points") {
        CurveFloat curveFloat = CurveFloat();
        REQUIRE(curveFloat.Eval(0.0f) == 0.0f);
    }

    SECTION("Curve Float one point") {
        const std::vector<SkaCurveControlPoint> controlPoints = {
            { .x = 0.3f, .y = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
        };
        CurveFloat curveFloat = CurveFloat(controlPoints);
        REQUIRE(curveFloat.Eval(0.3f) == 10.0f);
    }

    SECTION("Curve Float two points") {
        const std::vector<SkaCurveControlPoint> controlPoints = {
            { .x = 0.3f, .y = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            { .x = 1.0f, .y = 20.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
        };
        CurveFloat curveFloat = CurveFloat(controlPoints);
        REQUIRE(curveFloat.Eval(0.3f) == 10.0f);
        REQUIRE(curveFloat.Eval(1.0f) == 20.0f);
    }

    SECTION("Curve Float three points") {
        const std::vector<SkaCurveControlPoint> controlPoints = {
            { .x = 0.0f, .y = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            { .x = 5.0f, .y = 20.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            { .x = 10.0f, .y = 30.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
        };
        CurveFloat curveFloat = CurveFloat(controlPoints);
        REQUIRE(curveFloat.Eval(0.0f) == 10.0f);
        REQUIRE(curveFloat.Eval(2.5f) == 15.0f);
        REQUIRE(curveFloat.Eval(5.0f) == 20.0f);
        REQUIRE(curveFloat.Eval(10.0f) == 30.0f);
    }
}
