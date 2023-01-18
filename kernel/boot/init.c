#include <driver.h>
#include <memory.h>
#include <trap.h>
#include <process.h>
#include <sd.h>

void sd_test();

void main() {
    extern u_longlong bssStart[];
    extern u_longlong bssEnd[];
    for (u_longlong *i = bssStart; i < bssEnd; i++) {
        *i = 0;
    }
    consoleInit();
    printf("Hello! Welcome to FibOS!\n\n");
    memory_management_init();
    process_management_init();
    sdInit();
//    PROCESS_CREATE_PRIORITY(ProcessA, 1);
//    PROCESS_CREATE_PRIORITY(ProcessB, 1);
//    PROCESS_CREATE_PRIORITY(forkTest, 1);
//    PROCESS_CREATE_PRIORITY(ipcTest, 5);
    // sd_test();
    // PROCESS_CREATE_PRIORITY(ideTest, 5);
//    PROCESS_CREATE_PRIORITY(fsTest, 5);
    trap_init();
    process_yield();
    printf("\n\nAll processes executed successfully!");
    while(1);
}

void sd_test() {
    char a[10];
    a[0] = 'A';
    a[1] = 'B';
    a[2] = 0;
    // char b = 'Z';
    sdWrite(a, 24, 1);
    // sdWrite(&b, 30, 1);
    printf("write finished\n");
    char c[10];
    // char d = 0;
    sdRead(&c, 24, 1);
    // sdRead(d, 30, 1);
    printf("read finished\n");
    printf("%s\n", c);
    // printf("%c\n", d);
}