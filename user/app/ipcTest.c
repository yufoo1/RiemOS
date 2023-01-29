#include "../include/syscall.h"
#include "../include/printf.h"
#include "../include/ipc.h"

int userMain(int argc, char **argv) {
    u_longlong who;
    if ((who = fork()) == 0) {
        printf("\n@@@@@send value 100 from %x to %x\n", getprocessId(), getParentProcessId());
        ipc_send(getParentProcessId(), 100, 0, 0);
    } else {
        printf("%x am waiting.....\n", getprocessId());
        ipc_recv(&who, 0, 0);
        printf("get value successfully!\n");
    }
    return 0;
}
