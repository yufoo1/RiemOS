#include "../../fs/server.h"
#include "../include/printf.h"
#include "../include/syscallLib.h"

int userMain(int argc, char **argv) {
    printf("ide test begin!\n");
    printf("please input: \n");
    // int i;
    // for(i = 0; i < 10; i++) {
    //     char c = getchar(); // 系统调用，获得字符并打印
    //     printf("%c", c);
    // }
    char* s = "abcd";
    printf("start write\n");
    sd_Write(s, 25, 1);
    printf("write done\n");
    char* t;
    sd_Read(t, 25, 1);
    printf("%s", t);
    return 0;
}