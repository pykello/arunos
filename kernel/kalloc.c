#include <kalloc.h>
#include <memory.h>
#include <types.h>

static struct PageList *page_list_prepend(struct PageList *page_list,
					  char *page_address);

/*
 * List of pages that are free to be allocated by kalloc. Each of the
 * list nodes are stored in the beginning of the actual page, because
 * the page is free and we can use it for our purposes.
 */
static struct PageList *free_list = NULL;

/* kalloc_init adds the given address range to the free list. */
void kalloc_init(uint32_t start, uint32_t end)
{
	char *start_address = (char *) ROUND_UP(start, PAGE_SIZE);
	char *end_address = (char *) ROUND_DOWN(end, PAGE_SIZE);
	char *current_page = NULL;

	/* add each of the pages to the free list */
	for (current_page = start_address; current_page != end_address;
	     current_page += PAGE_SIZE)
	{
		free_list = page_list_prepend(free_list, current_page);
	}
}

/* kalloc allocates and returns a single page. */
void *kalloc()
{
	void *result = NULL;

	if (free_list != NULL) {
		result = free_list;
		free_list = free_list->next;
	}

	return result;
}

/* kfree adds the given page to the free list. */
void kfree(void *page)
{
	free_list = page_list_prepend(free_list, page);
}

/*
 * page_list_prepend adds the given page to the beginning of the page list
 * and returns the address of the new page list.
 */
static struct PageList *page_list_prepend(struct PageList *page_list,
					  char *page_address)
{
	struct PageList *page = (struct PageList *) page_address;
	page->next = page_list;

	return page;
}
