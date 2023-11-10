#include "curve_float_manager.h"

#include "../seika/math/se_curve_float.h"
#include "../seika/data_structures/se_queue.h"
#include "../seika/utils/se_assert.h"

#include "../json/json_file_loader.h"

static SECurveFloat curves[CRE_MAX_CURVE_FLOATS] = {0};
static SEQueue* idQueue = NULL;

void cre_curve_float_manager_init() {
    if (idQueue == NULL) {
        idQueue = se_queue_create(CRE_MAX_CURVE_FLOATS, CRE_CURVE_FLOAT_INVALID_ID);
        for (uint32_t i = 0; i < CRE_MAX_CURVE_FLOATS; i++) {
            se_queue_enqueue(idQueue, i);
        }
    }
}

void cre_curve_float_manager_finalize() {
    if (idQueue != NULL) {
        se_queue_destroy(idQueue);
        idQueue = NULL;
    }
}

CurveFloatId cre_curve_float_manager_create_new() {
    SE_ASSERT_FMT(!se_queue_is_empty(idQueue), "Curve float id queue is empty!");
    CurveFloatId newId = se_queue_dequeue(idQueue);
    curves[newId].controlPointCount = 0;
    return newId;
}

void cre_curve_float_manager_delete(CurveFloatId curveId) {
    SE_ASSERT_FMT(!se_queue_is_full(idQueue), "Curve float id queue is full!");
    se_queue_enqueue(idQueue, curveId);
}

bool cre_curve_float_manager_load_from_file(CurveFloatId curveId, const char* filePath) {
    bool hasLoaded = false;
    curves[curveId] = cre_json_load_curve_float_file(filePath, &hasLoaded);
    return hasLoaded;
}

void cre_curve_float_manager_add_point(CurveFloatId curveId, double x, double y, double tangentIn, double tangentOut) {
    SE_ASSERT_FMT(curveId < CRE_MAX_CURVE_FLOATS, "Invalid curve id");
    const SECurveControlPoint point = { .x = x, .y = y, .tangentIn = tangentIn, .tangentOut = tangentOut };
    se_curve_float_add_control_point(&curves[curveId], point);
}

bool cre_curve_float_manager_remove_point(CurveFloatId curveId, double x, double y) {
    SE_ASSERT_FMT(curveId < CRE_MAX_CURVE_FLOATS, "Invalid curve id");
    return se_curve_float_remove_control_point(&curves[curveId], x, y);
}

double cre_curve_float_manager_eval(CurveFloatId curveId, double t) {
    SE_ASSERT_FMT(curveId < CRE_MAX_CURVE_FLOATS, "Invalid curve id");
    return se_curve_float_eval(&curves[curveId], t);
}
