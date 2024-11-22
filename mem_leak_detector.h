#ifndef MEMORY_LEAK_DETECTOR_H
#define MEMORY_LEAK_DETECTOR_H

#include <stdlib.h>
#include <stdio.h>

// Structure to track memory blocks
typedef struct MemoryBlock {
    void *ptr;
    size_t size;
    struct MemoryBlock *next;
} MemoryBlock;

// Function declarations
void add_to_tracked_memory(void *ptr, size_t size);
void free_tracked_memory();
void display_memory_leaks();

#endif // MEMORY_LEAK_DETECTOR_H
