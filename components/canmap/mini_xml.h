/*
 * mini_xml - small in-place XML reader for the signal map.
 *
 * Supports what the map needs: elements, attributes (single or double quoted,
 * the five predefined entities), self-closing tags, comments, <?...?> and
 * <!...> declarations. Text content and CDATA are ignored. No namespaces, no
 * validation. The buffer is modified (attribute values are terminated and
 * unescaped in place) and must stay alive while the callbacks run.
 */
#ifndef MINI_XML_H
#define MINI_XML_H

#include <stdbool.h>

#define XML_MAX_ATTRS     24
#define XML_MAX_NAME      32

typedef struct {
    char        name[XML_MAX_NAME];
    const char *value;          /* points into the buffer */
} xml_attr_t;

typedef struct {
    /* Return false to stop parsing (xml_parse then returns false). */
    bool (*start)(void *ctx, const char *tag, const xml_attr_t *attrs, int nattrs, int line);
    bool (*end)(void *ctx, const char *tag, int line);
} xml_handler_t;

/* Returns true when the whole document was read. On failure *err_line is the
 * line of the syntax error (or of the element a callback rejected). */
bool xml_parse(char *buf, const xml_handler_t *h, void *ctx, int *err_line);

/* Attribute lookup helper: NULL if not present. */
const char *xml_attr(const xml_attr_t *attrs, int nattrs, const char *name);

#endif /* MINI_XML_H */
