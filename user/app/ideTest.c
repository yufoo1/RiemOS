#include "../include/ide.h"
#include "../include/printf.h"
#include "../include/syscallLib.h"

int userMain(int argc, char **argv) {
    printf("ide test begin!\n");
    printf("please input: \n");
    while (1) {
        char c = getchar(); // 系统调用，获得字符并打印
        printf("%c", c);
    }
    return 0;
}