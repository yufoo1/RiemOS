#include "../../include/process.h"
#include "../../include/memory.h"
#include "../../include/error.h"
#include "../../include/elf.h"
#include "../../include/riscv.h"
#include "../../include/trap.h"
#include "../../include/mmio.h"

static struct ProcessList freeProcesses;
struct ProcessList scheduleList;
Process processes[PROCESS_TOTAL_NUMBER];
Process *curProcess;

extern void userVector();

u_longlong getProcessTopSp(Process* p) {
    return KERNEL_STACK_TOP - (u_longlong)(p - processes) * 10 * PAGE_SIZE;
}

void process_management_init() {
    printf("Process init start...\n");
    
    LIST_INIT(&freeProcesses);
    LIST_INIT(&scheduleList);
    int i;
    extern u_longlong kernelPageDirectory[];
    for (i = PROCESS_TOTAL_NUMBER - 1; i >= 0; i--) {
        processes[i].state = FREE;
        processes[i].trapframe.kernelSatp = MAKE_SATP(kernelPageDirectory);
        LIST_INSERT_HEAD(&freeProcesses, &processes[i], link);
    }
    w_sscratch((u_longlong) getTrapFrame());
    printf("Process init finish!\n");
}

u_int generate_pid(Process *p) {
    static u_int nextId = 0;
    u_int processId = (++nextId << (1 + LOG_PROCESS_NUM)) | (u_int)(p - processes);
    printf("pid is: %d\n", processId);
    return processId;
}

void process_destory(Process *p) {
    process_free(p);
    if (curProcess == p) {
        curProcess = NULL;
        extern char kernelStack[];
        u_longlong sp = (u_longlong)kernelStack + 2 * KERNEL_STACK_SIZE;
        asm volatile("ld sp, 0(%0)" : :"r"(&sp): "memory");
        process_yield();
    }
}

void process_free(Process *p) {
    pgdir_free(p->pgdir);
    p->state = FREE; // new
}

int pid2Process(u_int processId, struct Process **process, int checkPerm) {
    struct Process* p;

    if (processId == 0) {
        *process = curProcess;
        printf("pid2Process: %x\n", curProcess->id);
        return 0;
    }

    p = processes + PROCESS_OFFSET(processId);

    if (p->state == FREE || p->id != processId) {
        *process = NULL;
        return -E_BAD_ENV;
    }

    if (checkPerm) {
        if (p != curProcess && p->parentId != curProcess->id) {
            *process = NULL;
            return -E_BAD_ENV;
        }
    }

    *process = p;
    return 0;
}

int process_setup(Process *p) {
    int r;
    PhysicalPage *page;
    r = pgdir_alloc(&page);
    if (r < 0) {
        panic("process_setup page alloc error\n");
    }
    
    p->pgdir = (u_longlong*) page2pa(page);
    p->parentId = 0;
    page_alloc(&page);
    extern u_longlong kernelPageDirectory[];
    page_insert(kernelPageDirectory, getProcessTopSp(p) - PGSIZE, page2pa(page), PTE_R | PTE_W | PTE_X);

    extern char trampoline[];
    page_insert(p->pgdir, TRAMPOLINE_BASE, (u_longlong) trampoline,PTE_R | PTE_W | PTE_X);
    page_insert(p->pgdir, TRAMPOLINE_BASE + PAGE_SIZE, ((u_longlong) trampoline) + PAGE_SIZE,PTE_R | PTE_W | PTE_X);
    return 0;
}

void copyKernelPgdir(u_longlong* pgdir) {
    extern u_longlong kernelPageDirectory[];
    u_char* ptr = (u_char*)pgdir;
    for(int i = 0; i < PGSIZE; ++i) {
        ptr = *(((u_char*)kernelPageDirectory) + i);
        ptr++;
    }
}

int process_alloc(Process **new, u_longlong parentId) {
    int r;
    Process *p;
    if (LIST_EMPTY(&freeProcesses)) {
        *new = NULL;
        return -E_NO_FREE_ENV;
    }
    p = LIST_FIRST(&freeProcesses);
    LIST_REMOVE(p, link);
    if ((r = process_setup(p)) < 0) {
        return r;
    }

    p->id = generate_pid(p);
    p->state = RUNNABLE;
    p->parentId = parentId;
    p->trapframe.sp = USER_STACK_TOP;
    copyKernelPgdir(p->pgdir);
    *new = p;
    return 0;
}

int codeMapper(u_longlong va, u_int segmentSize, u_char *binary, u_int binSize, void *userData) {
    Process *process = (Process*)userData;
    PhysicalPage *p = NULL;
    u_longlong i;
    int r = 0;
    u_longlong offset = va - DOWN_ALIGN(va, PAGE_SIZE);
    u_longlong* j;

    if (offset > 0) {
        p = pa2page(page_lookup(process->pgdir, va, &j));
        if (p == NULL) {
            if (page_alloc(&p) < 0) {
                return -1;
            }
            page_insert(process->pgdir, va, page2pa(p),
                        PTE_X | PTE_R | PTE_W | PTE_U);
        }
        r = binSize < PAGE_SIZE - offset ? binSize : PAGE_SIZE - offset;
        bcopy(binary, (void*) page2pa(p) + offset, r);
    }
    for (i = r; i < binSize; i += r) {
        if (page_alloc(&p) != 0) {
            return -1;
        }
        page_insert(process->pgdir, va + i, page2pa(p),
                    PTE_X | PTE_R | PTE_W | PTE_U);
        r = PAGE_SIZE < binSize - i ? PAGE_SIZE : binSize - i;
        bcopy(binary + i, (void*) page2pa(p), r);
    }

    offset = va + i - DOWN_ALIGN(va + i, PAGE_SIZE);
    if (offset > 0) {
        p = pa2page(page_lookup(process->pgdir, va + i, &j));
        if (p == NULL) {
            if (page_alloc(&p) != 0) {
                return -1;
            }
            page_insert(process->pgdir, va + i, page2pa(p),
                        PTE_X | PTE_R | PTE_W | PTE_U);
        }
        r = segmentSize - i < PAGE_SIZE - offset ? segmentSize - i : PAGE_SIZE - offset;
        bzero((void*) page2pa(p) + offset, r);
    }
    for (i += r; i < segmentSize; i += r) {
        if (page_alloc(&p) != 0) {
            return -1;
        }
        page_insert(process->pgdir, va + i, page2pa(p),
                    PTE_X | PTE_R | PTE_W | PTE_U);
        r = PAGE_SIZE < segmentSize - i ? PAGE_SIZE : segmentSize - i;
        bzero((void*) page2pa(p), r);
    }
    return 0;
}

void processCreatePriority(u_char *binary, u_int size, u_int priority) {
    Process *p;
    int r = process_alloc(&p, 0);
    if (r < 0) {
        return;
    }
    p->priority = priority;
    u_longlong entryPoint;
    if (loadElf(binary, size, &entryPoint, p, codeMapper) < 0) {
        panic("process create error\n");
    }
    p->trapframe.sepc = entryPoint;
    LIST_INSERT_TAIL(&scheduleList, p, scheduleLink);
    printf("process create successfully\n");
}

void process_run(Process* p) {
    static volatile int first = 0;
    Trapframe* trapframe = getTrapFrame();
    if (curProcess) {
        bcopy(trapframe, &(curProcess->trapframe),
              sizeof(Trapframe));
    }
    p->state = RUNNABLE;
    curProcess = p;
    printf("curProcess's trapframe is %lx and trapframe addr is %lx, size is %lx\n", &(curProcess->trapframe), trapframe, sizeof(Trapframe));
    bcopy(&(curProcess->trapframe), trapframe, sizeof(Trapframe));
    u_longlong sp = KERNEL_STACK_TOP;
    asm volatile("ld sp, 0(%0)" : :"r"(&sp): "memory");
    userTrapReturn();
}

Process* myproc() {
    if (curProcess == NULL)
        panic("get current process error");
    Process *ret = curProcess;
    return ret;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
int either_copyout(int user_dst, u_longlong dst, void* src, u_longlong len) {
    if (user_dst) {
//         struct Process* p = myproc();//because only this branch uses p->pgdir, so it need call myproc
//         return copyout(p->pgdir, dst, src, len);
    } else {
//        memmove((char*)dst, src, len);
        return 0;
    }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
int either_copyin(void* dst, int user_src, u_longlong src, u_longlong len) {
    if (user_src) {
         struct Process* p = myproc();//because only this branch uses p->pgdir, so it need call myproc
         return copyin(p->pgdir, dst, src, len);
        return 0;
    } else {
         memmove(dst, (char*)src, len);
        return 0;
    }
}

void process_yield() {
    static u_int count = 0;
    Process* process = curProcess;
    if (process && process->state == RUNNABLE) {
        process->state = RUNNABLE;
    }
    while ((count == 0) || !process || (process->state != RUNNABLE)) {
        if (process)
            LIST_INSERT_TAIL(&scheduleList, process, scheduleLink);
        while (LIST_EMPTY(&scheduleList));
        if (!LIST_EMPTY(&scheduleList)) {
            process = LIST_FIRST(&scheduleList);
            LIST_REMOVE(process, scheduleLink);
            count = 1;
        }
    }
    count--;
    printf("process id is %x\n", process->id);
    process_run(process);
}

void process_fork() {
    Process *process;
    int r = process_alloc(&process, curProcess->id);
    if (r < 0) {
        curProcess->trapframe.a0 = r;
        return;
    }
    process->priority = curProcess->priority;
    Trapframe* trapframe = getTrapFrame();
    bcopy(trapframe, &process->trapframe, sizeof(Trapframe));
    process->trapframe.a0 = 0;
    trapframe->a0 = process->id;
    u_longlong i, j, k;
    for (i = 0; i < 512; i++) {
        if (!(curProcess->pgdir[i] & PTE_V)) {
            continue;
        }
        u_longlong *pa = (u_longlong*) PTE2PA(curProcess->pgdir[i]);
        for (j = 0; j < 512; j++) {
            if (!(pa[j] & PTE_V)) {
                continue;
            }
            u_longlong *pa2 = (u_longlong*) PTE2PA(pa[j]);
            for (k = 0; k < 512; k++) {
                if (!(pa2[k] & PTE_V)) {
                    continue;
                }
                u_longlong va = (i << 30) + (j << 21) + (k << 12);
                if (va == TRAMPOLINE_BASE || va == TRAMPOLINE_BASE + PAGE_SIZE) {
                    continue;
                }
                if (pa2[k] & PTE_W) {
                    pa2[k] |= PTE_C;
                    pa2[k] &= ~PTE_W;
                }
                page_insert(process->pgdir, va, PTE2PA(pa2[k]), PTE2PERM(pa2[k]));
            }
        }
    }
    LIST_INSERT_TAIL(&scheduleList, process, scheduleLink);
}

Trapframe* getTrapFrame() {
    return (Trapframe*)(TRAMPOLINE_BASE + PAGE_SIZE + sizeof(Trapframe));
}
