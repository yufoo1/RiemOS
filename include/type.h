#ifndef _TYPE_H_
#define _TYPE_H_

typedef unsigned __attribute__((__mode__(QI))) u_char;
typedef unsigned __attribute__((__mode__(HI))) u_short;
typedef unsigned __attribute__((__mode__(SI))) u_int;
typedef unsigned __attribute__((__mode__(DI))) u_longlong;
typedef int __attribute__((__mode__(QI))) i_char;
typedef int __attribute__((__mode__(HI))) i_short;
typedef int __attribute__((__mode__(SI))) i_int;
typedef int __attribute__((__mode__(DI))) i_longlong;

typedef u_char bool;
#define true 1
#define false 0

typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap) __builtin_va_end(ap)
#define va_arg(ap, type) __builtin_va_arg(ap, type)

typedef unsigned char uchar;
typedef unsigned short wchar;

typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

#endif