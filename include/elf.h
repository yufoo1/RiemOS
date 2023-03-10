#ifndef _ELF_H_
#define _ELF_H_

#include <type.h>

#define MAG_SIZE 4
#define ELF_MAGIC0  0x7f
#define ELF_MAGIC1  0x45
#define ELF_MAGIC2  0x4c
#define ELF_MAGIC3  0x46

#define PT_NULL		0x00000000
#define PT_LOAD		0x00000001
#define PT_DYNAMIC	0x00000002
#define PT_INTERP	0x00000003
#define PT_NOTE		0x00000004
#define PT_SHLIB	0x00000005
#define PT_PHDR		0x00000006
#define PT_LOOS		0x60000000
#define PT_HIOS		0x6fffffff
#define PT_LOPROC	0x70000000
#define PT_HIRPOC	0x7fffffff

#define PF_ALL	0x7
#define PF_X	0x1
#define PF_W	0x2
#define PF_R	0x4

#define ET_EXEC 2

typedef struct {
	u_char magic[MAG_SIZE];
	u_char type;
	u_char data;
	u_char version;
	u_char osabi;
	u_char abiversion;
	u_char pad[7];
} Indent;

typedef struct {
	Indent indent;
	u_short type;
	u_short machine;
	u_int version;
	u_longlong entry;
	u_longlong phoff;
	u_longlong shoff;
	u_int flags;
	u_short ehsize;
	u_short phentsize;
	u_short phnum;
	u_short shentsize;
	u_short shnum;
	u_short shstrndx;
} Ehdr;

typedef struct {
	u_int type;
	u_int flags;
	u_longlong offset;
	u_longlong vaddr;
	u_longlong paddr;
	u_longlong filesz;
	u_longlong memsz;
	u_longlong align;
} Phdr;

int loadElf(u_char *binary, int size, u_longlong *entry, void *userData,
            int (*map)(u_longlong, u_int, u_char*, u_int, void*));

inline bool is_elf_format(u_char *binary) {
    u_char *magic = ((Indent*) binary)->magic;
    if (magic[0] == ELF_MAGIC0 &&
        magic[1] == ELF_MAGIC1 &&
        magic[2] == ELF_MAGIC2 &&
        magic[3] == ELF_MAGIC3)
        return true;
    return false;
}

#endif