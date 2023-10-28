
extern "C"
{
#include "memory_alloc.h"
}

int main()
{

	uint8_t* ptr1 = (uint8_t*)memory_alloc_init(5);
	uint8_t* ptr2 = (uint8_t*)memory_alloc_init(20000);
	memory_alloc_free(ptr1);
	uint8_t* ptr3 = (uint8_t*)memory_alloc_init(20000);

	memory_alloc_consolidate();

	for (int i = 0; i < 5; i++)
	{
		ptr1[i] = i;
		ptr2[i] = i;
	}

	memory_alloc_free(ptr2);
	ptr2 = (uint8_t*)memory_alloc_init(20000);

	return 0;
}