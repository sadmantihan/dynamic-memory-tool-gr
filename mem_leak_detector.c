#include "mem_leak_detector.h"

// Global pointer to track allocated memory
MemoryBlock *allocated_memory = NULL;

// Function to add a memory block to the tracking list
void add_to_tracked_memory(void *ptr, size_t size) {
    MemoryBlock *new_block = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    if (!new_block) {
        printf("Error: Unable to allocate memory for tracking.\n");
        return;
    }
    new_block->ptr = ptr;
    new_block->size = size;
    new_block->next = allocated_memory;
    allocated_memory = new_block;
}

// Function to free all tracked memory blocks
void free_tracked_memory() {
    MemoryBlock *current = allocated_memory;
    while (current) {
        free(current->ptr); // Free allocated memory
        MemoryBlock *temp = current;
        current = current->next;
        free(temp); // Free tracking node
    }
    allocated_memory = NULL;
}

// Function to display memory leaks in the console
void display_memory_leaks() {
    MemoryBlock *current = allocated_memory;
    if (!current) {
        printf("\nNo memory leaks detected. All memory has been freed.\n");
        return;
    }
    printf("\n--- Memory Leaks Detected ---\n");
    printf("Address\t\tSize (Bytes)\n");
    printf("----------------------------\n");
    while (current) {
        printf("%p\t%zu\n", current->ptr, current->size);
        current = current->next;
    }
    printf("----------------------------\n");
    printf("Please ensure all allocated memory is properly freed.\n");
}

// Function to allocate memory dynamically and track it
void allocate_memory(size_t size) {
    void *data = malloc(size);
    if (data == NULL) {
        printf("Error: Memory allocation failed.\n");
        return;
    }
    add_to_tracked_memory(data, size);
    printf("Memory allocated at %p (Size: %zu bytes)\n", data, size);
}

// Main function
int main() {
    size_t size;
    int choice;

    while (1) {
        // Display the menu
        printf("\n╔══════════════════════════════════════════════╗\n");
        printf("║              Memory Leak Detector            ║\n");
        printf("╠══════════════════════════════════════════════╣\n");
        printf("║ 1. Allocate memory                           ║\n");
        printf("║ 2. Display memory leaks                      ║\n");
        printf("║ 3. Free all allocated memory                 ║\n");
        printf("║ 4. Exit                                      ║\n");
        printf("╚══════════════════════════════════════════════╝\n");

        printf("Select an option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter size of memory to allocate (in bytes): ");
                if (scanf("%zu", &size) != 1 || size <= 0) {
                    printf("Invalid size. Please enter a positive integer.\n");
                    while (getchar() != '\n'); // Clear invalid input
                    break;
                }
                allocate_memory(size);
                break;

            case 2:
                display_memory_leaks();
                break;

            case 3:
                printf("Freeing all tracked memory...\n");
                free_tracked_memory();
                printf("All allocated memory has been freed.\n");
                break;

            case 4:
                printf("Exiting program...\n");
                display_memory_leaks();
                free_tracked_memory();
                return 0;

            default:
                printf("Invalid option. Please try again.\n");
                break;
        }
    }

    return 0;
}
