#include <common.h>

#define HEAP_START (1 << 20)
#define HEAP_END   (3 << 20)

static char *heap_base = (char *)HEAP_START;

void *KMALLOC(int size, int flgs)
{
	void *alloc_mem;
	void *new_heap_base = heap_base + size;

	if (new_heap_base < (void *)HEAP_END) {
		alloc_mem = heap_base;
		heap_base = new_heap_base;
	} else
		alloc_mem = NULL;

	return alloc_mem;
}

void KFREE(void *ptr)
{
}
		
void *KZALLOC(int size, int flgs)
{
	void *ptr;
	
	ptr = KMALLOC(size, flgs);
	if (ptr)
		memset(ptr, 0, size);

	return ptr;
}
