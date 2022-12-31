#include <elf.h>

int loadElf(u_char *binary, int size, u_longlong *entry, void *userData,
            int (*map)(u_longlong va, u_int segmentSize, u_char *bin, u_int binSize, void *userData)) {
    
    Ehdr *ehdr = (Ehdr*) binary;
    Phdr *phdr;

    u_char *ph_table;
    u_short entry_num;
    u_short entry_size;
    if (size < 4 || !is_elf_format(binary)) {
        return -1;
    }

    ph_table = binary + ehdr->phoff;
    entry_num = ehdr->phnum;
    entry_size = ehdr->phentsize;

    while (entry_num--) {
        phdr = (Phdr*)ph_table;
        if (phdr->type == PT_LOAD) {
            int r = map(phdr->vaddr, phdr->memsz, binary + phdr->offset, phdr->filesz, userData);
            if (r < 0) {
                return r;
            }
        }
        ph_table += entry_size;
    }
    *entry = ehdr->entry;
    return 0;
}