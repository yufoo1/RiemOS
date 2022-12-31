#include "../include/uLib.h"

char* strcpy(char* s, const char* t) {
    char* os;

    os = s;
    while ((*s++ = *t++) != 0)
        ;
    return os;
}

int strcmp(const char* p, const char* q) {
    while (*p && *p == *q)
        p++, q++;
    return (uchar)*p - (uchar)*q;
}

u_int strlen(const char* s) {
    int n;

    for (n = 0; s[n]; n++)
        ;
    return n;
}

void* memset(void* dst, int c, u_int n) {
    char* cdst = (char*)dst;
    int i;
    for (i = 0; i < n; i++) {
        cdst[i] = c;
    }
    return dst;
}

char* strchr(const char* s, char c) {
    for (; *s; s++)
        if (*s == c)
            return (char*)s;
    return 0;
}

int atoi(const char* s) {
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9')
        n = n * 10 + *s++ - '0';
    return n;
}

void* memmove(void* vdst, const void* vsrc, int n) {
    char* dst;
    const char* src;

    dst = vdst;
    src = vsrc;
    if (src > dst) {
        while (n-- > 0)
            *dst++ = *src++;
    } else {
        dst += n;
        src += n;
        while (n-- > 0)
            *--dst = *--src;
    }
    return vdst;
}

int memcmp(const void* s1, const void* s2, u_int n) {
    const char *p1 = s1, *p2 = s2;
    while (n-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

void* memcpy(void* dst, const void* src, u_int n) {
    return memmove(dst, src, n);
}

void user_bcopy(void *src, void *dst, u_int len) {
    void *max;
    max = dst + len;
    while (dst + 3 < max) {
        *(int *) dst = *(int *) src;
        dst += 4;
        src += 4;
    }
    while (dst < max) {
        *(char *) dst = *(char *) src;
        dst += 1;
        src += 1;
    }
}

void user_bzero(void *start, u_int len) {
    void *max;
    max = start + len;
    while (start + 3 < max)
    {
        *(int *)start = 0;
        start += 4;
    }
    while (start < max)
    {
        *(char *)start++ = 0;
    }
}