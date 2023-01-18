#include "../../include/driver.h"
#include "../../include/process.h"
#include "../../include/type.h"
#include "../../include/memory.h"
#include "../../include/syscallLib.h"
#include "../../include/error.h"
#include "../../include/sd.h"

void (*syscallVector[])(void) = {
     [SYSCALL_PUTCHAR]                     syscallPutchar,
     [SYSCALL_GETCHAR]                     syscallGetchar,
     [SYSCALL_GET_PROCESS_ID]              syscallGetProcessId,
     [SYSCALL_SCHED_YIELD]                 syscallYield,
     [SYSCALL_CLONE]                       syscallFork,
     [SYSCALL_PUT_STRING]                  syscallPutString,
     [SYSCALL_GET_PARENT_PID]              syscallGetParentProcessId,
     [SYSCALL_EXIT]                        syscallExit,
     [SYSCALL_IPC_CAN_SEND]               syscallIpcCanSend,
     [SYSCALL_IPC_RECV]                    syscallIpcRecv,
     [SYSCALL_SD_READ]                    syscallSdRead,
     [SYSCALL_SD_WRITE]                   syscallSdWrite,
     [SYSCALL_MAP_MEMORY_FROM_TO]         syscallMapMemoryFromTo,
     [SYSCALL_UNMAP_MEMORY_FROM]          syscallUnmapMemoryFrom,
     [SYSCALL_MEMORY_ALLOC]               syscallMemoryAlloc,
     [SYSCALL_CHECK_VA_IS_MAP]            syscallCheckVaIsMap,
     [SYSCALL_GET_PROCESS_PCB]            syscallGetProcessPcb,
     [SYSCALL_GET_PHYSICAL_PAGE_REF]      syscallGetPhysicalPageRef,
     [SYSCALL_GET_PHYSICAL_PAGE_ADDR]     syscallGetPhysicalPageAddr
};
extern Process *curProcess;

void syscallPutchar() {
     Trapframe* trapframe = getTrapFrame();
     _putchar(trapframe->a0);
}

void syscallGetchar() {
    Trapframe* trapframe = getTrapFrame();
    trapframe->a0 = _getchar();
}

void syscallPutString() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong va = trapframe->a0;
    int len = trapframe->a1;
    u_longlong* pte;
    char* start = page_lookup(curProcess->pgdir, va, &pte) + (va & 0xfff);
    while (len--) {
        _putchar(*start);
        start++;
    }
}

void syscallGetProcessId() {
    Trapframe* trapframe = getTrapFrame();
    trapframe->a0 = curProcess->id;
}

void syscallGetParentProcessId() {
    Trapframe* trapframe = getTrapFrame();
    trapframe->a0 = curProcess->parentId;
}

void syscallYield() {
    process_yield();
}

void syscallFork() {
    process_fork();
}

void syscallExit() {
    struct Process* process;
    pid2Process(0, &process, 1);
    process_destory(process);
}

void syscallIpcRecv() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong dstVa = trapframe->a0;
    if (dstVa >= PHYSICAL_MEMORY_TOP) {
        return;
    }
    curProcess->env_ipc_recving = 1;
    curProcess->env_ipc_dstva = dstVa;
    curProcess->state = BLOCKED;
    syscallYield();
}

void syscallIpcCanSend() {
    Trapframe* trapframe = getTrapFrame();
    Process *process;
    PhysicalPage *page;
    u_longlong srcVa = trapframe->a2;
    u_longlong pid = trapframe->a0;
    u_longlong value = trapframe->a1;
    u_longlong perm = trapframe->a3;
    int r = pid2Process(pid, &process, 0);
    if (r < 0) {
        trapframe->a0 = r;
        return;
    }
    if (process->env_ipc_recving == 0) {
        trapframe->a0 = -1;
        return;
    }
    process->env_ipc_value = value;
    printf("process id: %x get value: %x\n", process->id, process->env_ipc_value);
    process->env_ipc_from = curProcess->id;
    process->env_ipc_perm = perm;
    process->env_ipc_recving = 0;
    process->state = RUNNABLE;
    if (srcVa != 0) {
        page = pa2page(page_lookup(curProcess->pgdir, srcVa, NULL));
        if (page == NULL || process->env_ipc_dstva >= PHYSICAL_MEMORY_TOP) {
            trapframe->a0 = -1;
            return;
        }
        r = page_insert(curProcess->pgdir, process->env_ipc_dstva, page2pa(page), perm);
        if (r) {
            trapframe->a0 = -1;
            return;
        }
    }
    trapframe->a0 = 0;
}

void syscallSdRead() {
    Trapframe* trapframe = getTrapFrame();
    u_char* buf = (u_char *) trapframe->a0;
    u_longlong startSector = trapframe->a1;
    u_int sectorNumber = trapframe->a2;
    sdRead(buf, startSector, sectorNumber);
}

void syscallSdWrite() {
    printf("wa\n");
    Trapframe* trapframe = getTrapFrame();
    u_char* buf = (u_char *) trapframe->a0;
    u_longlong startSector = trapframe->a1;
    u_int sectorNumber = trapframe->a2;
    sdWrite(buf, startSector, sectorNumber);
}

void syscallMapMemoryFromTo() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong srcId = trapframe->a0;
    u_longlong srcVa = trapframe->a1;
    u_longlong dstId = trapframe->a2;
    u_longlong dstVa = trapframe->a3;
    u_longlong perm = trapframe->a4;
    int ret;
    u_longlong roundSrcVa, roundDstVa;
    struct Process *srcProcess, *dstProcess;
    struct PhysicalPage *ppage = NULL;
    u_longlong* ppte;
    roundSrcVa = DOWN_ALIGN(srcVa, PAGE_SIZE);
    roundDstVa = DOWN_ALIGN(dstVa, PAGE_SIZE);
    if ((perm & PTE_V) == 0) {
        trapframe->a0 = -E_INVAL;
        return;
    }
//    if (srcVa >= UTOP || dstVa >= UTOP) {
//        trapframe->a0 = -E_INVAL;
//        return;
//    }
    if ((ret = pid2Process(srcId, &srcProcess, 0)) < 0) {
        trapframe->a0 = ret;
        return;
    }
    if ((ret = pid2Process(dstId, &dstProcess, 0)) < 0) {
        trapframe->a0 = ret;
        return;
    }
    ppage = pa2page(page_lookup(srcProcess->pgdir, roundSrcVa, &ppte));
    if (ppage == NULL) {
        trapframe->a0 = -E_INVAL;
        return;
    }
    ret = page_insert(dstProcess->pgdir, roundDstVa, page2pa(ppage), perm);
    trapframe->a0 = ret;
}

void syscallUnmapMemoryFrom() {
    Trapframe* trapframe = getTrapFrame();
    int ret;
    u_longlong pid = trapframe->a0;
    u_longlong va = trapframe->a1;
    struct Process *process;
//    if (va >= UTOP) {
//        trapframe->a0 = -E_INVAL;
//        return;
//    }
    ret = pid2Process(pid, &process, 0);
    if (ret < 0) {
        trapframe->a0 = ret;
        return;
    }
    page_remove(process->pgdir, va);
    trapframe->a0 = ret;
}

void syscallMemoryAlloc() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong pid = trapframe->a0;
    u_longlong va = trapframe->a1;
    u_longlong perm = trapframe->a2;
    struct Process* process;
    struct PhysicalPage *ppage;
    int ret;
    if ((perm & PTE_V) == 0) {
        trapframe->a0 = -E_INVAL;
        return;
    }
    if (perm & PTE_C) {
        trapframe->a0 = -E_INVAL;
        return;
    }
    ret = pid2Process(pid, &process, 1);
    if (ret < 0) {
        trapframe->a0 = ret;
        return;
    }
    ret = page_alloc(&ppage);
    if (ret < 0) {
        trapframe->a0 = ret;
        return;
    }
    ret = page_insert(process->pgdir, va, page2pa(ppage), perm);
    if (ret < 0) {
        trapframe->a0 = ret;
        return;
    }
    trapframe->a0 = 0;
}

void syscallCheckVaIsMap() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong va = trapframe->a0;
    u_longlong *pte;
    page_walk(curProcess->pgdir, va, 0, &pte);
    if (pte) {
        trapframe->a0 = 1;
        return;
    } else {
        trapframe->a0 = 0;
        return;
    }
}

void syscallGetProcessPcb() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong pid = trapframe->a0;
    struct Process* process;
    pid2Process(pid, &process, 0);
    trapframe->a0 = (u_longlong) process;
}

void syscallGetPhysicalPageRef() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong va = trapframe->a0;
    struct PhysicalPage* page = pa2page(page_lookup(curProcess->pgdir, va, 0));
    trapframe->a0 = page->ref;
}

void syscallGetPhysicalPageAddr() {
    Trapframe* trapframe = getTrapFrame();
    u_longlong va = trapframe->a0;
    trapframe->a0 = page_lookup(curProcess->pgdir, va, 0);
}