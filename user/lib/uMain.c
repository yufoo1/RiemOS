#include "../include/uMain.h"
#include "../include/syscallLib.h"

int libMain(int argc, char **argv) {
    int ret = userMain(argc, argv);
    exit(ret);
}