
#include <stdio.h>
#include "ts_calib.h"



//todo: this should be feed by  actions.c -> calib screens
static CalibrationPoint calib_points[5] = {
    {  40,  42,  30,  30 },  // top-left
    { 190,  40, 210,  30 },  // top-right
    { 194, 262, 210, 290 },  // bottom-right
    {  39, 258,  30, 290 },  // bottom-left
    { 119, 149, 120, 160 }   // center
};

CalibrationMatrix calib_matrix_nvm;

CalibrationMatrix compute_affine_matrix(const CalibrationPoint *points, int count) {
    float sum_tx = 0, sum_ty = 0;
    float sum_sx = 0, sum_sy = 0;
    float sum_tx_tx = 0, sum_ty_ty = 0, sum_tx_ty = 0;
    float sum_tx_sx = 0, sum_ty_sx = 0;
    float sum_tx_sy = 0, sum_ty_sy = 0;

    for (int i = 0; i < count; ++i) {
        float tx = points[i].touch_x;
        float ty = points[i].touch_y;
        float sx = points[i].screen_x;
        float sy = points[i].screen_y;

        sum_tx += tx;
        sum_ty += ty;
        sum_sx += sx;
        sum_sy += sy;

        sum_tx_tx += tx * tx;
        sum_ty_ty += ty * ty;
        sum_tx_ty += tx * ty;

        sum_tx_sx += tx * sx;
        sum_ty_sx += ty * sx;

        sum_tx_sy += tx * sy;
        sum_ty_sy += ty * sy;
    }

    float det = count * (sum_tx_tx * sum_ty_ty - sum_tx_ty * sum_tx_ty)
              + sum_tx * (sum_tx_ty * sum_ty - sum_tx * sum_ty_ty)
              + sum_ty * (sum_tx * sum_tx_ty - sum_tx_tx * sum_ty);

    CalibrationMatrix m = {0};

    if (det != 0) {
        float inv_det = 1.0f / det;

        m.a = inv_det * (count * sum_tx_sx - sum_tx * sum_sx);
        m.b = inv_det * (count * sum_ty_sx - sum_ty * sum_sx);
        m.c = inv_det * (sum_tx * sum_ty_sx - sum_ty * sum_tx_sx);

        m.d = inv_det * (count * sum_tx_sy - sum_tx * sum_sy);
        m.e = inv_det * (count * sum_ty_sy - sum_ty * sum_sy);
        m.f = inv_det * (sum_tx * sum_ty_sy - sum_ty * sum_tx_sy);
    }

    return m;
}


void ts_calib_init(void) {
     calib_matrix_nvm = compute_affine_matrix(calib_points, 5);
}

void ts_calib_get(CalibrationMatrix *ptr) {
    if (ptr != NULL)
        ptr = &calib_matrix_nvm;
}