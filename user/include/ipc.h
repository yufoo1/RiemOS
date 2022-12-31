//
// Created by yufu on 2022/7/2.
//

#include "../../include/type.h"

#ifndef FIBOS_FINAL_IPC_H
#define FIBOS_FINAL_IPC_H

void ipc_send(u_longlong whom, u_longlong val, u_longlong srcva, u_longlong perm);
u_int ipc_recv(u_longlong *whom, u_longlong dstva, u_longlong *perm);

#endif //FIBOS_FINAL_IPC_H
