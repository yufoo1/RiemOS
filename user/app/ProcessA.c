#include "../include/printf.h"

int userMain(int argc, char **argv) {
    for (int i = 1; i <= 100000; ++ i) {
        putchar('A');
    }
    return 0;
}