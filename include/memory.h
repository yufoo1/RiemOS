#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "./type.h"
#include "./queue.h"
#include "./memConfig.h"
#include "./driver.h"

#define NULL 0
#define DOWN_ALIGN(x, y) (((u_longlong)(x)) & (~((u_longlong)((y) - 1))))
#define UP_ALIGN(x, y) (DOWN_ALIGN((x) - 1, (y)) + (y))

#define PGSIZE (4096)
#define PDMAP (0x200000)
struct PhysicalPage;
LIST_HEAD(PageList, PhysicalPage);
typedef struct PageList PageList;
typedef LIST_ENTRY(PhysicalPage) PageListEntry;

#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

typedef struct PhysicalPage {
    PageListEntry link;
    u_int ref;
} PhysicalPage;

inline u_int page2PPN(PhysicalPage *page) {
    extern PhysicalPage pages[];
    return page - pages;
}

inline PhysicalPage* ppn2page(u_int ppn) {
    extern PhysicalPage pages[];
    return pages + ppn;
}


inline u_longlong page2pa(PhysicalPage *page) {
    return PHYSICAL_ADDRESS_BASE + (page2PPN(page) << PAGE_SHIFT);
}

inline PhysicalPage* pa2page(u_longlong pa) {
    return ppn2page((pa - PHYSICAL_ADDRESS_BASE) >> PAGE_SHIFT);
}

void bcopy(void *src, void *dst, u_int len);
void bzero(void *start, u_int len);
void memory_management_init();
void mmu_init();

#define PA2PPN(va) ((((u_longlong)(va)) & 0x0fffffff) >> PAGE_SHIFT)
#define PPN2PA(ppn) (((u_longlong)(ppn)) << PAGE_SHIFT)
#define GET_PAGE_TABLE_INDEX(va, level) ((((u_longlong)(va)) >> (PAGE_SHIFT + 9 * level)) & 0x1ff)
#define PTE2PT 512

#define PTE_V (1ll << 0)
#define PTE_R (1ll << 1)
#define PTE_W (1ll << 2)
#define PTE_X (1ll << 3)
#define PTE_U (1ll << 4)
#define PTE_G (1ll << 5)
#define PTE_A (1ll << 6)
#define PTE_D (1ll << 7)
#define PTE_C (1ll << 8)
#define PERM_WIDTH 10
#define PTE2PERM(pte) (((u_longlong)(pte)) & ~((1ull << 54) - (1ull << 10)))
#define PTE2PA(pte) (((((u_longlong)(pte)) & ((1ull << 54) - (1ull << 10))) >> PERM_WIDTH) << PAGE_SHIFT)
#define PA2PTE(pa) ((((u_longlong)(pa)) >> PAGE_SHIFT) << PERM_WIDTH)

inline u_longlong page2pte(PhysicalPage *page) {
    return (page2pa(page) >> PAGE_SHIFT) << PERM_WIDTH;
}

inline PhysicalPage* pte2page(u_longlong *pte) {
    return pa2page(PTE2PA(*pte));
}

int page_remove(u_longlong *pgdir, u_longlong va);
int page_alloc(PhysicalPage **pp);
int page_insert(u_longlong *pgdir, u_longlong va, u_longlong pa, u_longlong perm);
void pgdir_free(u_longlong* pgdir);
u_longlong page_lookup(u_longlong *pgdir, u_longlong va, u_longlong **pte);
int pgdir_alloc(PhysicalPage **page);
void pageout(u_longlong *pgdir, u_longlong badAddr);
void cowHandler(u_longlong *pgdir, u_longlong badAddr);
void page_free(PhysicalPage *page);
int page_walk(u_longlong *pgdir, u_longlong va, bool create, u_longlong **pte);
u_longlong vir2phy(u_longlong* pagetable, u_longlong va, int* cow);
int copyin(u_longlong* pagetable, char* dst, u_longlong srcva, u_longlong len);
int copyout(u_longlong* pagetable, u_longlong dstva, char* src, u_longlong len);

#endif