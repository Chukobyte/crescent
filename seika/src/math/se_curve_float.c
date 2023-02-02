#include "se_curve_float.h"
#include "se_math.h"
#include "../utils/se_assert.h"

// Helper functions
void control_point_swap(SECurveControlPoint* pointA, SECurveControlPoint* pointB) {
    SECurveControlPoint temp = *pointA;
    *pointA = *pointB;
    *pointB = temp;
}

// Function to perform Selection Sort
void selection_sort_curve_float(SECurveFloat* curve) {
    size_t minX;
    // One by one move boundary of unsorted subarray
    for (size_t i = 0; i < curve->controlPointCount - 1; i++) {
        // Find the minimum element in unsorted array
        minX = i;
        for (size_t j = i + 1; j < curve->controlPointCount; j++) {
            if (curve->controlPoints[j].x < curve->controlPoints[minX].x) {
                minX = j;
            }
        }
        // Swap the found minimum element
        // with the first element
        control_point_swap(&curve->controlPoints[minX], &curve->controlPoints[i]);
    }
}

//--- SECurveFloat ---//
void se_curve_float_add_control_point(SECurveFloat* curve, SECurveControlPoint point) {
    SE_ASSERT_FMT(curve->controlPointCount + 1 < SE_CURVE_MAX_CONTROL_POINTS, "Trying to add more points than max '%u'", SE_CURVE_MAX_CONTROL_POINTS);
    curve->controlPoints[curve->controlPointCount++] = point;
    selection_sort_curve_float(curve);
}

void se_curve_float_add_control_points(SECurveFloat* curve, SECurveControlPoint points[], size_t count) {
    if (count == 0) {
        return;
    }
    for (size_t i = 0; i < count; i++) {
        SE_ASSERT_FMT(curve->controlPointCount + 1 < SE_CURVE_MAX_CONTROL_POINTS, "Trying to add multiple points that go beyond the max of '%u'", SE_CURVE_MAX_CONTROL_POINTS);
        curve->controlPoints[curve->controlPointCount++] = points[i];
    }
    selection_sort_curve_float(curve);
}

bool se_curve_float_remove_control_point(SECurveFloat* curve, double x, double y) {
    for (size_t i = 0; i < curve->controlPointCount; i++) {
        SECurveControlPoint* point = &curve->controlPoints[i];
        if (se_math_is_almost_equal_double_default(x, point->x) && se_math_is_almost_equal_double_default(y, point->y)) {
            // We've found a matching point so set it's x to the highest value, sort it, then decrement the point count.
            point->x = DBL_MAX;
            selection_sort_curve_float(curve);
            curve->controlPointCount--;
            return true;
        }
    }
    return false;
}

double se_curve_float_eval(SECurveFloat* curve, double t) {
    if (curve->controlPointCount == 0) {
        return 0.0;
    } else if (curve->controlPointCount == 1) {
        return curve->controlPoints[0].y;
    }
    size_t leftIndex = 0;
    size_t rightIndex = curve->controlPointCount - 1;
    while (rightIndex - leftIndex > 1) {
        size_t midIndex = (leftIndex + rightIndex) / 2;
        if (t < curve->controlPoints[midIndex].x) {
            rightIndex = midIndex;
        } else {
            leftIndex = midIndex;
        }
    }

    const SECurveControlPoint* leftPoint = &curve->controlPoints[leftIndex];
    const SECurveControlPoint* rightPoint = &curve->controlPoints[rightIndex];

    const double t1 = (t - leftPoint->x) / (rightPoint->x - leftPoint->x);
    const double t2 = t1 * t1;
    const double t3 = t2 * t1;

    const double a = 2.0f * t3 - 3.0f * t2 + 1.0f;
    const double b = -2.0f * t3 + 3.0f * t2;
    const double c = t3 - 2.0f * t2 + t1;
    const double d = t3 - t2;

    return a * leftPoint->y + b * rightPoint->y + c * leftPoint->tangentOut + d * rightPoint->tangentIn;
}
