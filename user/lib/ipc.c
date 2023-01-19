#include "../include/printf.h"
#include "../../include/process.h"
#include "../../include/error.h"
#include "../include/debugf.h"

void
ipc_send(u_longlong whom, u_longlong val, u_longlong srcva, u_longlong perm)
{
    int r;

    while ((r = ipcSend(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
        schedYield();
    }

    if (r == 0) {
        return;
    }

    user_panic("error in ipc_send: %d", r);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int
ipc_recv(u_longlong *whom, u_longlong dstva, u_longlong *perm)
{
    ipcRecv(dstva);
    struct Process *process;
    process = (struct Process *) getProcessPcb(getprocessId());
    if (whom) {
        *whom = process->env_ipc_from;
    }

    if (perm) {
        *perm = process->env_ipc_perm;
    }

    return process->env_ipc_value;
}

