#include "canmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include "can_if.h"
#include "mini_xml.h"

static const char *TAG = "canmap";

#define MAP_VERSION         1
#define MAX_FILE_SIZE       (512 * 1024)
#define DECODE_QUEUE_LEN    256
#define DECODE_TASK_STACK   4096
#define DECODE_TASK_PRIO    7           /* above datalog: values are never late. Core 0: the UI has core 1 */

extern const char default_map_start[] asm("_binary_default_map_xml_start");
extern const char default_map_end[]   asm("_binary_default_map_xml_end");

typedef enum { SIG_UNSIGNED = 0, SIG_SIGNED, SIG_FLOAT } sig_type_t;

typedef struct {
    uint16_t   start;
    uint8_t    len;
    bool       motorola;        /* big endian (DBC @0) */
    sig_type_t type;
    float      factor;
    float      offset;
    ch_id_t    ch;
    /* load time only (point into the XML buffer) */
    const char *name;
    const char *unit;
    uint8_t     decimals;
    bool        log;
    uint32_t    timeout_ms;
} map_sig_t;

typedef struct {
    uint32_t key;               /* id | 0x80000000 for 29-bit */
    uint16_t first;             /* index into s_sigs */
    uint16_t count;
} map_frame_t;

typedef struct {
    canmap_ui_binding_t b;
    uint16_t            sig;    /* load time: index into s_sigs */
} pending_ui_t;

/* Parser state, also the final map once committed. */
typedef struct {
    map_frame_t  *frames;
    size_t        nframes, cap_frames;
    map_sig_t    *sigs;
    size_t        nsigs, cap_sigs;
    pending_ui_t *uis;
    size_t        nuis, cap_uis;

    bool          in_root;
    int           cur_frame;    /* -1 = none */
    int           cur_sig;
    uint32_t      frame_timeout;
    uint32_t      bitrate;
    char          description[48];
    char          error[64];
} map_t;

static map_t          s_map;
static canmap_info_t  s_info;
static canmap_stats_t s_stats;
static canmap_ui_binding_t *s_ui;
static size_t         s_nui;
static QueueHandle_t  s_queue;
static portMUX_TYPE   s_lock = portMUX_INITIALIZER_UNLOCKED;

/* ---------------------------------------------------------------- parsing */

static bool grow(void **arr, size_t *cap, size_t n, size_t item)
{
    if (n < *cap) {
        return true;
    }
    size_t nc = *cap ? *cap * 2 : 32;
    void *p = realloc(*arr, nc * item);
    if (p == NULL) {
        return false;
    }
    *arr = p;
    *cap = nc;
    return true;
}

static bool parse_u32(const char *s, uint32_t *out)
{
    if (s == NULL || *s == '\0') {
        return false;
    }
    char *end;
    unsigned long v = strtoul(s, &end, 0);
    if (*end != '\0') {
        return false;
    }
    *out = (uint32_t)v;
    return true;
}

static bool parse_float(const char *s, float *out)
{
    if (s == NULL || *s == '\0') {
        return false;
    }
    char *end;
    float v = strtof(s, &end);
    if (*end != '\0') {
        return false;
    }
    *out = v;
    return true;
}

static bool fail(map_t *m, int line, const char *what)
{
    snprintf(m->error, sizeof(m->error), "line %d: %s", line, what);
    return false;
}

static bool on_frame(map_t *m, const xml_attr_t *a, int n, int line)
{
    uint32_t id, ext = 0, to = CONFIG_CANMAP_DEFAULT_TIMEOUT_MS;
    if (!parse_u32(xml_attr(a, n, "id"), &id)) {
        return fail(m, line, "frame without valid id");
    }
    const char *s = xml_attr(a, n, "ext");
    if (s && !parse_u32(s, &ext)) {
        return fail(m, line, "bad ext");
    }
    s = xml_attr(a, n, "timeout_ms");
    if (s && !parse_u32(s, &to)) {
        return fail(m, line, "bad timeout_ms");
    }
    if (id > (ext ? 0x1FFFFFFFu : 0x7FFu)) {
        return fail(m, line, "id out of range");
    }
    uint32_t key = id | (ext ? 0x80000000u : 0);
    for (size_t i = 0; i < m->nframes; i++) {
        if (m->frames[i].key == key) {
            return fail(m, line, "duplicate frame id");
        }
    }
    if (!grow((void **)&m->frames, &m->cap_frames, m->nframes, sizeof(map_frame_t))) {
        return fail(m, line, "out of memory");
    }
    m->frames[m->nframes] = (map_frame_t){ .key = key, .first = (uint16_t)m->nsigs, .count = 0 };
    m->cur_frame = (int)m->nframes++;
    m->frame_timeout = to;
    return true;
}

static bool on_signal(map_t *m, const xml_attr_t *a, int n, int line)
{
    if (m->cur_frame < 0) {
        return fail(m, line, "signal outside frame");
    }
    if (m->nsigs >= CH_MAX) {
        return fail(m, line, "too many signals");
    }
    map_sig_t sg = {
        .factor = 1.0f, .timeout_ms = m->frame_timeout, .ch = CH_NONE,
    };
    uint32_t v;
    sg.name = xml_attr(a, n, "name");
    if (sg.name == NULL || sg.name[0] == '\0') {
        return fail(m, line, "signal without name");
    }
    if (!parse_u32(xml_attr(a, n, "start"), &v) || v > 511) {
        return fail(m, line, "bad start");
    }
    sg.start = (uint16_t)v;
    if (!parse_u32(xml_attr(a, n, "len"), &v) || v < 1 || v > 64) {
        return fail(m, line, "bad len");
    }
    sg.len = (uint8_t)v;

    const char *s = xml_attr(a, n, "order");
    if (s == NULL || strcmp(s, "intel") == 0) {
        sg.motorola = false;
    } else if (strcmp(s, "motorola") == 0) {
        sg.motorola = true;
    } else {
        return fail(m, line, "order must be intel or motorola");
    }

    s = xml_attr(a, n, "type");
    if (s == NULL || strcmp(s, "unsigned") == 0) {
        sg.type = SIG_UNSIGNED;
    } else if (strcmp(s, "signed") == 0) {
        sg.type = SIG_SIGNED;
    } else if (strcmp(s, "float") == 0) {
        sg.type = SIG_FLOAT;
        if (sg.len != 32) {
            return fail(m, line, "float signals must be 32 bit");
        }
    } else {
        return fail(m, line, "type must be unsigned, signed or float");
    }

    s = xml_attr(a, n, "factor");
    if (s && !parse_float(s, &sg.factor)) {
        return fail(m, line, "bad factor");
    }
    s = xml_attr(a, n, "offset");
    if (s && !parse_float(s, &sg.offset)) {
        return fail(m, line, "bad offset");
    }
    sg.unit = xml_attr(a, n, "unit");
    s = xml_attr(a, n, "decimals");
    if (s) {
        if (!parse_u32(s, &v) || v > 9) {
            return fail(m, line, "bad decimals");
        }
        sg.decimals = (uint8_t)v;
    }
    s = xml_attr(a, n, "log");
    sg.log = (s == NULL) || strcmp(s, "0") != 0;
    s = xml_attr(a, n, "timeout_ms");
    if (s && !parse_u32(s, &sg.timeout_ms)) {
        return fail(m, line, "bad timeout_ms");
    }

    for (size_t i = 0; i < m->nsigs; i++) {
        if (strcmp(m->sigs[i].name, sg.name) == 0) {
            return fail(m, line, "duplicate signal name");
        }
    }
    if (!grow((void **)&m->sigs, &m->cap_sigs, m->nsigs, sizeof(map_sig_t))) {
        return fail(m, line, "out of memory");
    }
    m->sigs[m->nsigs] = sg;
    m->cur_sig = (int)m->nsigs++;
    m->frames[m->cur_frame].count++;
    return true;
}

/* The format is passed to snprintf with one float: allow exactly one
 * %[flags][width][.prec](f|e|g) conversion plus literal text ("%%" allowed). */
static bool format_ok(const char *s)
{
    int conversions = 0;
    for (const char *p = s; *p != '\0'; p++) {
        if (*p != '%') {
            continue;
        }
        p++;
        if (*p == '%') {
            continue;
        }
        while (*p != '\0' && strchr("-+ #0", *p)) {
            p++;
        }
        while (*p >= '0' && *p <= '9') {
            p++;
        }
        if (*p == '.') {
            p++;
            while (*p >= '0' && *p <= '9') {
                p++;
            }
        }
        if (*p == '\0' || strchr("fFeEgG", *p) == NULL) {
            return false;
        }
        conversions++;
    }
    return conversions == 1;
}

static bool on_ui(map_t *m, const xml_attr_t *a, int n, int line)
{
    if (m->cur_sig < 0) {
        return fail(m, line, "ui outside signal");
    }
    pending_ui_t u = { .sig = (uint16_t)m->cur_sig, .b = { .ch = CH_NONE, .scale = 1.0f } };
    const char *s = xml_attr(a, n, "var");
    if (s == NULL || s[0] == '\0' || strlen(s) >= CANMAP_VAR_NAME_MAX) {
        return fail(m, line, "ui needs var (max 31 chars)");
    }
    strcpy(u.b.var, s);

    s = xml_attr(a, n, "type");
    if (s == NULL || strcmp(s, "string") == 0) {
        u.b.type = CANMAP_UI_STRING;
    } else if (strcmp(s, "integer") == 0) {
        u.b.type = CANMAP_UI_INTEGER;
    } else if (strcmp(s, "float") == 0) {
        u.b.type = CANMAP_UI_FLOAT;
    } else if (strcmp(s, "boolean") == 0) {
        u.b.type = CANMAP_UI_BOOLEAN;
    } else {
        return fail(m, line, "ui type must be integer, float, string or boolean");
    }
    s = xml_attr(a, n, "scale");
    if (s && !parse_float(s, &u.b.scale)) {
        return fail(m, line, "bad scale");
    }
    s = xml_attr(a, n, "offset");
    if (s && !parse_float(s, &u.b.offset)) {
        return fail(m, line, "bad offset");
    }
    s = xml_attr(a, n, "step");
    if (s && (!parse_float(s, &u.b.step) || u.b.step < 0.0f)) {
        return fail(m, line, "bad step (>= 0)");
    }
    s = xml_attr(a, n, "period_ms");
    if (s) {
        char *end;
        unsigned long p = strtoul(s, &end, 10);
        if (*end != '\0' || p > 60000) {
            return fail(m, line, "bad period_ms (0..60000)");
        }
        u.b.period_ms = (uint16_t)p;
    }
    s = xml_attr(a, n, "format");
    if (s) {
        if (strlen(s) >= CANMAP_FORMAT_MAX || !format_ok(s)) {
            return fail(m, line, "format must hold one %f style conversion");
        }
        strcpy(u.b.format, s);
    } else {
        snprintf(u.b.format, sizeof(u.b.format), "%%.%uf", m->sigs[m->cur_sig].decimals);
    }
    if (!grow((void **)&m->uis, &m->cap_uis, m->nuis, sizeof(pending_ui_t))) {
        return fail(m, line, "out of memory");
    }
    m->uis[m->nuis++] = u;
    return true;
}

static bool xml_start(void *ctx, const char *tag, const xml_attr_t *a, int n, int line)
{
    map_t *m = ctx;
    if (strcmp(tag, "fpdash_map") == 0) {
        uint32_t ver = 0;
        if (!parse_u32(xml_attr(a, n, "version"), &ver) || ver != MAP_VERSION) {
            return fail(m, line, "unsupported map version");
        }
        const char *src = xml_attr(a, n, "source");
        snprintf(m->description, sizeof(m->description), "%s", src ? src : "");
        m->in_root = true;
        return true;
    }
    if (!m->in_root) {
        return fail(m, line, "root element must be fpdash_map");
    }
    if (strcmp(tag, "bus") == 0) {
        const char *s = xml_attr(a, n, "bitrate");
        if (s && !parse_u32(s, &m->bitrate)) {
            return fail(m, line, "bad bitrate");
        }
        return true;
    }
    if (strcmp(tag, "frame") == 0) {
        return on_frame(m, a, n, line);
    }
    if (strcmp(tag, "signal") == 0) {
        return on_signal(m, a, n, line);
    }
    if (strcmp(tag, "ui") == 0) {
        return on_ui(m, a, n, line);
    }
    return true;    /* unknown elements are ignored (forward compatible) */
}

static bool xml_end(void *ctx, const char *tag, int line)
{
    map_t *m = ctx;
    if (strcmp(tag, "frame") == 0) {
        m->cur_frame = -1;
    } else if (strcmp(tag, "signal") == 0) {
        m->cur_sig = -1;
    } else if (strcmp(tag, "fpdash_map") == 0) {
        m->in_root = false;
    }
    return true;
}

static void map_free(map_t *m)
{
    free(m->frames);
    free(m->sigs);
    free(m->uis);
    memset(m, 0, sizeof(*m));
}

/* Parses buf into m. On failure m is freed and m->error kept. */
static bool map_parse(char *buf, map_t *m)
{
    memset(m, 0, sizeof(*m));
    m->cur_frame = -1;
    m->cur_sig = -1;
    const xml_handler_t h = { .start = xml_start, .end = xml_end };
    int line = 0;
    bool ok = xml_parse(buf, &h, m, &line);
    if (ok && m->nframes == 0) {
        snprintf(m->error, sizeof(m->error), "no frames in map");
        ok = false;
    }
    if (!ok) {
        char err[64];
        if (m->error[0] == '\0') {
            snprintf(m->error, sizeof(m->error), "line %d: XML syntax error", line);
        }
        strcpy(err, m->error);
        map_free(m);
        strcpy(m->error, err);
    }
    return ok;
}

static int cmp_frame(const void *a, const void *b)
{
    uint32_t ka = ((const map_frame_t *)a)->key, kb = ((const map_frame_t *)b)->key;
    return ka < kb ? -1 : (ka > kb ? 1 : 0);
}

/* Registers channels and resolves UI bindings. Must be the last step (no rollback). */
static void map_commit(map_t *m)
{
    for (size_t i = 0; i < m->nsigs; i++) {
        map_sig_t *sg = &m->sigs[i];
        ch_info_t info = {
            .name = sg->name, .unit = sg->unit ? sg->unit : "",
            .decimals = sg->decimals, .log = sg->log, .timeout_ms = sg->timeout_ms,
        };
        sg->ch = ch_add(&info);
        sg->name = NULL;        /* the XML buffer is freed after loading */
        sg->unit = NULL;
    }
    s_ui = calloc(m->nuis ? m->nuis : 1, sizeof(*s_ui));
    s_nui = 0;
    for (size_t i = 0; s_ui != NULL && i < m->nuis; i++) {
        s_ui[s_nui] = m->uis[i].b;
        s_ui[s_nui].ch = m->sigs[m->uis[i].sig].ch;
        s_nui++;
    }
    free(m->uis);
    m->uis = NULL;
    m->nuis = 0;
    qsort(m->frames, m->nframes, sizeof(map_frame_t), cmp_frame);
}

/* ---------------------------------------------------------------- loading */

static char *read_file(const char *path, char *err, size_t err_len)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        snprintf(err, err_len, "not found");
        return NULL;
    }
    char *buf = NULL;
    long size = -1;
    if (fseek(f, 0, SEEK_END) == 0) {
        size = ftell(f);
        rewind(f);
    }
    if (size <= 0 || size > MAX_FILE_SIZE) {
        snprintf(err, err_len, "bad file size %ld", size);
    } else {
        buf = heap_caps_malloc(size + 1, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (buf == NULL) {
            buf = malloc(size + 1);
        }
        if (buf == NULL) {
            snprintf(err, err_len, "out of memory");
        } else if (fread(buf, 1, size, f) != (size_t)size) {
            snprintf(err, err_len, "read error");
            free(buf);
            buf = NULL;
        } else {
            buf[size] = '\0';
        }
    }
    fclose(f);
    return buf;
}

esp_err_t canmap_load(void)
{
    const char *path = CONFIG_CANMAP_SD_PATH;
    char err[64] = "";

    char *buf = read_file(path, err, sizeof(err));
    if (buf != NULL) {
        if (map_parse(buf, &s_map)) {
            s_info.from_sd = true;
            snprintf(s_info.source, sizeof(s_info.source), "%s", path);
        } else {
            snprintf(err, sizeof(err), "%s", s_map.error);
            ESP_LOGE(TAG, "%s rejected (%s), using the built-in map", path, err);
            snprintf(s_info.error, sizeof(s_info.error), "%s", err);
            free(buf);
            buf = NULL;
        }
    } else {
        ESP_LOGI(TAG, "%s: %s, using the built-in map", path, err);
    }

    if (buf == NULL) {
        size_t len = default_map_end - default_map_start;
        buf = malloc(len + 1);
        if (buf == NULL) {
            return ESP_ERR_NO_MEM;
        }
        memcpy(buf, default_map_start, len);
        buf[len] = '\0';
        if (!map_parse(buf, &s_map)) {
            ESP_LOGE(TAG, "built-in map invalid: %s", s_map.error);
            free(buf);
            return ESP_ERR_INVALID_STATE;
        }
        s_info.from_sd = false;
        snprintf(s_info.source, sizeof(s_info.source), "built-in");
    }

    map_commit(&s_map);
    free(buf);

    s_info.bitrate = s_map.bitrate;
    s_info.frames = (uint16_t)s_map.nframes;
    s_info.signals = (uint16_t)s_map.nsigs;
    s_info.ui_bindings = (uint16_t)s_nui;
    snprintf(s_info.description, sizeof(s_info.description), "%s", s_map.description);
    ESP_LOGI(TAG, "map %s (%s): %u frames, %u signals, %u UI bindings, bitrate %" PRIu32,
             s_info.source, s_info.description, s_info.frames, s_info.signals, s_info.ui_bindings,
             s_info.bitrate);
    return ESP_OK;
}

/* ---------------------------------------------------------------- decoding */

static bool extract(const can_frame_t *f, const map_sig_t *sg, uint64_t *out)
{
    uint64_t raw = 0;
    if (!sg->motorola) {
        if (sg->start + sg->len > f->dlc * 8u) {
            return false;
        }
        uint64_t all = 0;
        for (int i = f->dlc - 1; i >= 0; i--) {
            all = (all << 8) | f->data[i];
        }
        raw = all >> sg->start;
        if (sg->len < 64) {
            raw &= (1ULL << sg->len) - 1;
        }
    } else {
        /* DBC Motorola: start is the MSB, bits numbered LSB first inside a byte;
         * walk towards the LSB, wrapping to the next byte's bit 7. */
        unsigned pos = sg->start;
        for (unsigned i = 0; i < sg->len; i++) {
            unsigned byte = pos / 8;
            if (byte >= f->dlc) {
                return false;
            }
            raw = (raw << 1) | ((f->data[byte] >> (pos % 8)) & 1u);
            pos = (pos % 8 == 0) ? pos + 15 : pos - 1;
        }
    }
    *out = raw;
    return true;
}

static float to_value(const map_sig_t *sg, uint64_t raw)
{
    double v;
    switch (sg->type) {
    case SIG_SIGNED:
        if (sg->len < 64 && (raw >> (sg->len - 1)) & 1u) {
            raw |= ~0ULL << sg->len;
        }
        v = (double)(int64_t)raw;
        break;
    case SIG_FLOAT: {
        uint32_t u = (uint32_t)raw;
        float fv;
        memcpy(&fv, &u, sizeof(fv));
        v = fv;
        break;
    }
    default:
        v = (double)raw;
        break;
    }
    return (float)(v * sg->factor + sg->offset);
}

static const map_frame_t *find_frame(uint32_t key)
{
    size_t lo = 0, hi = s_map.nframes;
    while (lo < hi) {
        size_t mid = (lo + hi) / 2;
        uint32_t k = s_map.frames[mid].key;
        if (k == key) {
            return &s_map.frames[mid];
        }
        if (k < key) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }
    return NULL;
}

static void decode_task(void *arg)
{
    can_frame_t f;
    while (1) {
        if (xQueueReceive(s_queue, &f, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        const map_frame_t *fr = find_frame(f.id | (f.ext ? 0x80000000u : 0));
        portENTER_CRITICAL(&s_lock);
        if (fr) {
            s_stats.frames_decoded++;
        } else {
            s_stats.frames_unknown++;
        }
        portEXIT_CRITICAL(&s_lock);
        if (fr == NULL) {
            continue;
        }
        for (uint16_t i = 0; i < fr->count; i++) {
            const map_sig_t *sg = &s_map.sigs[fr->first + i];
            uint64_t raw;
            if (sg->ch != CH_NONE && extract(&f, sg, &raw)) {
                ch_set(sg->ch, to_value(sg, raw), f.ts_ms);
            }
        }
    }
}

esp_err_t canmap_start(void)
{
    if (s_map.nframes == 0) {
        return ESP_ERR_INVALID_STATE;
    }
    s_queue = xQueueCreate(DECODE_QUEUE_LEN, sizeof(can_frame_t));
    if (s_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }
    esp_err_t err = can_if_subscribe(0, 0x1FFFFFFF, s_queue);
    if (err != ESP_OK) {
        return err;
    }
    if (xTaskCreatePinnedToCore(decode_task, "canmap", DECODE_TASK_STACK, NULL, DECODE_TASK_PRIO, NULL, 0) != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

/* ---------------------------------------------------------------- info */

void canmap_get_info(canmap_info_t *out)
{
    *out = s_info;
}

void canmap_get_stats(canmap_stats_t *out)
{
    can_if_stats_t cs;
    can_if_get_stats(&cs);
    portENTER_CRITICAL(&s_lock);
    *out = s_stats;
    portEXIT_CRITICAL(&s_lock);
    out->frames_dropped = cs.rx_dropped;
}

const canmap_ui_binding_t *canmap_ui_bindings(size_t *count)
{
    *count = s_nui;
    return s_ui;
}
