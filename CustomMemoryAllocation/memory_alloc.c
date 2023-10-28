
#include "memory_alloc.h"

malloc_record records[HEAP_MEMORY_RECORDS];
uint32_t heap[HEAP_MEMORY];

void* memory_alloc_init(uint32_t size)
{
	uint32_t aligned_size = WORD_ALIGN(size);
	malloc_record* record;
	int8_t record_index = -1;
	for(uint8_t i = 0; i < HEAP_MEMORY_RECORDS; i++)
	{
		if(records[i].state == MALLOC_IN_USE)
			continue;

		record_index = i;
		break;
	}

	if (record_index == -1)
		return (void*)0;
	else
	{
		record = &records[record_index];
		record->start = &heap[0]; // Start address
		record->end = (uint32_t)record->start + aligned_size;
		record->size = size; // Bytes
	}

	for(uint8_t i = 0; i < HEAP_MEMORY_RECORDS; i++)
	{
		int8_t block_collision = check_block_collision(record->start, record->end);

		if (record->end > &heap[HEAP_MEMORY])
			return (void*)0;

		if(block_collision == -1)
		{
			record->state = MALLOC_IN_USE;
			return (void*)record->start;
		}
		else
		{
			record->start = (uint32_t)records[block_collision].end + WORD_SIZE; // Start address
			record->end = (uint32_t)record->start + aligned_size;
		}
	}

	return (void*)0;
}

uint8_t memory_alloc_free(void* ptr)
{
	for (uint8_t i = 0; i < HEAP_MEMORY_RECORDS; i++)
	{
		if (ptr == records[i].start)
		{
			ptr = (void*)0;
			records[i].state = MALLOC_NOT_IN_USE;
			return 1;
		}
	}
	return 0;
}

void memory_alloc_consolidate()
{
	uint32_t tmp[100];

	// First, sort ascending by start address
	memory_alloc_sort_ascending();

	uint32_t* start = &heap[0];
	uint32_t offset = 0;
	for (uint8_t i = 0; i < HEAP_MEMORY_RECORDS; i++)
	{
		if (records[i].state == MALLOC_NOT_IN_USE)
			continue;

		malloc_record* record = &records[i];
		offset = (uint32_t)record->start - (uint32_t)start;
		if (offset > 0)
		{
			uint32_t words_left = record->size;
			while (words_left > 0)
			{
				uint32_t tx;
				if (words_left >= 100)
					tx = 100;
				else
					tx = words_left;

				memcpy(&tmp, record->start, sizeof(WORD_SIZE) * tx);
				memcpy(start, &tmp, sizeof(WORD_SIZE) * tx);
				words_left = words_left - tx;
			}
			uint32_t aligned_size = WORD_ALIGN(record->size);
			record->start = start;
			record->end = (uint32_t)record->start + aligned_size;
			start = (uint32_t)record->end + WORD_SIZE;
		}
	}

	return 0;
}

void memory_alloc_sort_ascending()
{
	malloc_record tmp;
	for (uint8_t i = 0; i < HEAP_MEMORY_RECORDS - 1; i++)
	{
		for (uint8_t j = 0; j < HEAP_MEMORY_RECORDS - 1; j++)
		{
			if (records[j].state == MALLOC_NOT_IN_USE || records[j + 1].state == MALLOC_NOT_IN_USE)
				continue;

			if ((uint32_t)records[j + 1].start < (uint32_t)records[j].start)
			{
				memcpy(&tmp, &records[j], sizeof(malloc_record));
				memcpy(&records[j], &records[j + 1], sizeof(malloc_record));
				memcpy(&records[j + 1], &tmp, sizeof(malloc_record));
			}
		}
	}
}

uint8_t check_block_collision(uint32_t* start, uint32_t* end)
{
	for (uint8_t i = 0; i < HEAP_MEMORY_RECORDS; i++)
	{
		if (records[i].state == MALLOC_NOT_IN_USE)
			continue;

		uint8_t left_overlap = start <= records[i].start && end >= records[i].start;
		uint8_t right_overlap = start <= records[i].end && end >= records[i].end;
		uint8_t within = start >= records[i].start && end <= records[i].end;

		if (left_overlap || right_overlap || within)
			return i;
	}

	return -1;
}