/*
 * datalog - SD card logging, runs in its own low priority task.
 *
 *   raw     every CAN frame on the bus, candump log format
 *           /sdcard/log/can_NNN.log   "(12.345000) can0 201#E803..."
 *   params  one row per CONFIG_DATALOG_PARAM_PERIOD_MS with the latest value
 *           of every signal marked log="1" in the map; empty cell = no fresh
 *           value. MegaLogViewer CSV: names row, units row, then data.
 *           /sdcard/log/par_NNN.csv
 *   events  with the raw log: map in use, CAN error counters, dropped frames
 *           /sdcard/log/evt_NNN.txt
 *
 * Files open on the first CAN frame and close after CONFIG_DATALOG_IDLE_CLOSE_MS
 * without traffic (ignition off), so every drive gets its own files.
 * Raw, params and events of one session share the same number NNN.
 * Timestamps are seconds since boot (the board has no RTC).
 */
#ifndef DATALOG_H
#define DATALOG_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool     raw_enabled;
    bool     params_enabled;
    bool     raw_active;        /* file open and being written */
    bool     params_active;
    char     raw_file[24];      /* current or last file name, "" if none */
    char     params_file[24];
    uint32_t raw_frames;        /* frames written to the current raw file */
    uint32_t raw_dropped;       /* frames lost (queue full) since boot */
    uint32_t params_rows;
    uint32_t error_count;       /* increments on every open / write failure */
    char     error[48];         /* last error, "" if none */
    bool     failed;            /* SD error: logging stopped until datalog_retry() */
    bool     retrying;          /* datalog_retry() requested, not finished yet */
} datalog_status_t;

/* Remounts the SD card (unmount + mount). Called from the datalog task with
 * all log files closed. */
typedef esp_err_t (*datalog_remount_fn_t)(void);

/* dir: mount point of the SD card, e.g. "/sdcard". Logs go to <dir>/log.
 * raw / params: initial state (the stored settings).
 * remount: used by datalog_retry(); NULL = only retry opening the files. */
esp_err_t datalog_init(const char *dir, bool raw, bool params, datalog_remount_fn_t remount);

/* Idempotent; safe to call from any task. */
void datalog_enable_raw(bool on);
void datalog_enable_params(bool on);

void datalog_get_status(datalog_status_t *out);

/* After an SD error logging stops (no automatic retries: the board has no
 * card detect). This closes any open files, remounts the card and resumes
 * logging; a new session starts with the next CAN frame. Any task. */
void datalog_retry(void);

#ifdef __cplusplus
}
#endif

#endif /* DATALOG_H */
