#include <kalloc.h>
#include <vm.h>
#include <types.h>

static struct PageList *page_list_prepend(struct PageList *page_list,
					  char *page_address);

/*
 * List of pages that are free to be allocated by kalloc. Each of the
 * list nodes are stored in the beginning of the actual page, because
 * the page is free and we can use it for our purposes.
 */
static struct PageList *free_list_4k = NULL;
static struct PageList *free_list_1k = NULL;

/* kalloc_init adds the given address range to the free list. */
void kalloc_init(uint32_t start, uint32_t end)
{
	char *start_address = (char *) ROUND_UP(start, PAGE_SIZE);
	char *end_address = (char *) ROUND_DOWN(end, PAGE_SIZE);
	char *current_page = NULL;

	free_list_4k = NULL;
	free_list_1k = NULL;

	/* add each of the pages to the free list */
	for (current_page = start_address; current_page != end_address;
	     current_page += PAGE_SIZE)
	{
		free_list_4k = page_list_prepend(free_list_4k, current_page);
	}
}

/* kalloc allocates and returns a single page. */
void *kalloc()
{
	void *result = NULL;

	if (free_list_4k != NULL) {
		result = free_list_4k;
		free_list_4k = free_list_4k->next;
	}

	return result;
}

/* kfree adds the given page to the 4k free list. */
void kfree(void *page)
{
	free_list_4k = page_list_prepend(free_list_4k, page);
}

/* kalloc1k allocates 1k sized and aligned chuncks of memory. */
void *kalloc1k()
{
	void *result = NULL;

	/*
	 * if we don't have any free 1k chunks, convert a 4k page into four
	 * 1k chunks.
	 */
	if (free_list_1k == NULL) {
		char *page = kalloc();
		if (page != NULL) {
			kfree1k(page);
			kfree1k(page + 1024);
			kfree1k(page + 2048);
			kfree1k(page + 3072);
		}
	}

	if (free_list_1k != NULL) {
		result = free_list_1k;
		free_list_1k = free_list_1k->next;
	}

	return result;
}

/* kfree1k adds the given chunk to the 1k free list. */
void kfree1k(void *page)
{
	free_list_1k = page_list_prepend(free_list_1k, page);
}

/*
 * get_free_memory_size returns total amount of free memory that can be allocated
 * by kalloc and kalloc1k.
 */
uint32_t get_free_memory_size(void) {
	uint32_t result = 0;
	struct PageList *current_page = NULL;

	/* iterate over free 1k pages */
	current_page = free_list_1k;
	while (current_page != NULL) {
		result += 1024;
		current_page = current_page->next;
	}

	/* iterate over free 4k pages */
	current_page = free_list_4k;
	while (current_page != NULL) {
		result += 4096;
		current_page = current_page->next;
	}

	return result;
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
