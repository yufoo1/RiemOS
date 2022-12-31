#ifndef _SYSCALL_ID_H_
#define _SYSCALL_ID_H_
#define SYSCALL_PUTCHAR 1
#define SYSCALL_GETCHAR 2
#define SYSCALL_GET_PROCESS_ID 3
#define SYSCALL_PUT_STRING 4
#define SYSCALL_EXIT 5
#define SYSCALL_SCHED_YIELD 6
#define SYSCALL_GET_PARENT_PID 7
#define SYSCALL_CLONE 8
// IPC
#define SYSCALL_IPC_RECV 9
#define SYSCALL_IPC_CAN_SEND 10
// SD
#define SYSCALL_SD_READ 11
#define SYSCALL_SD_WRITE 12

// memory related
#define SYSCALL_MAP_MEMORY_FROM_TO 13
#define SYSCALL_UNMAP_MEMORY_FROM 14
#define SYSCALL_MEMORY_ALLOC 15
#define SYSCALL_CHECK_VA_IS_MAP 16
#define SYSCALL_GET_PROCESS_PCB 17
#define SYSCALL_GET_PHYSICAL_PAGE_REF 18
#define SYSCALL_GET_PHYSICAL_PAGE_ADDR 19

extern void (*syscallVector[])(void);

void syscallPutchar();
void syscallGetchar();
void syscallGetProcessId();
void syscallYield();
void syscallFork();
void syscallPutString();
void syscallGetProcessId();
void syscallGetParentProcessId();
void syscallExit();
void syscallIpcRecv();
void syscallIpcCanSend();
void syscallSdRead();
void syscallSdWrite();
void syscallMapMemoryFromTo();
void syscallUnmapMemoryFrom();
void syscallMemoryAlloc();
void syscallCheckVaIsMap();
void syscallGetProcessPcb();
void syscallGetPhysicalPageRef();
void syscallGetPhysicalPageAddr();


#endif