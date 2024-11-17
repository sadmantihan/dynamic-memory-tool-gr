#include "mem_allocate.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Initialize the memory list with the given total memory size
void initialize_memory(MemoryList *memory, int total_memory_size) {
    memory->head = NULL;
    memory->total_processes = 0;

    // Create a single memory block that represents the whole memory
    MemoryBlock *block = (MemoryBlock *)malloc(sizeof(MemoryBlock));
    if (block == NULL) {
        fprintf(stderr, "Memory allocation failed for memory block.\n");
        exit(1);
    }

    block->start_address = 0;
    block->size = total_memory_size;
    block->block_status = 'f';  // 'f' for Free
    block->process_status = 'N';  // 'N' for None (no process allocated)
    block->process_code[0] = '\0';  // No process code
    block->next = NULL;
    block->previous = NULL;

    // Set the memory list head to the created block
    memory->head = block;
}

// Initialize the process list
void initialize_processes(ProcessList *process_list) {
    process_list->head = NULL;
    process_list->tail = NULL;
    process_list->total_to_allocate = 0;
    process_list->total_waiting = 0;
}

// Function to allocate memory using First-Fit strategy
void implement_first_fit(MemoryList *memory, Process *process) {
    MemoryBlock *current = memory->head;
    while (current) {
        if (current->block_status == 'f' && current->size >= process->memory_required) {
            // Split block if excess memory is available
            if (current->size > process->memory_required) {
                MemoryBlock *new_block = malloc(sizeof(MemoryBlock));
                if (!new_block) {
                    printf("Error: Memory allocation failed!\n");
                    return;
                }
                *new_block = (MemoryBlock){'f', '\0', "", current->start_address + process->memory_required,
                                           current->size - process->memory_required, current->next, current};
                if (current->next) {
                    current->next->previous = new_block;
                }
                current->next = new_block;
                current->size = process->memory_required;
            }
            // Assign process to current block
            current->block_status = 'a';  // 'a' for allocated
            strcpy(current->process_code, process->code);
            memory->total_processes++;
            printf("Process %s allocated at address %d\n", process->code, current->start_address);
            return;
        }
        current = current->next;
    }
    printf("Error: No suitable block found for process %s\n", process->code);
}

// Function to allocate memory using Best-Fit strategy
void implement_best_fit(MemoryList *memory, Process *process) {
    MemoryBlock *current = memory->head;
    MemoryBlock *best_fit = NULL;
    while (current) {
        if (current->block_status == 'f' && current->size >= process->memory_required) {
            if (!best_fit || current->size < best_fit->size) {
                best_fit = current;
            }
        }
        current = current->next;
    }
    if (best_fit) {
        // Split the best fit block if necessary
        if (best_fit->size > process->memory_required) {
            MemoryBlock *new_block = malloc(sizeof(MemoryBlock));
            if (!new_block) {
                printf("Error: Memory allocation failed!\n");
                return;
            }
            *new_block = (MemoryBlock){'f', '\0', "", best_fit->start_address + process->memory_required,
                                       best_fit->size - process->memory_required, best_fit->next, best_fit};
            if (best_fit->next) {
                best_fit->next->previous = new_block;
            }
            best_fit->next = new_block;
            best_fit->size = process->memory_required;
        }
        // Assign process to the best fit block
        best_fit->block_status = 'a';  // 'a' for allocated
        strcpy(best_fit->process_code, process->code);
        printf("Process %s allocated at address %d\n", process->code, best_fit->start_address);
    } else {
        printf("Error: No suitable block found for process %s\n", process->code);
    }
}

// Function to display the memory map
void display_memory_map(const MemoryList *memory) {
    MemoryBlock *current = memory->head;
    printf("\nMemory Map:\n");
    printf("╔═══════╦═════════╦════════════════╦════════════╗\n");
    printf("║ Start ║   Size  ║ Block Status   ║ Process ID ║\n");
    printf("╠═══════╬═════════╬════════════════╬════════════╣\n");
    while (current) {
        printf("║ %-5d ║ %-7d ║ %-14c ║ %-10s ║\n", current->start_address, current->size,
               current->block_status, current->process_code);
        current = current->next;
    }
    printf("╚═══════╩═════════╩════════════════╩════════════╝\n");
}

// Function to add a new process interactively
void add_process_interactively(ProcessList *process_list) {
    Process *new_process = malloc(sizeof(Process));
    if (!new_process) {
        printf("Error: Memory allocation failed for new process!\n");
        return;
    }
    printf("\nEnter Process Details:\n");
    printf("Process Code: ");
    scanf("%s", new_process->code);
    printf("Arrival Time: ");
    scanf("%d", &new_process->arrival_date);
    printf("Memory Required: ");
    scanf("%d", &new_process->memory_required);
    printf("Execution Time: ");
    scanf("%d", &new_process->execution_time);

    new_process->allocation_status = 'N';
    new_process->waiting_status = 'N';
    new_process->next = NULL;
    new_process->previous = process_list->tail;

    if (process_list->tail) {
        process_list->tail->next = new_process;
    } else {
        process_list->head = new_process;
    }
    process_list->tail = new_process;
    process_list->total_to_allocate++;

    printf("Process %s added successfully!\n", new_process->code);
}

// Main menu function
void menu() {
    printf("\n╔══════════════════════════════╗\n");
    printf("║       Memory Management      ║\n");
    printf("╠══════════════════════════════╣\n");
    printf("║ 1. Display Memory Map        ║\n");
    printf("║ 2. Add a New Process         ║\n");
    printf("║ 3. Allocate Memory (First-Fit)║\n");
    printf("║ 4. Allocate Memory (Best-Fit)║\n");
    printf("║ 5. Exit                      ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("Enter your choice: ");
}

int main() {
    MemoryList memory;
    ProcessList process_list;

    // Initialize structures
    initialize_memory(&memory, 1024); // Total memory size = 1024
    initialize_processes(&process_list);

    int choice;
    do {
        menu();
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                display_memory_map(&memory);
                break;
            case 2:
                add_process_interactively(&process_list);
                break;
            case 3: {
                Process *current = process_list.head;
                while (current) {
                    if (current->allocation_status == 'N') {
                        implement_first_fit(&memory, current);
                        current->allocation_status = 'Y';
                        display_memory_map(&memory); // Show memory map after allocation
                    }
                    current = current->next;
                }
                break;
            }
            case 4: {
                Process *current = process_list.head;
                while (current) {
                    if (current->allocation_status == 'N') {
                        implement_best_fit(&memory, current);
                        current->allocation_status = 'Y';
                        display_memory_map(&memory); // Show memory map after allocation
                    }
                    current = current->next;
                }
                break;
            }
            case 5:
                printf("Exiting program...\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while (choice != 5);

    return 0;
}
