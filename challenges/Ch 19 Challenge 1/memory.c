#include <stdlib.h>

#include "memory.h"
#include "vm.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    if (result == NULL) exit(1); // Failsafe for if there isn’t enough memory (realloc() returns NULL)
    return result;
}

static void freeObject(Obj* object) {
    switch (object->type) {
        case OBJ_STRING: {
                ObjString* string = (ObjString*)object;
                reallocate(object, sizeof(ObjString) + string->length + 1, 0); // FREE_ARRAY() and FREE() -> reallocate()
                break;
        }
    }
}

void freeObjects() {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
}