#ifndef HAVE_LOGGER_H
#define HAVE_LOGGER_H

#include <inttypes.h>

#define LOG_DEBUG  0
#define LOG_INFO   1
#define LOG_ERROR  2

#define LOG_LEVEL LOG_DEBUG

typedef void (*LOG_cb_t)(uint8_t lvl, char *fmt, ...);
extern LOG_cb_t LOG_cb;

#define LOG_printf(lvl, fmt, ...) do { if (lvl >= LOG_LEVEL) LOG_cb(lvl, fmt, __VA_ARGS__); } while (0)

void LOG_init (LOG_cb_t cb);

#endif
