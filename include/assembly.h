//
// Created by yufu on 2022/7/4.
//

#ifndef FIBOS_ASSEMBLY_H
#define FIBOS_ASSEMBLY_H
#define KERNEL_SATP 0
#define KERNEL_SP (KERNEL_SATP + 8)
#define STVEC (KERNEL_SP + 8)
#define SEPC (STVEC + 8)
#define REG_RA (SEPC + 8)
#define REG_SP (REG_RA + 8)
#define REG_GP (REG_SP + 8)
#define REG_TP (REG_GP + 8)
#define REG_T0 (REG_TP + 8)
#define REG_T1 (REG_T0 + 8)
#define REG_T2 (REG_T1 + 8)
#define REG_S0 (REG_T2 + 8)
#define REG_S1 (REG_S0 + 8)
#define REG_A0 (REG_S1 + 8)
#define REG_A1 (REG_A0 + 8)
#define REG_A2 (REG_A1 + 8)
#define REG_A3 (REG_A2 + 8)
#define REG_A4 (REG_A3 + 8)
#define REG_A5 (REG_A4 + 8)
#define REG_A6 (REG_A5 + 8)
#define REG_A7 (REG_A6 + 8)
#define REG_S2 (REG_A7 + 8)
#define REG_S3 (REG_S2 + 8)
#define REG_S4 (REG_S3 + 8)
#define REG_S5 (REG_S4 + 8)
#define REG_S6 (REG_S5 + 8)
#define REG_S7 (REG_S6 + 8)
#define REG_S8 (REG_S7 + 8)
#define REG_S9 (REG_S8 + 8)
#define REG_S10 (REG_S9 + 8)
#define REG_S11 (REG_S10 + 8)
#define REG_T3 (REG_S11 + 8)
#define REG_T4 (REG_T3 + 8)
#define REG_T5 (REG_T4 + 8)
#define REG_T6 (REG_T5 + 8)

#define SCAUSE_INTERRUPT (1UL << 63)
#define SCAUSE_EXCEPTION_CODE ((1UL << 63) - 1)
#define SCAUSE_USER_SOFTWARE 0
#define SCAUSE_SUPERVISOR_SOFRWARE 1
#define SCAUSE_USER_TIMER 4
#define SCAUSE_SUPERVISOR_TIMER 5
#define SCAUSE_USER_EXTERNAL 8
#define SCAUSE_SUPERVISOR_EXTERNAL 9

#define SCAUSE_STORE_ACCESS_FAULT 7
#define SCAUSE_ENVIRONMENT_CALL 8
#define SCAUSE_LOAD_PAGE_FAULT 13
#define SCAUSE_STORE_PAGE_FAULT 15
#endif //FIBOS_ASSEMBLY_H
