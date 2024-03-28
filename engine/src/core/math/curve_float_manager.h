#pragma once

#include <stddef.h>
#include <stdbool.h>

#include <seika/defines.h>

#define CRE_MAX_CURVE_FLOATS 1000
#define CRE_CURVE_FLOAT_INVALID_ID CRE_MAX_CURVE_FLOATS

typedef uint32 CurveFloatId;

void cre_curve_float_manager_init();
void cre_curve_float_manager_finalize();
CurveFloatId cre_curve_float_manager_create_new();
void cre_curve_float_manager_delete(CurveFloatId curveId);
bool cre_curve_float_manager_load_from_file(CurveFloatId curveId, const char* filePath);
void cre_curve_float_manager_add_point(CurveFloatId curveId, f64 x, f64 y, f64 tangentIn, f64 tangentOut);
bool cre_curve_float_manager_remove_point(CurveFloatId curveId, f64 x, f64 y);
f64 cre_curve_float_manager_eval(CurveFloatId curveId, f64 t);
