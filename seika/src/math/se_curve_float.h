#pragma once

#include <stddef.h>
#include <stdbool.h>

#define SE_CURVE_MAX_CONTROL_POINTS 16

typedef struct SECurveControlPoint {
    double x; // a.k.a time
    double y; // a.k.a value
    double tangentIn;
    double tangentOut;
} SECurveControlPoint;

typedef struct SECurveFloat {
    size_t controlPointCount;
    SECurveControlPoint controlPoints[SE_CURVE_MAX_CONTROL_POINTS];
} SECurveFloat;

void se_curve_float_add_control_point(SECurveFloat* curve, SECurveControlPoint point);
void se_curve_float_add_control_points(SECurveFloat* curve, SECurveControlPoint points[], size_t count);
bool se_curve_float_remove_control_point(SECurveFloat* curve, double x, double y);
double se_curve_float_eval(SECurveFloat* curve, double t);
