#include "bezier_curve.h"

//--- BezierMath ---//
BezierPoint BezierMath::Linear(const BezierPoint& p1, const BezierPoint& p2, double t) {
    return se_math_vec2_lerp(&p1, &p2, (float) t);
}

BezierPoint BezierMath::Quadratic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, double t) {
    return Linear(
               Linear(p0, p1, t),
               Linear(p1, p2, t),
               t
           );
}

BezierPoint BezierMath::Cubic(const BezierPoint& p0, const BezierPoint& p1, const BezierPoint& p2, const BezierPoint& p3, double t) {
    return Linear(
               Quadratic(p0, p1, p2, t),
               Quadratic(p1, p2, p3, t),
               t
           );
}

//--- CubicBezierCurve ---//
BezierPoint CubicBezierCurve::Eval(double t) const {
    const BezierPoint p00 = p0;
    const BezierPoint p01 = p1;
    const BezierPoint p02 = p2;
    const BezierPoint p03 = p3;

    const BezierPoint p10 = BezierMath::Linear(p00, p01, t);
    const BezierPoint p11 = BezierMath::Linear(p01, p02, t);
    const BezierPoint p12 = BezierMath::Linear(p02, p03, t);
    const BezierPoint p20 = BezierMath::Linear(p10, p11, t);
    const BezierPoint p21 = BezierMath::Linear(p11, p12, t);
    const BezierPoint p30 = BezierMath::Linear(p20, p21, t);

    return p30;
}
