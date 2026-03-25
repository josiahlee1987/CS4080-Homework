#include <stdlib.h>

#include "chunk.h"
#include "memory.h"

void initChunk(Chunk* chunk) {
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;

    // for RLE
    chunk->rle = NULL;
    chunk->lineCount = 0;
    chunk->lineCapacity = 0;

    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(Lines, chunk->rle, chunk->lineCapacity); // instead of FREE_ARRAY(int, chunk->lines, chunk->capacity);
    initChunk(chunk);
    freeValueArray(&chunk->constants);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line)
{
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
        // Shouldn't grow the line array when we grow the code array -> Sizes are not proportionate anymore
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;

    // Check for same line -> return
    if (chunk->lineCount > 0 && chunk->rle[chunk->lineCount - 1].line == line) {
        return;
    }

    // If outgrows the capacity -> new line
    if (chunk->lineCapacity < chunk->lineCount + 1) {
        int oldCapacity = chunk->lineCapacity;
        chunk->lineCapacity = GROW_CAPACITY(oldCapacity);
        chunk->rle = GROW_ARRAY(Lines, chunk->rle, oldCapacity, chunk->lineCapacity);
    }

    // New RLE struct storage
    Lines* lineStart = &chunk->rle[chunk->lineCount++];
    lineStart->offset = chunk->count - 1;
    lineStart->line = line;
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}


int getLine(Chunk* chunk, int instruction) { // index of instruction = chunk + offset

    // Use binary search for more efficient line search up ( O(log n) < O(n) )
    int start = 0;
    int end = chunk->lineCount - 1;

    for (;;) { // infinite loop
        int mid = (start + end) / 2;
        Lines* line = &chunk->rle[mid];
        if (instruction < line->offset) {
            end = mid - 1;
        } else if (mid == chunk->lineCount - 1 ||
            instruction < chunk->rle[mid + 1].offset) {
            return line->line;
            } else {
                start = mid + 1;
            }
    }
}