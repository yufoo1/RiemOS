#ifndef _SD_H_
#define _SD_H_

#include "type.h"

void sdInit();
int sdRead(u_char *buf, u_longlong startSector, u_int sectorNumber);
int sdWrite(u_char *buf, u_longlong startSector, u_int sectorNumber);

#endif