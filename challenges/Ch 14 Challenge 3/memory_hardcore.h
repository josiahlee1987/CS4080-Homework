#ifndef clox_memory_hardcore_h
#define clox_memory_hardcore_h

#include "common.h"

#endif

#define HARDCORE_GROW_CAPACITY(capacity) \
((capacity) < 8 ? 8 : (capacity) * 2)

#define HARDCORE_GROW_ARRAY(type, pointer, oldCount, newCount) \
(type*)hardcore_reallocate(pointer, sizeof(type) * (oldCount), \
sizeof(type) * (newCount))

#define HARDCORE_FREE_ARRAY(type, pointer, oldCount) \
hardcore_reallocate(pointer, sizeof(type) * (oldCount), 0)

bool hardcore_allocator_init(size_t heapBytes);

void hardcore_allocator_shutdown(void);

void* hardcore_reallocate(void* pointer, size_t oldSize, size_t newSize);