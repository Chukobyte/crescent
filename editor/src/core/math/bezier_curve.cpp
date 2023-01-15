#include "bezier_curve.h"

namespace {
    BezierPoint LerpPoint(const BezierPoint& p1, const BezierPoint& p2, float t) {
        const float x = std::lerp(p1.x, p2.x, t);
        const float y = std::lerp(p1.y, p2.y, t);
        return { x, y };
    }
} // namespace

BezierPoint CubicBezierCurve::Eval(float t) const {
    const BezierPoint p00 = p0;
    const BezierPoint p01 = p1;
    const BezierPoint p02 = p2;
    const BezierPoint p03 = p3;

    const BezierPoint p10 = LerpPoint(p00, p01, t);
    const BezierPoint p11 = LerpPoint(p01, p02, t);
    const BezierPoint p12 = LerpPoint(p02, p03, t);
    const BezierPoint p20 = LerpPoint(p10, p11, t);
    const BezierPoint p21 = LerpPoint(p11, p12, t);
    const BezierPoint p30 = LerpPoint(p20, p21, t);

    return p30;
}
