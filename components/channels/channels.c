#include "channels.h"

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"

static const char *TAG = "channels";

typedef struct {
    float    value;
    uint32_t ts_ms;
    bool     seen;
} slot_t;

static ch_info_t    *s_info;
static slot_t       *s_slot;
static size_t        s_count;
static size_t        s_cap;
static volatile uint32_t s_generation;
static portMUX_TYPE  s_lock = portMUX_INITIALIZER_UNLOCKED;

ch_id_t ch_add(const ch_info_t *info)
{
    if (info == NULL || info->name == NULL || s_count >= CH_MAX) {
        return CH_NONE;
    }
    if (ch_find(info->name) != CH_NONE) {
        ESP_LOGW(TAG, "duplicate channel '%s'", info->name);
        return CH_NONE;
    }
    if (s_count == s_cap) {
        size_t cap = s_cap ? s_cap * 2 : 64;
        ch_info_t *ni = realloc(s_info, cap * sizeof(*ni));
        if (ni == NULL) {
            return CH_NONE;
        }
        s_info = ni;
        slot_t *ns = realloc(s_slot, cap * sizeof(*ns));
        if (ns == NULL) {
            return CH_NONE;
        }
        s_slot = ns;
        s_cap = cap;
    }
    ch_info_t *dst = &s_info[s_count];
    *dst = *info;
    dst->name = strdup(info->name);
    dst->unit = strdup(info->unit ? info->unit : "");
    if (dst->name == NULL || dst->unit == NULL) {
        return CH_NONE;
    }
    memset(&s_slot[s_count], 0, sizeof(slot_t));
    return (ch_id_t)s_count++;
}

ch_id_t ch_find(const char *name)
{
    for (size_t i = 0; i < s_count; i++) {
        if (strcmp(s_info[i].name, name) == 0) {
            return (ch_id_t)i;
        }
    }
    return CH_NONE;
}

size_t ch_count(void)
{
    return s_count;
}

const ch_info_t *ch_info(ch_id_t id)
{
    return id < s_count ? &s_info[id] : NULL;
}

void ch_set(ch_id_t id, float value, uint32_t ts_ms)
{
    if (id >= s_count) {
        return;
    }
    portENTER_CRITICAL(&s_lock);
    s_slot[id].value = value;
    s_slot[id].ts_ms = ts_ms;
    s_slot[id].seen = true;
    s_generation++;
    portEXIT_CRITICAL(&s_lock);
}

bool ch_get(ch_id_t id, uint32_t now_ms, ch_sample_t *out)
{
    if (id >= s_count) {
        *out = (ch_sample_t){ 0 };
        return false;
    }
    slot_t s;
    portENTER_CRITICAL(&s_lock);
    s = s_slot[id];
    portEXIT_CRITICAL(&s_lock);

    uint32_t to = s_info[id].timeout_ms;
    out->value = s.value;
    out->ts_ms = s.ts_ms;
    out->valid = s.seen && (to == 0 || (uint32_t)(now_ms - s.ts_ms) <= to);
    return out->valid;
}

uint32_t ch_generation(void)
{
    return s_generation;
}
