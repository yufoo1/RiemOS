#ifndef _USER_SYSCALL_LIB_H_
#define _USER_SYSCALL_LIB_H_

#include "../../include/syscallLib.h"
#include "../../include/type.h"
#include "syscall.h"

extern char printfBuffer[];
extern int bufferLen;

static inline void putchar(char c) {
    msyscall(SYSCALL_PUTCHAR, c, 0, 0, 0, 0, 0);
}

static inline char getchar() {
    return msyscall(SYSCALL_GETCHAR, 0, 0, 0, 0, 0, 0);
}

static inline void putString(char* buffer, u_longlong len) {
    msyscall(SYSCALL_PUT_STRING, (u_longlong) buffer, (u_longlong) len, 0, 0, 0, 0);
}

static inline u_int fork() {
    return msyscall(SYSCALL_CLONE, 0, 0, 0, 0, 0, 0);
}

static inline u_int getprocessId() {
    return msyscall(SYSCALL_GET_PROCESS_ID, 0, 0, 0, 0, 0, 0);
}

static inline u_int getParentProcessId() {
    return msyscall(SYSCALL_GET_PARENT_PID, 0, 0, 0, 0, 0, 0);
}

static inline int exit(int ec) {
    return msyscall(SYSCALL_EXIT, (u_longlong)ec, 0, 0, 0, 0, 0);
}

static inline int schedYield() {
    return msyscall(SYSCALL_SCHED_YIELD, 0, 0, 0, 0, 0, 0);
}

static inline int ipcSend(u_longlong pid, u_longlong value, u_longlong srcVa, u_longlong perm) {
    return msyscall(SYSCALL_IPC_CAN_SEND, pid, value, srcVa, perm, 0, 0);
}

static inline int ipcRecv(u_longlong dstVa) {
    return msyscall(SYSCALL_IPC_RECV, dstVa, 0, 0, 0, 0, 0);
}

static inline int sd_Read(char* buf, u_longlong startSector, u_int sectorNumber) {
    return msyscall(SYSCALL_SD_READ, (u_longlong)buf, startSector, sectorNumber, 0, 0, 0);
}

static inline int sd_Write(char* buf, u_longlong startSector, u_int sectorNumber) {
    return msyscall(SYSCALL_SD_WRITE, (u_longlong)buf, startSector, sectorNumber, 0, 0, 0);
}

static inline int mapMemoryFromTo(u_longlong srcId, u_longlong srcVa, u_longlong dstId, u_longlong dstVa, u_longlong perm){
    return msyscall(SYSCALL_MAP_MEMORY_FROM_TO, (u_longlong)srcId, (u_longlong)srcVa, (u_longlong)dstId, (u_longlong)dstVa, (u_longlong)perm, 0);
}

static inline int unmapMemoryFrom(u_longlong pid, u_longlong va) {
    return msyscall(SYSCALL_UNMAP_MEMORY_FROM, (u_longlong)pid, (u_longlong)va, 0, 0, 0, 0);
}

static inline int memoryAlloc(u_longlong pid, u_longlong va, u_longlong perm) {
    return msyscall(SYSCALL_MEMORY_ALLOC, (u_longlong)pid, (u_longlong)va, (u_longlong)perm, 0, 0, 0);
}

static inline int checkVaIsMap(u_longlong va) {
    return msyscall(SYSCALL_CHECK_VA_IS_MAP, va, 0, 0, 0, 0, 0);
}

static inline u_longlong getProcessPcb(u_longlong pid) {
    return msyscall(SYSCALL_GET_PROCESS_PCB, pid, 0, 0, 0, 0, 0);
}

static inline u_int getPhysicalPageRef(u_longlong va) {
    return msyscall(SYSCALL_GET_PHYSICAL_PAGE_REF, va, 0, 0, 0, 0, 0);
}

static inline u_longlong getPhysicalPageAddr(u_longlong va) {
    return msyscall(SYSCALL_GET_PHYSICAL_PAGE_ADDR, va, 0, 0, 0, 0, 0);
}

#endif