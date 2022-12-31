#include "../include/syscall.h"
#include "../include/printf.h"

int userMain(int argc, char **argv) {
    printf("start fork test....\n");
    int a = 0;
    int id = 0;
    if ((id = fork()) == 0) {
        if ((id = fork()) == 0) {
            a += 3;
            if ((id = fork()) == 0) {
                a += 4;
                for (;;) {
                    printf("this is child3: a:%d\n", a);
                }
            }
            for (;;) {
                printf("this is child2: a:%d\n", a);
            }
        }
        a += 2;
        for (;;) {
            printf("this is child: a:%d\n", a);
        }
    }
    a++;
    for (;;) {
        printf("this is father: a:%d\n", a);
    }
return 0;
}

