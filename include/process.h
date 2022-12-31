#ifndef _PROCESS_H_
#define _PROCESS_H_

#include "./type.h"
#include "./queue.h"
#include "./kclock.h"

#define LOG_PROCESS_NUM 10
#define PROCESS_TOTAL_NUMBER (1 << LOG_PROCESS_NUM)
#define PROCESS_OFFSET(processId) ((processId) & (PROCESS_TOTAL_NUMBER - 1))

#define PROCESS_CREATE_PRIORITY(x, y) { \
    extern u_char binary##x##Start[]; \
    extern int binary##x##Size; \
    processCreatePriority(binary##x##Start, binary##x##Size, y); \
}
typedef struct Trapframe {
    u_longlong kernelSatp;
    u_longlong kernelSp;
    u_longlong stvec;
    u_longlong sepc;
    u_longlong ra;
    u_longlong sp;
    u_longlong gp;
    u_longlong tp;
    u_longlong t0;
    u_longlong t1;
    u_longlong t2;
    u_longlong s0;
    u_longlong s1;
    u_longlong a0;
    u_longlong a1;
    u_longlong a2;
    u_longlong a3;
    u_longlong a4;
    u_longlong a5;
    u_longlong a6;
    u_longlong a7;
    u_longlong s2;
    u_longlong s3;
    u_longlong s4;
    u_longlong s5;
    u_longlong s6;
    u_longlong s7;
    u_longlong s8;
    u_longlong s9;
    u_longlong s10;
    u_longlong s11;
    u_longlong t3;
    u_longlong t4;
    u_longlong t5;
    u_longlong t6;
} Trapframe;

enum ProcessState {
    FREE,
    BLOCKED,
    RUNNABLE,
};

typedef struct Process {
    Trapframe trapframe;
    LIST_ENTRY(Process) link;
    u_longlong *pgdir;
    u_int id;
    u_int parentId;
    LIST_ENTRY(Process) scheduleLink;
    u_int priority;
    enum ProcessState state;
    // for ipc;
    u_longlong env_ipc_value;            // data value sent to us
    u_longlong env_ipc_from;             // envid of the sender
    u_longlong env_ipc_recving;          // env is blocked receiving
    u_longlong env_ipc_dstva;		// va at which to map received page
    u_longlong env_ipc_perm;

    // Lab 6 scheduler counts
    u_longlong runs;			// number of times been env_run'ed

} Process;

LIST_HEAD(ProcessList, Process);

u_longlong getProcessTopSp(Process* p);
void process_management_init();
void processCreatePriority(u_char* binary, u_int size, u_int priority);
void process_yield();
void process_fork();
void process_destory(Process* p);
void process_free(Process* p);
int pid2Process(u_int processId, struct Process **process, int checkPerm);
Process* myproc();
int either_copyout(int user_dst, u_longlong dst, void* src, u_longlong len);
int either_copyin(void* dst, int user_src, u_longlong src, u_longlong len);
int process_setup(Process *p);
struct Trapframe;
struct Trapframe* getTrapFrame(void);
#endif