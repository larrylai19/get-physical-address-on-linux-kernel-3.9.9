#include <linux/init_task.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/linkage.h>
#include <linux/highmem.h>
#include <linux/gfp.h>

asmlinkage void* virt_to_phy(void* user_vaddr) {
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    printk("\ninput virtual address: 0x%p\n", user_vaddr);

    unsigned long vaddr = (unsigned long)user_vaddr;
    printk("vaddr: 0x%p\n", vaddr);

    // page global directory
    pgd = pgd_offset(current->mm, vaddr);
    printk("pgd_val = 0x%lx\n", pgd_val(*pgd));
    printk("pgd_index = %lu\n", pgd_index(vaddr));
    if (pgd_none(*pgd)) {
        printk("Not mapped in pgd!\n");
        return (void*)0;
    }

    // page upper directory
    pud = pud_offset(pgd, vaddr);
    printk("pud_val = 0x%lx\n", pud_val(*pud));
    if (pud_none(*pud)) {
        printk("Not mapped in pud!\n");
        return (void*)0;
    }

    // page middle directory
    pmd = pmd_offset(pud, vaddr);
    printk("pmd_val = 0x%lx\n", pmd_val(*pmd));
    printk("pmd_index = %lu\n", pmd_index(vaddr));
    if (pmd_none(*pmd)) {
        printk("Not mapped in pmd!\n");
        return (void*)0;
    }

    // page table
    pte = pte_offset_map(pmd, vaddr);
    printk("pte_val = 0x%lx\n", pte_val(*pte));
    printk("pte_index = %lu\n", pte_index(vaddr));
    if (pte_none(*pte)) {
        printk("Not mapped in pte!\n");
        return (void*)0;
    }

    struct page *page = pte_page(*pte);
    pte_unmap(pte);
    unsigned long page_address = page_to_phys(page);
    printk("page_address: 0x%p\n", page_address);

    unsigned long page_offset = vaddr & ~PAGE_MASK;
    unsigned long physical_address = page_address | page_offset;

    printk("page_address = 0x%p, page_offset = %lx\n", page_address, page_offset);
    printk("virtual_address = 0x%p, physical_address = 0x%p\n", vaddr, physical_address);

    return (void*)physical_address;
}