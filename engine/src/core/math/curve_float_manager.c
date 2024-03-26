#include "curve_float_manager.h"

#include <seika/math/curve_float.h>
#include <seika/data_structures/queue.h>
#include <seika/assert.h>

#include "../json/json_file_loader.h"

static SkaCurveFloat curves[CRE_MAX_CURVE_FLOATS] = {0};
static SkaQueue* idQueue = NULL;

void cre_curve_float_manager_init() {
    if (idQueue == NULL) {
        idQueue = ska_queue_create(CRE_MAX_CURVE_FLOATS, CRE_CURVE_FLOAT_INVALID_ID);
        for (uint32_t i = 0; i < CRE_MAX_CURVE_FLOATS; i++) {
            ska_queue_enqueue(idQueue, i);
        }
    }
}

void cre_curve_float_manager_finalize() {
    if (idQueue != NULL) {
        ska_queue_destroy(idQueue);
        idQueue = NULL;
    }
}

CurveFloatId cre_curve_float_manager_create_new() {
    SKA_ASSERT_FMT(!ska_queue_is_empty(idQueue), "Curve float id queue is empty!");
    CurveFloatId newId = ska_queue_dequeue(idQueue);
    curves[newId].controlPointCount = 0;
    return newId;
}

void cre_curve_float_manager_delete(CurveFloatId curveId) {
    SKA_ASSERT_FMT(!ska_queue_is_full(idQueue), "Curve float id queue is full!");
    ska_queue_enqueue(idQueue, curveId);
}

bool cre_curve_float_manager_load_from_file(CurveFloatId curveId, const char* filePath) {
    bool hasLoaded = false;
    curves[curveId] = cre_json_load_curve_float_file(filePath, &hasLoaded);
    return hasLoaded;
}

void cre_curve_float_manager_add_point(CurveFloatId curveId, double x, double y, double tangentIn, double tangentOut) {
    SKA_ASSERT_FMT(curveId < CRE_MAX_CURVE_FLOATS, "Invalid curve id");
    const SkaCurveControlPoint point = { .x = x, .y = y, .tangentIn = tangentIn, .tangentOut = tangentOut };
    ska_curve_float_add_control_point(&curves[curveId], point);
}

bool cre_curve_float_manager_remove_point(CurveFloatId curveId, double x, double y) {
    SKA_ASSERT_FMT(curveId < CRE_MAX_CURVE_FLOATS, "Invalid curve id");
    return ska_curve_float_remove_control_point(&curves[curveId], x, y);
}

double cre_curve_float_manager_eval(CurveFloatId curveId, double t) {
    SKA_ASSERT_FMT(curveId < CRE_MAX_CURVE_FLOATS, "Invalid curve id");
    return ska_curve_float_eval(&curves[curveId], t);
}
