#ifndef _TS_CALIB_H_
#define _TS_CALIB_H_


typedef struct {
    int touch_x, touch_y;
    int screen_x, screen_y;
} CalibrationPoint;

typedef struct {
    float a, b, c;
    float d, e, f;
} CalibrationMatrix;

CalibrationMatrix compute_affine_matrix(const CalibrationPoint *points, int count);
void ts_calib_init(void);
void ts_calib_get(CalibrationMatrix *ptr) ;

#endif