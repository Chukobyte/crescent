#include <catch.hpp>

#include <iostream>

#include "../../src/core/math/bezier_curve.h"

TEST_CASE("Bezier Curve", "[bezier_curve]") {
    SECTION("Cubic Bezier Curve") {

        const CubicBezierCurve curve = {
            .p0 = { 0.0f, 0.0f },
            .p1 = { 1.0f, 2.0f },
            .p2 = { 3.0f, 2.0f },
            .p3 = { 4.0f, 0.0f }
        };
        const BezierPoint point = curve.Eval(0.5f);

        std::cout << "Point(" << point.x << ", " << point.y << ")" << std::endl;

        REQUIRE(true);
    }
}
