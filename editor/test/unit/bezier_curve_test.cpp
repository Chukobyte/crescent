#include <catch.hpp>

#include <iostream>

#include "../../src/core/math/bezier_curve.h"

TEST_CASE("Curve Float", "[curve_float]") {
    SECTION("Curve Float no points") {
        CurveFloat curveFloat = CurveFloat();
        REQUIRE(curveFloat.Eval(0.0f) == 0.0f);
    }

    SECTION("Curve Float one point") {
        const std::vector<CurveFloat::ControlPoint> controlPoints = {
            { .position = 0.3f, .value = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
        };
        CurveFloat curveFloat = CurveFloat(controlPoints);
        REQUIRE(curveFloat.Eval(0.3f) == 10.0f);
    }

    SECTION("Curve Float two points") {
        const std::vector<CurveFloat::ControlPoint> controlPoints = {
            { .position = 0.3f, .value = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            { .position = 1.0f, .value = 20.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
        };
        CurveFloat curveFloat = CurveFloat(controlPoints);
        REQUIRE(curveFloat.Eval(0.3f) == 10.0f);
        REQUIRE(curveFloat.Eval(1.0f) == 20.0f);
    }

    SECTION("Curve Float three points") {
        const std::vector<CurveFloat::ControlPoint> controlPoints = {
            { .position = 0.0f, .value = 10.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            { .position = 5.0f, .value = 20.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
            { .position = 10.0f, .value = 30.0f, .tangentIn = 0.0f, .tangentOut = 0.0f },
        };
        CurveFloat curveFloat = CurveFloat(controlPoints);
        REQUIRE(curveFloat.Eval(0.0f) == 10.0f);
        REQUIRE(curveFloat.Eval(2.5f) == 15.0f);
        REQUIRE(curveFloat.Eval(5.0f) == 20.0f);
        REQUIRE(curveFloat.Eval(10.0f) == 30.0f);
    }
}

TEST_CASE("Bezier Curve", "[bezier_curve]") {
    SECTION("Cubic Bezier Curve") {
        const CubicBezierCurve curve = {
            .p0 = { 0.0f, 0.0f },
            .p1 = { 1.0f, 2.0f },
            .p2 = { 3.0f, 2.0f },
            .p3 = { 4.0f, 0.0f }
        };
        const BezierPoint point = curve.Eval(0.5f);
        REQUIRE(point.x == 2.0f);
        REQUIRE(point.y == 1.5f);
    }
}
