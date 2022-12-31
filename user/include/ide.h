

#ifndef FIBOS_FINAL_IDE_H
#define FIBOS_FINAL_IDE_H

#include "../../include/type.h"

void ide_read(void *dst, u_int secno, u_int nsecs);
void ide_write(void *src, u_int secno, u_int nsecs);


#endif
