#include "../../include/memConfig.h"

    .section .boot
    .globl _start
_start:
    la sp, kernelStack
    call main

    .section .stack
    .globl kernelStack
    .align 12
kernelStack:
    .space KERNEL_STACK_SIZE
    .globl kernelStackTop
kernelStackTop:
    
    .section .pgdir
    .align 12
    .globl kernelPageDirectory
kernelPageDirectory:
    .space PAGE_SIZE

