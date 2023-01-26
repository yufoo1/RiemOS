#include "../../include/riscv.h"
#include "../../include/driver.h"
#include "../../include/kclock.h"
#include "../../include/trap.h"
#include "../../include/process.h"
#include "../../include/memory.h"
#include "../../include/assembly.h"
#include "../../include/syscallLib.h"

extern Process *curProcess;

void trap_init() {
    printf("Trap init start...\n");
    w_stvec((u_longlong)kernelVec);
//    w_sstatus(SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_UIE | SSTATUS_UPIE);
    w_sstatus(SSTATUS_SIE | SSTATUS_SPIE | SSTATUS_SUM);
    w_sie(SIE_SEIE | SIE_SSIE | SIE_STIE);
    setNextTimeInterrupt();
    printf("Trap init finish!\n");
}


void kernelTrap() {
    u_longlong sepc = r_sepc();
    u_longlong sstatus = r_sstatus();
    u_longlong scause = r_scause();
    if ((scause & SCAUSE_INTERRUPT) && ((scause & SCAUSE_EXCEPTION_CODE) == SCAUSE_SUPERVISOR_TIMER)) {
        setNextTimeInterrupt();
        process_yield();
    }
    w_sepc(sepc);
    w_sstatus(sstatus);
}

void userTrap() {
    u_longlong sstatus = r_sstatus();
    u_longlong scause = r_scause();
    if (sstatus & SSTATUS_SPP) {
        panic("usertrap: not from user mode\n");
    }
    w_stvec((u_longlong) kernelVec);
    Trapframe* trapframe = getTrapFrame();
    if (scause & SCAUSE_INTERRUPT) {
        if ((scause & SCAUSE_EXCEPTION_CODE) == SCAUSE_SUPERVISOR_TIMER) {
            setNextTimeInterrupt();
        }
        process_yield();
    } else {
        u_longlong *pte = 0;
        u_longlong pa;
        switch (scause & SCAUSE_EXCEPTION_CODE) {
        case SCAUSE_ENVIRONMENT_CALL:
            trapframe->sepc += 4;
            syscallVector[trapframe->a7]();
            break;
        case SCAUSE_LOAD_PAGE_FAULT:
        case SCAUSE_STORE_PAGE_FAULT:
            pa = page_lookup(curProcess->pgdir, r_stval(), &pte);
            if (pa == 0) {
                pageout(curProcess->pgdir, r_stval());
            } else if (*pte & PTE_C) {
                cowHandler(curProcess->pgdir, r_stval());
            } else {
                panic("unknown");
            }
            break;
        }
    }
    userTrapReturn();
}

void userTrapReturn() {
    extern char trampoline[];
    w_stvec(TRAMPOLINE_BASE + ((u_longlong)userVector - (u_longlong)trampoline)); // 指向 userVector 函数的虚拟地址
    Trapframe* trapframe = getTrapFrame();
    trapframe->kernelSp = getProcessTopSp(curProcess);
    trapframe->stvec = (u_longlong)userTrap;
    u_longlong sstatus = r_sstatus();
    sstatus &= ~SSTATUS_SPP;
    sstatus |= SSTATUS_SPIE;
    w_sstatus(sstatus);
    u_longlong satp = MAKE_SATP(curProcess->pgdir);
    u_longlong fn = TRAMPOLINE_BASE + ((u_longlong)userReturn - (u_longlong)trampoline); // 指向 userReturn 函数的虚拟地址
    ((void(*)(u_longlong, u_longlong))fn)((u_longlong)trapframe, satp);
}