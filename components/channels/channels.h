/*
 * channels - thread-safe store of the latest value of every decoded signal.
 *
 * The table is built once at boot by canmap (one channel per signal in the
 * map) and is fixed afterwards. Producers (the CAN decoder) call ch_set();
 * consumers (UI bridge, logger) read with ch_get(). Every sample carries a
 * timestamp: a channel not refreshed within its timeout reads as invalid,
 * which the UI shows as "--" and the logger as an empty cell.
 *
 * No dependency on CAN, the map format or LVGL.
 */
#ifndef CHANNELS_H
#define CHANNELS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t ch_id_t;
#define CH_NONE     ((ch_id_t)0xFFFF)
#define CH_MAX      1024

typedef struct {
    const char *name;           /* unique, e.g. "RPM" (copied by ch_add) */
    const char *unit;           /* "" if none (copied) */
    uint8_t     decimals;       /* for text output (log, UI strings) */
    bool        log;            /* column in the parameter log */
    uint32_t    timeout_ms;     /* older samples are invalid; 0 = never stale */
} ch_info_t;

typedef struct {
    float    value;
    uint32_t ts_ms;             /* time of last update (ms since boot) */
    bool     valid;             /* updated at least once and not timed out */
} ch_sample_t;

/* Registration: single threaded, before any producer or consumer runs. */
ch_id_t ch_add(const ch_info_t *info);
ch_id_t ch_find(const char *name);      /* case sensitive, CH_NONE if unknown */
size_t  ch_count(void);
const ch_info_t *ch_info(ch_id_t id);

/* Runtime: any task. */
void ch_set(ch_id_t id, float value, uint32_t ts_ms);
bool ch_get(ch_id_t id, uint32_t now_ms, ch_sample_t *out);

/* Incremented on every ch_set(): cheap "did anything arrive?" check. */
uint32_t ch_generation(void);

#ifdef __cplusplus
}
#endif

#endif /* CHANNELS_H */
