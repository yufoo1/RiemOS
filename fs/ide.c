/*
 * operations on IDE disk.
 */

#include "../user/include/syscallLib.h"
void ide_write(void *src, u_int secno, u_int nsecs)
{
    sd_Write(src, secno, nsecs);
}

void ide_read(void *dst, u_int secno, u_int nsecs)
{
    sd_Read(dst, secno, nsecs);
}