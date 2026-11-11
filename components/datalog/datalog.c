#include "datalog.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "can_if.h"
#include "canmap.h"
#include "channels.h"

static const char *TAG = "datalog";

#define TASK_STACK          4096
#define TASK_PRIO           3           /* below the decoder and LVGL */
#define RAW_QUEUE_LEN       1024        /* ~0.25 s of a fully loaded 500 kbit/s bus */
#define FILE_BUF_SIZE       16384
#define MAX_FILES           999
#define EVT_STATS_PERIOD_MS 1000

typedef struct {
    FILE     *f;
    char     *buf;                      /* stdio buffer: one SD write per FILE_BUF_SIZE */
} log_file_t;

static char          s_dir[24];
static QueueHandle_t s_raw_queue;
static volatile bool s_want_raw;
static volatile bool s_want_params;

static log_file_t    s_raw, s_par, s_evt;
static bool          s_session;                 /* bus active, files belong to session s_num */
static int           s_num;
static bool          s_failed;                  /* SD error: stopped until datalog_retry() */
static volatile bool s_retry_req;
static datalog_remount_fn_t s_remount;
static uint32_t      s_last_rx_count;
static uint32_t      s_last_rx_ms;

static ch_id_t      *s_cols;
static size_t        s_ncols;

static datalog_status_t s_status;
static portMUX_TYPE     s_lock = portMUX_INITIALIZER_UNLOCKED;

/* ---------------------------------------------------------------- status */

static void set_error(const char *msg)
{
    ESP_LOGW(TAG, "%s", msg);
    portENTER_CRITICAL(&s_lock);
    snprintf(s_status.error, sizeof(s_status.error), "%s", msg);
    s_status.error_count++;
    portEXIT_CRITICAL(&s_lock);
}

void datalog_get_status(datalog_status_t *out)
{
    can_if_stats_t cs;
    can_if_get_stats(&cs);
    portENTER_CRITICAL(&s_lock);
    *out = s_status;
    portEXIT_CRITICAL(&s_lock);
    out->raw_enabled = s_want_raw;
    out->params_enabled = s_want_params;
    out->raw_dropped = cs.mon_dropped;
}

/* No automatic retries: the board has no card detect, and retrying a missing
 * card only fills the console. The user retries from the settings screen. */
static void fail(const char *msg)
{
    set_error(msg);
    s_failed = true;
    portENTER_CRITICAL(&s_lock);
    s_status.failed = true;
    portEXIT_CRITICAL(&s_lock);
}

/* ---------------------------------------------------------------- files */

static bool open_file(log_file_t *lf, const char *prefix, const char *ext, char *name_out, size_t name_len)
{
    char path[64];
    snprintf(path, sizeof(path), "%s/%s_%03d.%s", s_dir, prefix, s_num, ext);
    lf->f = fopen(path, "w");
    if (lf->f == NULL) {
        return false;
    }
    if (lf->buf != NULL) {
        setvbuf(lf->f, lf->buf, _IOFBF, FILE_BUF_SIZE);
    }
    if (name_out) {
        snprintf(name_out, name_len, "%s_%03d.%s", prefix, s_num, ext);
    }
    ESP_LOGI(TAG, "logging to %s", path);
    return true;
}

static void sync_file(log_file_t *lf)
{
    if (lf->f == NULL) {
        return;
    }
    if (fflush(lf->f) != 0 || fsync(fileno(lf->f)) != 0 || ferror(lf->f)) {
        fail("SD write failed, log closed");
        fclose(lf->f);
        lf->f = NULL;
    }
}

static void close_file(log_file_t *lf)
{
    sync_file(lf);
    if (lf->f != NULL) {
        fclose(lf->f);
        lf->f = NULL;
    }
}

/* Next session number: one past the highest can_/par_ file in the directory. */
static bool next_session_number(void)
{
    struct stat st;
    if (stat(s_dir, &st) != 0 && mkdir(s_dir, 0775) != 0) {
        return false;
    }
    char path[64];
    for (int n = 1; n <= MAX_FILES; n++) {
        snprintf(path, sizeof(path), "%s/can_%03d.log", s_dir, n);
        if (stat(path, &st) == 0) {
            continue;
        }
        snprintf(path, sizeof(path), "%s/par_%03d.csv", s_dir, n);
        if (stat(path, &st) == 0) {
            continue;
        }
        s_num = n;
        return true;
    }
    return false;
}

/* ---------------------------------------------------------------- event log */

static can_if_stats_t s_evt_cs;
static uint32_t       s_evt_next;

static const char *bus_state_name(can_if_bus_state_t st)
{
    switch (st) {
        case CAN_IF_BUS_ACTIVE:  return "active";
        case CAN_IF_BUS_WARNING: return "warning";
        case CAN_IF_BUS_PASSIVE: return "passive";
        case CAN_IF_BUS_OFF:     return "bus-off";
        default:                 return "stopped";
    }
}

static void evt_printf(uint32_t now, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
static void evt_printf(uint32_t now, const char *fmt, ...)
{
    if (s_evt.f == NULL) {
        return;
    }
    fprintf(s_evt.f, "(%" PRIu32 ".%03" PRIu32 ") ", now / 1000, now % 1000);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(s_evt.f, fmt, ap);
    va_end(ap);
    fputc('\n', s_evt.f);
}

static void evt_header(uint32_t now)
{
    canmap_info_t mi;
    canmap_get_info(&mi);
    fprintf(s_evt.f, "# FP_Dash event log, time = s since boot (as in can_%03d.log)\n", s_num);
    evt_printf(now, "map: %s (%s) %u frames %u signals", mi.source, mi.description, mi.frames, mi.signals);
    if (mi.error[0] != '\0') {
        evt_printf(now, "map: SD map rejected: %s", mi.error);
    }
    can_if_get_stats(&s_evt_cs);
    evt_printf(now, "can: rx %" PRIu32 " bus_errors %" PRIu32 " state %s",
               s_evt_cs.rx_frames, s_evt_cs.bus_errors, bus_state_name(s_evt_cs.state));
    s_evt_next = now + EVT_STATS_PERIOD_MS;
}

static void evt_service(uint32_t now)
{
    if (s_evt.f == NULL || (int32_t)(now - s_evt_next) < 0) {
        return;
    }
    s_evt_next = now + EVT_STATS_PERIOD_MS;
    can_if_stats_t cs;
    can_if_get_stats(&cs);
    can_if_stats_t *o = &s_evt_cs;
    if (cs.rx_dropped != o->rx_dropped || cs.mon_dropped != o->mon_dropped ||
        cs.bus_errors != o->bus_errors || cs.state != o->state) {
        evt_printf(now, "can: +decoder_dropped %" PRIu32 " +log_dropped %" PRIu32 " +bus_errors %" PRIu32
                   " state %s  (rx %" PRIu32 " in the last second)",
                   cs.rx_dropped - o->rx_dropped, cs.mon_dropped - o->mon_dropped,
                   cs.bus_errors - o->bus_errors, bus_state_name(cs.state), cs.rx_frames - o->rx_frames);
    }
    *o = cs;
}

/* ---------------------------------------------------------------- raw CAN */

static void raw_write(const can_frame_t *fr)
{
    /* candump -L: (sec.usec) iface id#data */
    char line[64];
    int n = snprintf(line, sizeof(line), "(%" PRIu32 ".%03" PRIu32 "000) can0 ",
                     fr->ts_ms / 1000, fr->ts_ms % 1000);
    n += snprintf(line + n, sizeof(line) - n, fr->ext ? "%08" PRIX32 "#" : "%03" PRIX32 "#", fr->id);
    static const char hex[] = "0123456789ABCDEF";
    for (int i = 0; i < fr->dlc; i++) {
        line[n++] = hex[fr->data[i] >> 4];
        line[n++] = hex[fr->data[i] & 0xF];
    }
    line[n++] = '\n';
    fwrite(line, 1, n, s_raw.f);
}

/* ---------------------------------------------------------------- parameters */

static void params_header(void)
{
    /* MegaLogViewer: first row names, second row units. */
    fputs("Time", s_par.f);
    for (size_t i = 0; i < s_ncols; i++) {
        fprintf(s_par.f, ",%s", ch_info(s_cols[i])->name);
    }
    fputs("\ns", s_par.f);
    for (size_t i = 0; i < s_ncols; i++) {
        fprintf(s_par.f, ",%s", ch_info(s_cols[i])->unit);
    }
    fputc('\n', s_par.f);
}

static void params_row(uint32_t now)
{
    fprintf(s_par.f, "%" PRIu32 ".%03" PRIu32, now / 1000, now % 1000);
    for (size_t i = 0; i < s_ncols; i++) {
        ch_sample_t smp;
        if (ch_get(s_cols[i], now, &smp)) {
            fprintf(s_par.f, ",%.*f", ch_info(s_cols[i])->decimals, smp.value);
        } else {
            fputc(',', s_par.f);
        }
    }
    fputc('\n', s_par.f);
    portENTER_CRITICAL(&s_lock);
    s_status.params_rows++;
    portEXIT_CRITICAL(&s_lock);
}

/* ---------------------------------------------------------------- session */

static void session_close(void)
{
    if (s_raw.f) {
        close_file(&s_raw);
        ESP_LOGI(TAG, "raw log closed (%" PRIu32 " frames)", s_status.raw_frames);
    }
    if (s_par.f) {
        close_file(&s_par);
        ESP_LOGI(TAG, "params log closed (%" PRIu32 " rows)", s_status.params_rows);
    }
    close_file(&s_evt);
    can_if_set_monitor(NULL);
    s_session = false;
    portENTER_CRITICAL(&s_lock);
    s_status.raw_active = false;
    s_status.params_active = false;
    portEXIT_CRITICAL(&s_lock);
}

/* Opens what is enabled but not open yet. Raw/params enabled mid-session join
 * the current session number. */
static void session_open(uint32_t now)
{
    if (s_failed) {
        return;
    }
    if (!s_session) {
        if (!next_session_number()) {
            fail("cannot create log dir (no SD card?)");
            return;
        }
        s_session = true;
    }
    bool failed = false;
    if (s_want_raw && s_raw.f == NULL) {
        char name[24];
        if (open_file(&s_raw, "can", "log", name, sizeof(name))) {
            xQueueReset(s_raw_queue);
            can_if_set_monitor(s_raw_queue);
            portENTER_CRITICAL(&s_lock);
            strcpy(s_status.raw_file, name);
            s_status.raw_frames = 0;
            s_status.raw_active = true;
            portEXIT_CRITICAL(&s_lock);
            if (open_file(&s_evt, "evt", "txt", NULL, 0)) {
                evt_header(now);
            }
        } else {
            failed = true;
        }
    }
    if (s_want_params && s_par.f == NULL && s_ncols > 0) {
        char name[24];
        if (open_file(&s_par, "par", "csv", name, sizeof(name))) {
            params_header();
            portENTER_CRITICAL(&s_lock);
            strcpy(s_status.params_file, name);
            s_status.params_rows = 0;
            s_status.params_active = true;
            portEXIT_CRITICAL(&s_lock);
        } else {
            failed = true;
        }
    }
    if (failed) {
        session_close();
        fail("cannot create log file (SD card full / read-only?)");
    }
}

static void session_service(uint32_t now)
{
    can_if_stats_t cs;
    can_if_get_stats(&cs);
    if (cs.rx_frames != s_last_rx_count) {
        s_last_rx_count = cs.rx_frames;
        s_last_rx_ms = now;
    }
    bool bus_active = s_last_rx_ms != 0 && (now - s_last_rx_ms) < CONFIG_DATALOG_IDLE_CLOSE_MS;

    if (s_session && !bus_active) {
        ESP_LOGI(TAG, "no CAN traffic for %d ms, closing session %d", CONFIG_DATALOG_IDLE_CLOSE_MS, s_num);
        session_close();
        return;
    }
    /* switched off from the UI */
    if (!s_want_raw && s_raw.f != NULL) {
        can_if_set_monitor(NULL);
        close_file(&s_raw);
        close_file(&s_evt);
        portENTER_CRITICAL(&s_lock);
        s_status.raw_active = false;
        portEXIT_CRITICAL(&s_lock);
    }
    if (!s_want_params && s_par.f != NULL) {
        close_file(&s_par);
        portENTER_CRITICAL(&s_lock);
        s_status.params_active = false;
        portEXIT_CRITICAL(&s_lock);
    }
    if (bus_active && ((s_want_raw && s_raw.f == NULL) || (s_want_params && s_par.f == NULL && s_ncols > 0))) {
        session_open(now);
    }
}

/* ---------------------------------------------------------------- retry */

static void do_retry(void)
{
    s_retry_req = false;
    session_close();                    /* nothing may be open during the remount */
    esp_err_t err = s_remount ? s_remount() : ESP_OK;
    if (err != ESP_OK) {
        char msg[48];
        snprintf(msg, sizeof(msg), "SD card: %s", esp_err_to_name(err));
        fail(msg);
        portENTER_CRITICAL(&s_lock);
        s_status.retrying = false;
        portEXIT_CRITICAL(&s_lock);
        return;
    }
    s_failed = false;
    portENTER_CRITICAL(&s_lock);
    s_status.failed = false;
    s_status.retrying = false;
    s_status.error[0] = '\0';
    portEXIT_CRITICAL(&s_lock);
    ESP_LOGI(TAG, "SD card ready, logging resumes with the next CAN frame");
}

void datalog_retry(void)
{
    portENTER_CRITICAL(&s_lock);
    s_status.retrying = true;
    portEXIT_CRITICAL(&s_lock);
    s_retry_req = true;
}

/* ---------------------------------------------------------------- task */

static void datalog_task(void *arg)
{
    uint32_t next_row = can_if_now_ms();
    uint32_t next_sync = next_row + CONFIG_DATALOG_SYNC_PERIOD_MS;
    can_frame_t fr;

    while (1) {
        uint32_t now = can_if_now_ms();
        if (s_retry_req) {
            do_retry();
        }
        session_service(now);
        evt_service(now);

        /* Drain CAN frames until the next parameter row is due. */
        int32_t wait = (int32_t)(next_row - now);
        if (xQueueReceive(s_raw_queue, &fr, pdMS_TO_TICKS(wait > 0 ? wait : 0)) == pdTRUE) {
            uint32_t written = 0;
            do {
                if (s_raw.f != NULL) {
                    raw_write(&fr);
                    written++;
                }
            } while (written < RAW_QUEUE_LEN && xQueueReceive(s_raw_queue, &fr, 0) == pdTRUE);
            portENTER_CRITICAL(&s_lock);
            s_status.raw_frames += written;
            portEXIT_CRITICAL(&s_lock);
        }

        now = can_if_now_ms();
        if ((int32_t)(now - next_row) >= 0) {
            if (s_par.f != NULL) {
                params_row(now);
            }
            next_row += CONFIG_DATALOG_PARAM_PERIOD_MS;
            if ((int32_t)(now - next_row) > CONFIG_DATALOG_PARAM_PERIOD_MS) {
                next_row = now + CONFIG_DATALOG_PARAM_PERIOD_MS;   /* fell behind: skip */
            }
        }
        if ((int32_t)(now - next_sync) >= 0) {
            sync_file(&s_raw);
            sync_file(&s_par);
            sync_file(&s_evt);
            next_sync = now + CONFIG_DATALOG_SYNC_PERIOD_MS;
        }
    }
}

static char *alloc_buf(void)
{
    return heap_caps_malloc(FILE_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

esp_err_t datalog_init(const char *dir, bool raw, bool params, datalog_remount_fn_t remount)
{
    s_remount = remount;
    snprintf(s_dir, sizeof(s_dir), "%s/log", dir);
    s_raw_queue = xQueueCreate(RAW_QUEUE_LEN, sizeof(can_frame_t));
    if (s_raw_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }
    s_raw.buf = alloc_buf();
    s_par.buf = alloc_buf();

    /* Columns: every channel marked for logging, in map order. */
    size_t n = ch_count();
    s_cols = calloc(n ? n : 1, sizeof(ch_id_t));
    if (s_cols == NULL) {
        return ESP_ERR_NO_MEM;
    }
    for (size_t i = 0; i < n; i++) {
        if (ch_info((ch_id_t)i)->log) {
            s_cols[s_ncols++] = (ch_id_t)i;
        }
    }

    s_want_raw = raw;
    s_want_params = params;
    if (xTaskCreate(datalog_task, "datalog", TASK_STACK, NULL, TASK_PRIO, NULL) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGI(TAG, "ready (%s, %u logged signals, row every %d ms)", s_dir, (unsigned)s_ncols,
             CONFIG_DATALOG_PARAM_PERIOD_MS);
    return ESP_OK;
}

void datalog_enable_raw(bool on)
{
    s_want_raw = on;
}

void datalog_enable_params(bool on)
{
    s_want_params = on;
}
