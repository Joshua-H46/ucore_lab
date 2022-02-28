#include <pmm.h>
#include <list.h>
#include <string.h>
#include <buddy_system_pmm.h>

control_area_t control_area;

#define control_list (control_area.control_list)

#define IS_POWER_OF_TWO(x) (!((x) & (x-1)))

static unsigned roundUpTo2sPower(unsigned size_)
{
    if (IS_POWER_OF_TWO(size_))
    {
        return size_;
    }
    size_ |= size_ >> 1;
    size_ |= size_ >> 2;
    size_ |= size_ >> 4;
    size_ |= size_ >> 8;
    size_ |= size_ >> 16;
    return size_ + 1;
}

static void buddy_system_init()
{
    list_init(&control_list);
}

/* buddy_system_init_memmap: initialize the control block list. 
 * Each control block corresponds to a continuous block of memory with size of 2's power.
 */
static void buddy_system_init_memmap(struct Page* base, size_t n)
{
}

static struct Page* buddy_system_alloc_pages(size_t n)
{
}

static void buddy_system_free_pages(struct Page* base, size_t n)
{
}

static size_t buddy_system_nr_free_pages()
{
}

static void buddy_system_check()
{
}

const struct pmm_manager buddy_system_pmm_manager = {
    .name = "buddy_system_pmm_manager",
    .init = buddy_system_init,
    .init_memmap = buddy_system_init_memmap,
    .alloc_pages = buddy_system_alloc_pages,
    .free_pages = buddy_system_free_pages,
    .nr_free_pages = buddy_system_nr_free_pages,
    .check = buddy_system_check,
};
