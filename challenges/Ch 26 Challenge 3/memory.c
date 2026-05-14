#include <stdlib.h>
#include <string.h> // <- memcpy

#include "compiler.h"
#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    // vm.bytesAllocated += newSize - oldSize;
    // if (newSize > oldSize) {
    // #ifdef DEBUG_STRESS_GC
    //     collectGarbage();
    // #endif
    //
    //     if (vm.bytesAllocated > vm.nextGC) {
    //         collectGarbage();
    //     }
    // }
    //
    // if (newSize == 0) {
    //     free(pointer);
    //     return NULL;
    // }
    //
    // void* result = realloc(pointer, newSize);
    // if (result == NULL) exit(1); // Failsafe for if there isn’t enough memory (realloc() returns NULL)
    // return result;

    if (newSize == 0) {
        return NULL;
    }

    // Check for garbage collection (if the next allocation pushes freePtr out of bounds)
    if ((uint8_t*)vm.freePtr + newSize > (uint8_t*)vm.fromSpace + vm.spaceSize)
    {
        collectGarbage();
        // STILL don't have enough room after a full GC -> the heap is full
        if ((uint8_t*)vm.freePtr + newSize > (uint8_t*)vm.fromSpace + vm.spaceSize) {
            printf("Out of memory.\n");
            exit(1);
        }
    }
    // Grab current freePtr as our result -> advance freePtr
    void* result = vm.freePtr;
    vm.freePtr = (uint8_t*)vm.freePtr + newSize;

    // If oldSize > 0 -> VM is trying to resize an existing array (like a hash table); copy the old data over to new block
    if (oldSize > 0 && pointer != NULL) {
        size_t copySize = oldSize < newSize ? oldSize : newSize;
        memcpy(result, pointer, copySize);
    }

    return result;
}

/* NOT NEEDED (USED FOR MARK-SWEEP)
void markObject(Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;

#ifdef DEBUG_LOG_GC
    printf("%p mark ", (void*)object);
    printValue(OBJ_VAL(object));
    printf("\n");
#endif

    object->isMarked = true;


    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        vm.grayStack = (Obj**)realloc(vm.grayStack,
                                      sizeof(Obj*) * vm.grayCapacity);

        if (vm.grayStack == NULL) exit(1);
    }

    vm.grayStack[vm.grayCount++] = object;
}

void markValue(Value value) {
    if (IS_OBJ(value)) markObject(AS_OBJ(value));
}

static void markArray(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        markValue(array->values[i]);
    }
}

static void blackenObject(Obj* object) {
#ifdef DEBUG_LOG_GC
    printf("%p blacken ", (void*)object);
    printValue(OBJ_VAL(object));
    printf("\n");
#endif

    switch (object->type) {
    case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            markObject((Obj*)closure->function);
            for (int i = 0; i < closure->upvalueCount; i++) {
                markObject((Obj*)closure->upvalues[i]);
            }
            break;
    }
    case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            markObject((Obj*)function->name);
            markArray(&function->chunk.constants);
            break;
    }
    case OBJ_UPVALUE:
        markValue(((ObjUpvalue*)object)->closed);
        break;
    case OBJ_NATIVE:
    case OBJ_STRING:
        break;
    }
}

static void freeObject(Obj* object) {
#ifdef DEBUG_LOG_GC
    printf("%p free type %d\n", (void*)object, object->type);
#endif

    switch (object->type) {
        case OBJ_CLOSURE: {
                ObjClosure* closure = (ObjClosure*)object;
                FREE_ARRAY(ObjUpvalue*, closure->upvalues,
                           closure->upvalueCount);
                FREE(ObjClosure, object);
                break;
        }
        case OBJ_FUNCTION: {
                ObjFunction* function = (ObjFunction*)object;
                freeChunk(&function->chunk);
                FREE(ObjFunction, object);
                break;
        }
        case OBJ_NATIVE:
                FREE(ObjNative, object);
                break;
        case OBJ_STRING: {
                ObjString* string = (ObjString*)object;
                FREE_ARRAY(char, string->chars, string->length + 1);
                FREE(ObjString, object);
                break;
        }
        case OBJ_UPVALUE:
            FREE(ObjUpvalue, object);
            break;
    }
}

static void markRoots() {
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }

    // CallFrame pointers
    for (int i = 0; i < vm.frameCount; i++) {
        markObject((Obj*)vm.frames[i].closure);
    }

    // Upvalue list
    for (ObjUpvalue* upvalue = vm.openUpvalues;
       upvalue != NULL;
       upvalue = upvalue->next) {
        markObject((Obj*)upvalue);
    }

    markTable(&vm.globals);
    markCompilerRoots();
}

static void traceReferences() {
    while (vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        blackenObject(object);
    }
}

// Reclaim "white" (unmarked) objects
static void sweep() {
    Obj* previous = NULL;
    Obj* object = vm.objects;
    while (object != NULL) {
        if (object->isMarked) {
            object->isMarked = false;
            previous = object;
            object = object->next;
        } else {
            Obj* unreached = object;
            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                vm.objects = object;
            }

            freeObject(unreached);
        }
    }
}
*/

// Helper
size_t getObjectSize(Obj* object) {
    switch (object->type) {
    case OBJ_CLOSURE:      return sizeof(ObjClosure);
    case OBJ_FUNCTION:     return sizeof(ObjFunction);
    case OBJ_NATIVE:       return sizeof(ObjNative);
    case OBJ_UPVALUE:      return sizeof(ObjUpvalue);
    case OBJ_STRING:       return sizeof(ObjString);
    }
    return 0; // Unreachable
}

void copyObject(Obj** objectPtr) {
    Obj* object = *objectPtr;
    if (object == NULL) return;

    // If it already has a forwarding address -> another reference already copied it -> Update THIS pointer to the new address
    if (object->forwardingAddress != NULL) {
        *objectPtr = object->forwardingAddress;
        return;
    }

    // Not copied yet -> Find size
    size_t size = getObjectSize(object);

    // Copy size to To-Space free pointer
    Obj* newObject = (Obj*)vm.freePtr;
    memcpy(newObject, object, size);

    // Advance the bump allocator
    vm.freePtr = (uint8_t*)vm.freePtr + size;

    // Leave a forwarding address in OLD object memory slot
    object->forwardingAddress = newObject;

    // Update caller's pointer to point to the To-Space
    *objectPtr = newObject;
}

void copyValue(Value* value) {
    if (IS_OBJ(*value)) {
        copyObject((Obj**)&value->as.obj);
    }
}

void copyRoots() {
    // Copy local variables and temps on stack
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        copyValue(slot);
    }
    // Copy closures
    for (int i = 0; i < vm.frameCount; i++) {
        copyObject((Obj**)&vm.frames[i].closure);
    }

    // Copy open upvalues
    for (ObjUpvalue* upvalue = vm.openUpvalues;
         upvalue != NULL;
         upvalue = upvalue->next) {
        copyObject((Obj**)&upvalue);
         }

    // Copy global variables
    for (int i = 0; i < vm.globals.capacity; i++) {
        Entry* entry = &vm.globals.entries[i];
        if (entry->key != NULL) {
            copyObject((Obj**)&entry->key);
            copyValue(&entry->value);
        }
    }
}

void scanToSpace() {
    // Scan from beginning of To-Space
    uint8_t* scanPtr = (uint8_t*)vm.toSpace;

    // Scan to bump allocator
    while (scanPtr < (uint8_t*)vm.freePtr) {
        Obj* object = (Obj*)scanPtr;

        // Copy internal references (based on Obj type) + extend freePtr accordingly
        switch (object->type) {
            case OBJ_CLOSURE: {
                ObjClosure* closure = (ObjClosure*)object;
                copyObject((Obj**)&closure->function);
                for (int i = 0; i < closure->upvalueCount; i++) {
                    copyObject((Obj**)&closure->upvalues[i]);
                }
                break;
            }
            case OBJ_FUNCTION: {
                ObjFunction* function = (ObjFunction*)object;
                copyObject((Obj**)&function->name);
                // Copy the constants array inside the chunk
                for (int i = 0; i < function->chunk.constants.count; i++) {
                    copyValue(&function->chunk.constants.values[i]);
                }
                break;
            }
            case OBJ_STRING:
            case OBJ_NATIVE:
                break;
        }

        // Advance scanPtr by the size of object
        scanPtr += getObjectSize(object);
    }
}

void updateStringTable(Table* table) {
    for (int i = 0; i < table->capacity; i++) {
        Entry* entry = &table->entries[i];
        if (entry->key != NULL) {
            // Check if the string was moved to To-Space
            if (entry->key->obj.forwardingAddress != NULL) { // If survived -> update hash table pointer to new address
                entry->key = (ObjString*)entry->key->obj.forwardingAddress;
            } else { // Did not survive -> delete from string table (tombstone)
                entry->key = NULL;
                entry->value = BOOL_VAL(true);
            }
        }
    }
}

void collectGarbage() {
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    // size_t before = vm.bytesAllocated;
    size_t before = (uint8_t*)vm.freePtr - (uint8_t*)vm.fromSpace;
#endif

    vm.freePtr = vm.toSpace;
    copyRoots();
    scanToSpace();
    updateStringTable(&vm.strings);

    // Flip (Wipe old data)
    void* temp = vm.fromSpace;
    vm.fromSpace = vm.toSpace;
    vm.toSpace = temp;

    // markRoots();
    // traceReferences();
    // tableRemoveWhite(&vm.strings);
    // sweep();
    // vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC

    size_t after = (uint8_t*)vm.freePtr - (uint8_t*)vm.fromSpace;
    printf("-- gc end\n");
    printf("   collected %zu bytes (from %zu to %zu)\n",
         before - after, before, after);

    // printf("   collected %zu bytes (from %zu to %zu) next at %zu\n",
    //      before - vm.bytesAllocated, before, vm.bytesAllocated,
    //      vm.nextGC);
#endif
}

void freeObjects() {
    // Obj* object = vm.objects;
    // while (object != NULL) {
    //     Obj* next = object->next;
    //     freeObject(object);
    //     object = next;
    // }
    //
    // free(vm.grayStack);

    // ONLY NEED TO FREE 2 HALVES
    free(vm.fromSpace);
    free(vm.toSpace);

    vm.fromSpace = NULL;
    vm.toSpace = NULL;
}