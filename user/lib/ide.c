/*
 * operations on IDE disk.
 */

#include "../include/syscallLib.h"
#include "../include/ide.h"
#include "../../include/sd.h"
#include "../../include/process.h"
#include "../include/printf.h"
#include "../../include/riscv.h"

void ide_write(void *src, u_int secno, u_int nsecs)
{
    sd_Write(src, secno, nsecs);
}

void ide_read(void *dst, u_int secno, u_int nsecs)
{
    sd_Read(dst, secno, nsecs);
}