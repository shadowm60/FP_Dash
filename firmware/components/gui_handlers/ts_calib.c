
#include <stdio.h>
#include <math.h>
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
    const int32_t scale = 10000;

    if (fabsf(det) > 1e-6f) {
        float inv_det = 1.0f / det;

        float a_f = inv_det * (count * sum_tx_sx - sum_tx * sum_sx);
        float b_f = inv_det * (count * sum_ty_sx - sum_ty * sum_sx);
        float c_f = inv_det * (sum_tx * sum_ty_sx - sum_ty * sum_tx_sx);

        float d_f = inv_det * (count * sum_tx_sy - sum_tx * sum_sy);
        float e_f = inv_det * (count * sum_ty_sy - sum_ty * sum_sy);
        float f_f = inv_det * (sum_tx * sum_ty_sy - sum_ty * sum_tx_sy);

        m.a = (int32_t)(a_f * scale);
        m.b = (int32_t)(b_f * scale);
        m.c = (int32_t)(c_f * scale);
        m.d = (int32_t)(d_f * scale);
        m.e = (int32_t)(e_f * scale);
        m.f = (int32_t)(f_f * scale);
        m.scale = scale;
    }

    return m;
}


CalibrationMatrix compute_affine_3pt(const CalibrationPoint *p) {
    CalibrationMatrix m = {0};
    const int32_t scale = 10000;

    float x1 = p[0].touch_x, y1 = p[0].touch_y;
    float x2 = p[1].touch_x, y2 = p[1].touch_y;
    float x3 = p[2].touch_x, y3 = p[2].touch_y;

    float u1 = p[0].screen_x, v1 = p[0].screen_y;
    float u2 = p[1].screen_x, v2 = p[1].screen_y;
    float u3 = p[2].screen_x, v3 = p[2].screen_y;

    float det = (x1*(y2 - y3) - x2*(y1 - y3) + x3*(y1 - y2));
    if (fabsf(det) < 1e-6f) return m;

    float a = ((u1*(y2 - y3) - u2*(y1 - y3) + u3*(y1 - y2)) / det);
    float b = ((u1*(x3 - x2) + u2*(x1 - x3) + u3*(x2 - x1)) / det);
    float c = ((u1*(x2*y3 - x3*y2) - u2*(x1*y3 - x3*y1) + u3*(x1*y2 - x2*y1)) / det);

    float d = ((v1*(y2 - y3) - v2*(y1 - y3) + v3*(y1 - y2)) / det);
    float e = ((v1*(x3 - x2) + v2*(x1 - x3) + v3*(x2 - x1)) / det);
    float f = ((v1*(x2*y3 - x3*y2) - v2*(x1*y3 - x3*y1) + v3*(x1*y2 - x2*y1)) / det);

    m.a = (int32_t)(a * scale);
    m.b = (int32_t)(b * scale);
    m.c = (int32_t)(c * scale);
    m.d = (int32_t)(d * scale);
    m.e = (int32_t)(e * scale);
    m.f = (int32_t)(f * scale);
    m.scale = scale;

    return m;
}

void ts_calib_init(void) {
     //calib_matrix_nvm = compute_affine_matrix(calib_points, 5);
     calib_matrix_nvm = compute_affine_3pt(calib_points);
}

void ts_calib_get(CalibrationMatrix *ptr) {
    if (ptr != NULL)
        ptr = &calib_matrix_nvm;
}