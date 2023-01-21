#include "bezier_curve.h"

//--- BezierMath ---//
BezierPoint BezierMath::Lerp(const BezierPoint& p1, const BezierPoint& p2, float t) {
    const float x = std::lerp(p1.x, p2.x, t);
    const float y = std::lerp(p1.y, p2.y, t);
    return { x, y };
}

BezierPoint BezierMath::Linear(const BezierPoint& p1, const BezierPoint& p2, float t) {
    return Lerp(p1, p2, t);
}

BezierPoint BezierMath::Quadratic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, float t) {
    return Lerp(
               Lerp(p0, p1, t),
               Lerp(p1, p2, t),
               t
           );
}

BezierPoint BezierMath::Cubic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, const BezierPoint& p3, float t) {
    return Lerp(
               Quadratic(p0, p1, p2, t),
               Quadratic(p1, p2, p3, t),
               t
           );
}

//--- CubicBezierCurve ---//
BezierPoint CubicBezierCurve::Eval(float t) const {
    const BezierPoint p00 = p0;
    const BezierPoint p01 = p1;
    const BezierPoint p02 = p2;
    const BezierPoint p03 = p3;

    const BezierPoint p10 = BezierMath::Lerp(p00, p01, t);
    const BezierPoint p11 = BezierMath::Lerp(p01, p02, t);
    const BezierPoint p12 = BezierMath::Lerp(p02, p03, t);
    const BezierPoint p20 = BezierMath::Lerp(p10, p11, t);
    const BezierPoint p21 = BezierMath::Lerp(p11, p12, t);
    const BezierPoint p30 = BezierMath::Lerp(p20, p21, t);

    return p30;
}
