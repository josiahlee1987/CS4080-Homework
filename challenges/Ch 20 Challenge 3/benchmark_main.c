#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // <- benchmarks

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include "object.h" // <- benchmarks

// For benchmarks
#define NUM_STRINGS 1000
#define LOOKUPS 10000

static void repl() {
    char line[1024];
    for (;;) {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }

        interpret(line);
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1); // + 1 to make room for the null byte
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char* path) {
  char* source = readFile(path);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]) {
    initVM();

    // ====================== LOOKUPS ======================
    // Generate 1000 (unique) string array
    char* strings[NUM_STRINGS];
    for (int i = 0; i < NUM_STRINGS; i++) {
        strings[i] = malloc(20);
        sprintf(strings[i], "benchmark_string_%d", i);
    }

    // Hash Table Benchmark
    clock_t start_hash_lookup = clock();
    for (int i = 0; i < LOOKUPS; i++) {
        for (int j = 0; j < NUM_STRINGS; j++) {
            copyString(strings[j], strlen(strings[j]));
        }
    }
    clock_t end_hash = clock();
    printf("Hash table time: %f seconds\n", (double)(end_hash - start_hash_lookup) / CLOCKS_PER_SEC);

    // Linear Search Benchmark
    clock_t start_linear = clock();
    for (int i = 0; i < LOOKUPS; i++) {
        for (int j = 0; j < NUM_STRINGS; j++) {
            for (int k = 0; k < NUM_STRINGS; k++) {
                if (strcmp(strings[k], strings[j]) == 0) {
                    break;
                }
            }
        }
    }
    clock_t end_linear = clock();
    printf("Linear search time: %f seconds\n", (double)(end_linear - start_linear) / CLOCKS_PER_SEC);

    freeVM();
    return 0;
}

// if (argc == 1) {
//     repl();
// } else if (argc == 2) {
//     runFile(argv[1]);
// } else {
//     fprintf(stderr, "Usage: clox [path]\n");
//     exit(64);
// }