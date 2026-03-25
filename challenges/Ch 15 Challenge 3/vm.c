#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "vm.h"

#include "../../clox/memory.h"

VM vm;

static void resetStack() {
    vm.stackCount = 0;
}

void initVM() {
    vm.stack = NULL;
    vm.stackCapacity = 0;
    resetStack();
}

void freeVM() {
    // Not yet implemented
}

void push(Value value) {
    if (vm.stackCapacity < vm.stackCount + 1) { // Check for possible overflow
        int oldCapacity = vm.stackCapacity;
        vm.stackCapacity = GROW_CAPACITY(oldCapacity);
        vm.stack = GROW_ARRAY(Value, vm.stack,
                              oldCapacity, vm.stackCapacity);
    }

    vm.stack[vm.stackCount] = value;
    vm.stackCount++;
}

Value pop() {
    vm.stackCount--;
    return vm.stack[vm.stackCount];
}


static InterpretResult run() {
#define READ_BYTE() (*vm.ip++) // reads the byte currently pointed at by ip and then advances the instruction pointer
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    do { \
    double b = pop(); \
    double a = pop(); \
    push(a op b); \
    } while (false)

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        for (Value *slot = vm.stack; slot < vm.stack + vm.stackCount; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
#endif
        // disassembles and prints each instruction right before executing it

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                    Value constant = READ_CONSTANT();
                    push(constant);
                    printValue(constant);
                    printf("\n");
                    break;
            }
            case OP_ADD:      BINARY_OP(+); break;
            case OP_SUBTRACT: BINARY_OP(-); break;
            case OP_MULTIPLY: BINARY_OP(*); break;
            case OP_DIVIDE:   BINARY_OP(/); break;
            case OP_NEGATE:   push(-pop()); break;
            case OP_RETURN: {
                    printValue(pop());
                    printf("\n");
                    return INTERPRET_OK; // temporarily repurposed to end the execution (will be used to return the current Lox function)
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}