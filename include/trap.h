#ifndef _TRAP_H_
#define _TRAP_H_

#include <memConfig.h>
#include <riscv.h>
#include <process.h>

void trap_init();
void kernelVec();
void userVector();
void userReturn();
void userTrapReturn();

#endif