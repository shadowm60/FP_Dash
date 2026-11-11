#include "mini_xml.h"

#include <string.h>
#include <ctype.h>

typedef struct {
    char *p;
    int   line;
} cursor_t;

static void skip_ws(cursor_t *c)
{
    while (*c->p != '\0' && isspace((unsigned char)*c->p)) {
        if (*c->p == '\n') {
            c->line++;
        }
        c->p++;
    }
}

/* Advance past `end` (counting lines). false if not found. */
static bool skip_past(cursor_t *c, const char *end)
{
    size_t n = strlen(end);
    while (*c->p != '\0') {
        if (strncmp(c->p, end, n) == 0) {
            c->p += n;
            return true;
        }
        if (*c->p == '\n') {
            c->line++;
        }
        c->p++;
    }
    return false;
}

static bool is_name_char(char ch)
{
    return isalnum((unsigned char)ch) || ch == '_' || ch == '-' || ch == '.' || ch == ':';
}

/* Copies a name into out (truncated to XML_MAX_NAME - 1). false if empty. */
static bool read_name(cursor_t *c, char *out)
{
    size_t n = 0;
    while (is_name_char(*c->p)) {
        if (n < XML_MAX_NAME - 1) {
            out[n++] = *c->p;
        }
        c->p++;
    }
    out[n] = '\0';
    return n > 0;
}

/* Unescape the predefined entities in place. */
static void unescape(char *s)
{
    static const struct { const char *ent; char ch; } ents[] = {
        { "&amp;", '&' }, { "&lt;", '<' }, { "&gt;", '>' }, { "&quot;", '"' }, { "&apos;", '\'' },
    };
    char *w = s;
    for (char *r = s; *r != '\0';) {
        if (*r == '&') {
            bool done = false;
            for (size_t i = 0; i < sizeof(ents) / sizeof(ents[0]); i++) {
                size_t n = strlen(ents[i].ent);
                if (strncmp(r, ents[i].ent, n) == 0) {
                    *w++ = ents[i].ch;
                    r += n;
                    done = true;
                    break;
                }
            }
            if (done) {
                continue;
            }
        }
        *w++ = *r++;
    }
    *w = '\0';
}

bool xml_parse(char *buf, const xml_handler_t *h, void *ctx, int *err_line)
{
    cursor_t c = { .p = buf, .line = 1 };
    char tag[XML_MAX_NAME];
    xml_attr_t attrs[XML_MAX_ATTRS];
    int depth = 0;

#define FAIL() do { if (err_line) { *err_line = c.line; } return false; } while (0)

    while (*c.p != '\0') {
        if (*c.p != '<') {
            if (*c.p == '\n') {
                c.line++;
            }
            c.p++;
            continue;
        }
        if (strncmp(c.p, "<!--", 4) == 0) {
            if (!skip_past(&c, "-->")) FAIL();
            continue;
        }
        if (strncmp(c.p, "<![CDATA[", 9) == 0) {
            if (!skip_past(&c, "]]>")) FAIL();
            continue;
        }
        if (c.p[1] == '?') {
            if (!skip_past(&c, "?>")) FAIL();
            continue;
        }
        if (c.p[1] == '!') {
            if (!skip_past(&c, ">")) FAIL();
            continue;
        }

        if (c.p[1] == '/') {                        /* end tag */
            c.p += 2;
            if (!read_name(&c, tag)) FAIL();
            skip_ws(&c);
            if (*c.p != '>') FAIL();
            c.p++;
            if (--depth < 0) FAIL();
            if (h->end && !h->end(ctx, tag, c.line)) FAIL();
            continue;
        }

        /* start tag */
        int start_line = c.line;
        c.p++;
        if (!read_name(&c, tag)) FAIL();
        int nattrs = 0;
        bool self_close = false;
        while (1) {
            skip_ws(&c);
            if (*c.p == '>') {
                c.p++;
                break;
            }
            if (c.p[0] == '/' && c.p[1] == '>') {
                c.p += 2;
                self_close = true;
                break;
            }
            if (nattrs >= XML_MAX_ATTRS) FAIL();
            xml_attr_t *a = &attrs[nattrs];
            if (!read_name(&c, a->name)) FAIL();
            skip_ws(&c);
            if (*c.p != '=') FAIL();
            c.p++;
            skip_ws(&c);
            char q = *c.p;
            if (q != '"' && q != '\'') FAIL();
            c.p++;
            char *val = c.p;
            while (*c.p != '\0' && *c.p != q) {
                if (*c.p == '\n') {
                    c.line++;
                }
                c.p++;
            }
            if (*c.p != q) FAIL();
            *c.p++ = '\0';                          /* closing quote becomes the terminator */
            unescape(val);
            a->value = val;
            nattrs++;
        }

        if (h->start && !h->start(ctx, tag, attrs, nattrs, start_line)) {
            c.line = start_line;
            FAIL();
        }
        if (self_close) {
            if (h->end && !h->end(ctx, tag, start_line)) FAIL();
        } else {
            depth++;
        }
    }
    if (depth != 0) FAIL();
    return true;
#undef FAIL
}

const char *xml_attr(const xml_attr_t *attrs, int nattrs, const char *name)
{
    for (int i = 0; i < nattrs; i++) {
        if (strcmp(attrs[i].name, name) == 0) {
            return attrs[i].value;
        }
    }
    return NULL;
}
