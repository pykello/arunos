#include <kalloc.h>

#define CHECK(condition) do { if (!(condition)) return __LINE__; } while (0)
static char pages[5 * 4096] __attribute__((aligned(4096)));

int main(void)
{
	uint32_t start = (uint32_t)(unsigned long)pages;
	void *chunk, *page, *allocated[3];
	int i, j;
	kalloc_init(start, start + 3 * 4096);
	page = kalloc();
	chunk = kalloc1k();
	CHECK(get_free_memory_size() == 4096 + 3072);
	kalloc_init(start + 3 * 4096, start + sizeof(pages));
	CHECK(get_free_memory_size() == 3 * 4096 + 3072);
	for (i = 0; i < 3; i++) {
		allocated[i] = kalloc();
		CHECK(allocated[i] && allocated[i] != page);
		for (j = 0; j < i; j++) CHECK(allocated[j] != allocated[i]);
	}
	CHECK(!kalloc());
	for (i = 0; i < 3; i++) CHECK(kalloc1k());
	CHECK(!kalloc1k());
	kfree(page); kfree1k(chunk);
	CHECK(get_free_memory_size() == 4096 + 1024);
	CHECK(kalloc() == page && kalloc1k() == chunk);
	return 0;
}
