#include "../../include/assembly.h"
	.section .trampoline
    .globl trampoline
    .align 12
trampoline:
    .align 4
    .globl userVector
userVector:
    csrrw a0, sscratch, a0 // read trapframe addr, sscratch->a0, a0->sscratch
    sd ra, REG_RA(a0)
    sd sp, REG_SP(a0)
    sd gp, REG_GP(a0)
    sd tp, REG_TP(a0)
    sd t0, REG_T0(a0)
    sd t1, REG_T1(a0)
    sd t2, REG_T2(a0)
    sd s0, REG_S0(a0)
    sd s1, REG_S1(a0)
    sd a1, REG_A1(a0)
    sd a2, REG_A2(a0)
    sd a3, REG_A3(a0)
    sd a4, REG_A4(a0)
    sd a5, REG_A5(a0)
    sd a6, REG_A6(a0)
    sd a7, REG_A7(a0)
    sd s2, REG_S2(a0)
    sd s3, REG_S3(a0)
    sd s4, REG_S4(a0)
    sd s5, REG_S5(a0)
    sd s6, REG_S6(a0)
    sd s7, REG_S7(a0)
    sd s8, REG_S8(a0)
    sd s9, REG_S9(a0)
    sd s10, REG_S10(a0)
    sd s11, REG_S11(a0)
    sd t3, REG_T3(a0)
    sd t4, REG_T4(a0)
    sd t5, REG_T5(a0)
    sd t6, REG_T6(a0)
    csrr t0, sscratch // t0 needn't be protected, read trapframe addr
    sd t0, REG_A0(a0)
    ld sp, KERNEL_SP(a0)
    ld t0, STVEC(a0)
    csrr t1, sepc
    sd t1, SEPC(a0)
    jr t0

.globl userReturn
userReturn:
    csrw satp, a1
    sfence.vma
    ld t0, REG_A0(a0)
    csrw sscratch, t0
    ld t1, SEPC(a0)
    csrw sepc, t1
    ld ra, REG_RA(a0)
    ld sp, REG_SP(a0)
    ld gp, REG_GP(a0)
    ld tp, REG_TP(a0)
    ld t0, REG_T0(a0)
    ld t1, REG_T1(a0)
    ld t2, REG_T2(a0)
    ld s0, REG_S0(a0)
    ld s1, REG_S1(a0)
    ld a1, REG_A1(a0)
    ld a2, REG_A2(a0)
    ld a3, REG_A3(a0)
    ld a4, REG_A4(a0)
    ld a5, REG_A5(a0)
    ld a6, REG_A6(a0)
    ld a7, REG_A7(a0)
    ld s2, REG_S2(a0)
    ld s3, REG_S3(a0)
    ld s4, REG_S4(a0)
    ld s5, REG_S5(a0)
    ld s6, REG_S6(a0)
    ld s7, REG_S7(a0)
    ld s8, REG_S8(a0)
    ld s9, REG_S9(a0)
    ld s10, REG_S10(a0)
    ld s11, REG_S11(a0)
    ld t3, REG_T3(a0)
    ld t4, REG_T4(a0)
    ld t5, REG_T5(a0)
    ld t6, REG_T6(a0)
    csrrw a0, sscratch, a0
    sret
