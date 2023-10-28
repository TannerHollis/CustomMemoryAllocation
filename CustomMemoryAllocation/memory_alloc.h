/*
 * memory_alloc.h
 *
 *  Created on: Oct 28, 2023
 *      Author: Tanner
 */

#ifndef INC_MEMORY_ALLOC_H_
#define INC_MEMORY_ALLOC_H_

#include "stdint.h"

#define HEAP_MEMORY_RECORDS 10 // Maximum of ten records
#define HEAP_MEMORY_BYTES 48000 // 48KB / 4 bytes for alignment
#define WORD_SIZE 4 // Bytes (32-bit)
#define WORD_ALIGN(x) (x + (WORD_SIZE - (x % WORD_SIZE)))
#define HEAP_MEMORY (HEAP_MEMORY_BYTES / WORD_SIZE) // 48KB / 4 bytes for alignment

typedef enum malloc_record_state
{
	MALLOC_NOT_IN_USE = 0,
	MALLOC_IN_USE = 1,
} malloc_record_state;

typedef struct malloc_record
{
	malloc_record_state state;
	uint32_t* start;
	uint32_t* end;
	uint32_t size; // Bytes
} malloc_record;

void* memory_alloc_init(uint32_t size);

uint8_t memory_alloc_free(void*  ptr);

uint8_t memory_alloc_consolidate();

void memory_alloc_sort_ascending();

uint8_t check_block_collision(uint32_t* start, uint32_t* end);

#endif /* INC_MEMORY_ALLOC_H_ */
