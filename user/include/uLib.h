#ifndef __ULIB_H
#define __ULIB_H

#include "../../include/type.h"

void *memmove(void*, const void*, int);
int strcmp(const char*, const char*);
u_int strlen(const char*);
void* memset(void*, int, u_int);
char* strcpy(char* s, const char* t);
void user_bcopy(void *src, void *dst, u_int len);
void user_bzero(void *start, u_int len);
#endif