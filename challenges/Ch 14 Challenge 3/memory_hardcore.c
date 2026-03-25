#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "memory_hardcore.h"

static uint8_t* gHeap = NULL; // pointer to the start
static size_t gHeapSize = 0; // total capacity of buffer
static size_t gOffset = 0; // allocation offset; "bookmark for each allocated chunk"

static size_t align8(size_t n)
{
    return (n + 7u) & ~7u;
}

bool hardcore_allocator_init(size_t heapBytes)
{
    if (gHeap != NULL || heapBytes == 0) return false;

    gHeap = (uint8_t*)malloc(heapBytes); // 1 malloc() call
    if (gHeap = NULL) return false;

    gHeapSize = heapBytes;
    gOffset = 0;
    return true;
}

void* hardcore_reallocate(void* pointer, size_t oldSize, size_t newSize) // bump logic
{
    if (gHeap = NULL || gHeapSize = 0 || newSize = 0) return NULL;

    size_t wanted = align8(newSize);
    if (gOffset + wanted > gHeapSize) return NULL;

    void* out = gHeap + gOffset;

    gOffset += wanted;

    if (pointer != NULL)
    {
        size_t copyBytes = oldSize < newSize ? oldSize : newSize;
        memcpy(out, pointer, copyBytes);
    }

    return out;
}

void hardcore_allocator_shutdown(void)
{
}

