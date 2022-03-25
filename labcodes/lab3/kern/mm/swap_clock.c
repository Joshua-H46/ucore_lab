#include <defs.h>
#include <x86.h>
#include <stdio.h>
#include <string.h>
#include <swap.h>
#include <swap_fifo.h>
#include <list.h>

list_entry_t clock_list_head;

static int compare_page(pte_t* pte1, pte_t* pte2)
{
    if ((*pte1 & (PTE_D | PTE_A)) > (*pte2 & (PTE_D | PTE_A)))
    {
	return 1;
    }
    return -1;
}

static int clock_init(void)
{
    return 0;
}

static int clock_init_mm(struct mm_struct *mm)
{
    list_init(&clock_list_head);
    mm->sm_priv = &clock_list_head;
    return 0;
}

static int clock_tick_event(struct mm_struct* mm)
{
    return 0;
}

static int clock_map_swappable(struct mm_struct *mm, uintptr_t addr, struct Page *page, int swap_in)
{
    list_entry_t* head = (list_entry_t*) mm->sm_priv;
    list_entry_t* entry = &(page->pra_page_link);
    page->pra_vaddr = addr;

    assert(head != NULL && entry != NULL);
    list_add_before(head, entry);

    return 0;
}

static int clock_set_unswappable(struct mm_struct *mm, uintptr_t addr)
{
    return 0;
}

static int clock_swap_out_victim(struct mm_struct *mm, struct Page **ptr_page, int in_tick)
{
    list_entry_t* head = (list_entry_t*) mm->sm_priv;
    assert(head != NULL);
    struct Page *p = NULL;
    list_entry_t* le = head;
    while ((le = list_next(le)) != head)
    {
	struct Page *page = le2page(le, pra_page_link);
	
	if (p == NULL)
	{
	    p = page;
	    continue;
	}

	pte_t* pte_sel = get_pte(mm->pgdir, p->pra_vaddr, 0);
	pte_t* pte_cur = get_pte(mm->pgdir, page->pra_vaddr, 0);
	if (compare_page(pte_sel, pte_cur) > 0)		// should select current page
	{
	    p = page;
	}
    }

    pte_t* pte = get_pte(mm->pgdir, p->pra_vaddr, 0);
    // I think the PTE_D should also be cleared
    if ((*pte & PTE_A) != 0)
    {
	le = head;
	while ((le = list_next(le)) != head)
	{
	    struct Page *page = le2page(le, pra_page_link);
	    pte_t* pte_cur = get_pte(mm->pgdir, page->pra_vaddr, 0);
	    *pte_cur &= ~PTE_A;
	    tlb_invalidate(mm->pgdir, page->pra_vaddr);
	}
    }
    *ptr_page = p;
    list_del(&(p->pra_page_link));
    return 0;
}

static int clock_check_swap(void)
{
    unsigned char data;
    cprintf("write Virt Page c in fifo_check_swap\n");
    *(unsigned char *)0x3000 = 0x0c;
    assert(pgfault_num==4);
    cprintf("write Virt Page a in fifo_check_swap\n");
    *(unsigned char *)0x1000 = 0x0a;
    assert(pgfault_num==4);
    cprintf("read Virt Page d in fifo_check_swap\n");
    data = *(unsigned char *)0x4000;
    assert(pgfault_num==4);
cprintf("Result: 0x%02x\n", data);
    cprintf("read Virt Page b in fifo_check_swap\n");
    data = *(unsigned char *)0x2000;
cprintf("Result: 0x%02x\n", data);
    assert(pgfault_num==4);
    cprintf("read Virt Page e in fifo_check_swap\n");
    data = *(unsigned char *)0x5000;
cprintf("Result: 0x%02x\n", data);
    assert(pgfault_num==5);
    cprintf("read Virt Page a in fifo_check_swap\n");
    data = *(unsigned char *)0x1000;
cprintf("Result: 0x%02x\n", data);
    assert(pgfault_num==6);

    cprintf("read Virt Page b in fifo_check_swap\n");
    data = *(unsigned char *)0x2000;
cprintf("Result: 0x%02x\n", data);
    assert(pgfault_num==6);
    cprintf("write Virt Page e in fifo_check_swap\n");
    *(unsigned char *)0x5000 = 0x0e;
    assert(pgfault_num==7);
    cprintf("read Virt Page c in fifo_check_swap\n");
    data = *(unsigned char *)0x3000;
cprintf("Result: 0x%02x\n", data);
    assert(pgfault_num==7);
    return 0;
}

struct swap_manager swap_manager_clock = {
	.name = "clock_swap_manager",
	.init = &clock_init,
	.init_mm = &clock_init_mm,
	.tick_event = &clock_tick_event,
	.map_swappable = &clock_map_swappable,
	.set_unswappable = &clock_set_unswappable,
	.swap_out_victim = &clock_swap_out_victim,
	.check_swap = &clock_check_swap
};
