#ifndef _USER_SYSCALL_H_
#define _USER_SYSCALL_H_

#include "../../include/type.h"

inline u_longlong inline msyscall(long n, u_longlong _a0, u_longlong _a1, u_longlong _a2, u_longlong
		_a3, u_longlong _a4, u_longlong _a5) {
	register u_longlong a0 asm("a0") = _a0;
	register u_longlong a1 asm("a1") = _a1;
	register u_longlong a2 asm("a2") = _a2;
	register u_longlong a3 asm("a3") = _a3;
	register u_longlong a4 asm("a4") = _a4;
	register u_longlong a5 asm("a5") = _a5;
	register long syscall_id asm("a7") = n;
	asm volatile ("ecall" : "+r"(a0) : "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"
			(a5), "r"(syscall_id));
	return a0;
}

#endif