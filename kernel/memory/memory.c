#include "../../include/memory.h"
#include "../../include/riscv.h"
#include "../../include/error.h"
#include "../../include/mmio.h"


extern PageList freePages;

PageList freePages;
PhysicalPage pages[PHYSICAL_PAGE_NUM];
extern char kernelStart[];
extern char kernelEnd[];
extern u_longlong kernelPageDirectory[];

static void pages_init() {
    u_int i;
    u_int n = PA2PPN(kernelEnd);
    for (i = 0; i < n; i++) {
        pages[i].ref = 1;
    }

    n = PA2PPN(PHYSICAL_MEMORY_TOP);
    LIST_INIT(&freePages);
    for (; i < n; i++) {
        pages[i].ref = 0;
        LIST_INSERT_HEAD(&freePages, &pages[i], link);
    }
}

static void virtual_memory_map() {
    u_longlong va, pa;
    page_insert(kernelPageDirectory, UART_V, UART, PTE_R | PTE_W | PTE_A | PTE_D);
    va = CLINT_V, pa = CLINT;
    for (u_longlong i = 0; i < 0x10000; i += PAGE_SIZE) {
        page_insert(kernelPageDirectory, va + i, pa + i, PTE_R | PTE_W | PTE_A | PTE_D);
    }
    va = PLIC_V; pa = PLIC;
    for (u_longlong i = 0; i < 0x4000; i += PAGE_SIZE) {
        page_insert(kernelPageDirectory, va + i, pa + i, PTE_R | PTE_W | PTE_A | PTE_D);
    }
    va = PLIC_V + 0x200000; pa = PLIC + 0x200000;
    for (u_longlong i = 0; i < 0x4000; i += PAGE_SIZE) {
        page_insert(kernelPageDirectory, va + i, pa + i, PTE_R | PTE_W | PTE_A | PTE_D);
    }
    page_insert(kernelPageDirectory, SPI_CTRL_ADDR, SPI_CTRL_ADDR, PTE_R | PTE_W | PTE_A | PTE_D);
    page_insert(kernelPageDirectory, UART_CTRL_ADDR, UART_CTRL_ADDR, PTE_R | PTE_W | PTE_A | PTE_D);
    extern char textEnd[];
    va = pa = (u_longlong)kernelStart;
    for (u_longlong i = 0; va + i < (u_longlong)textEnd; i += PAGE_SIZE) {
        page_insert(kernelPageDirectory, va + i, pa + i, PTE_R | PTE_X | PTE_W | PTE_A | PTE_D);
    }
    va = pa = (u_longlong)textEnd;
    for (u_longlong i = 0; va + i < PHYSICAL_MEMORY_TOP; i += PAGE_SIZE) {
        page_insert(kernelPageDirectory, va + i, pa + i, PTE_R | PTE_W | PTE_A | PTE_D);
    }
    extern char trampoline[];
    page_insert(kernelPageDirectory, TRAMPOLINE_BASE, (u_longlong)trampoline,PTE_R | PTE_W | PTE_X | PTE_A | PTE_D);
    page_insert(kernelPageDirectory, TRAMPOLINE_BASE + PAGE_SIZE, (u_longlong)trampoline + PAGE_SIZE,PTE_R | PTE_W | PTE_X | PTE_A | PTE_D);
}

void mmu_init() {
    w_satp(MAKE_SATP(kernelPageDirectory));
    sfence_vma();
}

void memory_management_init() {
    printf("Memory init start...\n");
    pages_init();
    virtual_memory_map();
    mmu_init();
    printf("Memory init finish!\n");
}

int page_remove(u_longlong *pgdir, u_longlong va) {
    u_longlong *pte;
    u_longlong pa = page_lookup(pgdir, va, &pte);

    if (!pte) {
        return -1;
    }
    if (pa < PHYSICAL_ADDRESS_BASE || pa >= PHYSICAL_MEMORY_TOP) {
        return -1;
    }
    PhysicalPage *page = pa2page(pa);
    page->ref--;
    page_free(page);
    *pte = 0;
    sfence_vma();
    return 0;
}

int page_alloc(PhysicalPage **pp) {
    PhysicalPage *page;
    if ((page = LIST_FIRST(&freePages)) != NULL) {
        *pp = page;
        LIST_REMOVE(page, link);
        bzero((void*)page2pa(page), PAGE_SIZE);
        return 0;
    }
    *pp = NULL;
    return -E_NO_MEM;
}

int page_walk(u_longlong *pgdir, u_longlong va, bool create, u_longlong **pte) {
    int level;
    u_longlong *addr = pgdir;
    for (level = 2; level > 0; level--) {
        addr += GET_PAGE_TABLE_INDEX(va, level);
        if (!(*addr & PTE_V)) {
            if (!create) {
                *pte = NULL;
                return 0;
            }
            PhysicalPage *pp;
            int ret = page_alloc(&pp);
            if (ret < 0) {
                return ret;
            }
            (*addr) = page2pte(pp) | PTE_V;
            pp->ref++;
        }
        addr = (u_longlong*)PTE2PA(*addr);
    }
    *pte = addr + GET_PAGE_TABLE_INDEX(va, 0);
    return 0;
}

u_longlong page_lookup(u_longlong *pgdir, u_longlong va, u_longlong **pte) {
    u_longlong *entry;
    page_walk(pgdir, va, false, &entry);
    if (entry == NULL || !(*entry & PTE_V)) {
        return 0;
    }
    if (pte) {
        *pte = entry;
    }
    return PTE2PA(*entry);
}

void page_free(PhysicalPage *page) {
    if (page->ref > 0) {
        return;
    }
    if (page->ref == 0) {
        LIST_INSERT_HEAD(&freePages, page, link);
    }
}

static void page_ref_decrease(u_longlong pa) {
    PhysicalPage *page = pa2page(pa);
    page->ref--;
    if (page->ref == 0) {
        LIST_INSERT_HEAD(&freePages, page, link);
    }
}

void pgdir_free(u_longlong* pgdir) {
    u_longlong i, j, k;
    u_longlong* pageTable;
    for (i = 0; i < PTE2PT; i++) {
        if (!(pgdir[i] & PTE_V))
            continue;
        pageTable = pgdir + i;
        u_longlong* pa = (u_longlong*) PTE2PA(*pageTable);
        for (j = 0; j < PTE2PT; j++) {
            if (!(pa[j] & PTE_V))
                continue;
            pageTable = (u_longlong*) pa + j;
            u_longlong* pa2 = (u_longlong*) PTE2PA(*pageTable);
            for (k = 0; k < PTE2PT; k++) {
                if (!(pa2[k] & PTE_V))
                    continue;
                u_longlong addr = (i << 30) | (j << 21) | (k << 12);
                page_remove(pgdir, addr);
            }
            pa2[j] = 0;
            page_ref_decrease((u_longlong) pa2);
        }
        page_ref_decrease((u_longlong) pa);
    }
    page_ref_decrease((u_longlong) pgdir);
}

int page_insert(u_longlong *pgdir, u_longlong va, u_longlong pa, u_longlong perm) {
    u_longlong *pte;
    va = DOWN_ALIGN(va, PAGE_SIZE);
    pa = DOWN_ALIGN(pa, PAGE_SIZE);
    perm |= PTE_A | PTE_D;
    int ret = page_walk(pgdir, va, false, &pte);
    if (ret < 0) {
        return ret;
    }
    if (pte != NULL && (*pte & PTE_V)) {
        page_remove(pgdir, va);
    }
    ret = page_walk(pgdir, va, true, &pte);
    if (ret < 0) {
        return ret;
    }
    *pte = PA2PTE(pa) | perm | PTE_V;
    if (pa >= PHYSICAL_ADDRESS_BASE && pa < PHYSICAL_MEMORY_TOP)
        pa2page(pa)->ref++;
    sfence_vma();
    return 0;
}

int pgdir_alloc(PhysicalPage **page) {
    int r;
    if ((r = page_alloc(page)) < 0) {
        return r;
    }
    (*page)->ref++;
    return 0;
}

void pageout(u_longlong *pgdir, u_longlong badAddr) {
    if (badAddr <= PAGE_SIZE) {
        panic("^^^^^^^^^^TOO LOW^^^^^^^^^^^\n");
    }
    printf("[Page out]pageout at %lx\n", badAddr);
    PhysicalPage *page;
    if (page_alloc(&page) < 0) {
        panic("");
    }
    if (page_insert(pgdir, badAddr, page2pa(page),
                    PTE_U | PTE_R | PTE_W) < 0) {
        panic("");
    }
}

u_char cowBuffer[PAGE_SIZE];
void cowHandler(u_longlong *pgdir, u_longlong badAddr) {
    u_longlong pa;
    u_longlong *pte;
    pa = page_lookup(pgdir, badAddr, &pte);
    // printf("[COW] %x to cow %lx %lx\n", myproc()->id, badAddr, pa);
    if (!(*pte & PTE_C)) {
        printf("access denied");
        return;
    }
    PhysicalPage *page;
    int r = page_alloc(&page);
    if (r < 0) {
        panic("cow handler error");
        return;
    }
    pa = page_lookup(pgdir, badAddr, &pte);
    bcopy((void *)pa, (void*)cowBuffer, PAGE_SIZE);
    page_insert(pgdir, badAddr, page2pa(page), (PTE2PERM(*pte) | PTE_W) & ~PTE_C);
    bcopy((void*) cowBuffer, (void*) page2pa(page), PAGE_SIZE);
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
u_longlong vir2phy(u_longlong* pagetable, u_longlong va, int* cow) {
    u_longlong* pte;
    u_longlong pa;

    if (va >= MAXVA)
        return NULL;

    int ret = page_walk(pagetable, va, 0, &pte);
    if (ret < 0) {
        panic("page_walk error in vir2phy function!");
    }
    if (pte == 0)
        return NULL;
    if ((*pte & PTE_V) == 0)
        return NULL;
    if ((*pte & PTE_U) == 0)
        return NULL;
    if (cow)
        *cow = (*pte & PTE_C) > 0;
    pa = PTE2PA(*pte) + (va&0xfff);
    return pa;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
 int copyin(u_longlong* pagetable, char* dst, u_longlong srcva, u_longlong len) {
     u_longlong n, va0, pa0;
     int cow;

     while (len > 0) {
         va0 = DOWN_ALIGN(srcva, PGSIZE);
         pa0 = vir2phy(pagetable, va0, &cow);
         if (pa0 == NULL)
             return -1;
         n = PGSIZE - (srcva - va0);
         if (n > len)
             n = len;
         memmove(dst, (void*)(pa0 + (srcva - va0)), n);

         len -= n;
         dst += n;
         srcva = va0 + PGSIZE;
     }
     return 0;
 }

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
 int copyout(u_longlong* pagetable, u_longlong dstva, char* src, u_longlong len) {
     u_longlong n, va0, pa0;
     int cow;

     while (len > 0) {
         va0 = DOWN_ALIGN(dstva, PGSIZE);
         pa0 = vir2phy(pagetable, va0, &cow);
         if (pa0 == NULL)
             return -1;
         if (cow) {
             // printf("COW?\n");
             cowHandler(pagetable, va0);
         }
         pa0 = vir2phy(pagetable, va0, &cow);
         n = PGSIZE - (dstva - va0);
         if (n > len)
             n = len;
         memmove((void*)(pa0 + (dstva - va0)), src, n);
         len -= n;
         src += n;
         dstva = va0 + PGSIZE;
     }
     return 0;
 }

void bcopy(void *src, void *dst, u_int len) {
    void* max = dst + len;
    if (((u_longlong)src & 3) != ((u_longlong)dst & 3)) {
        while (dst < max) {
            *(char *)dst++ = *(char *)src++;
        }
        return;
    }

    while (((u_longlong)dst & 3) && dst < max) {
        *(char *)dst++ = *(char *)src++;
    }

    while (dst + 4 <= max) {
        *(u_int*)dst = *(u_int*)src;
        dst += 4;
        src += 4;
    }
    while (dst < max) {
        *(char *)dst++ = *(char *)src++;
    }
}


void bzero(void *start, u_int len)
{
    void* max = start + len;
    while (((u_longlong)start & 3) && start < max) {
        *(u_char*)start++ = 0;
    }

    while (start + 4 <= max) {
        *(u_int*)start = 0;
        start += 4;
    }

    while(start < max) {
        *(u_char*)start++ = 0;
    }
}