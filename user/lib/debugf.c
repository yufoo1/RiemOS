#include "../../include/driver.h"
#include "../include/debugf.h"

void debugf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
}

void _user_panic(const char *file, int line, const char *fmt, ...) {
    debugf("panic at %s:%d: ", file, line);
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
    debugf("\n");
    while(1);
}