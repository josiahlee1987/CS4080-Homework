#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_RETURN,
  } OpCode;

typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    ValueArray constants;

    Lines* rle; //array of Line* to store the compressed RLE format
    // separate count & capacity needed due to size mismatch
    int lineCount;
    int  lineCapacity;
} Chunk;

typedef struct {
    int offset;
    int line;
} Lines;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk* chunk, int instruction);

#endif