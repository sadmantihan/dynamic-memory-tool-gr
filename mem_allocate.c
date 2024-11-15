#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structures
typedef struct MemoryBlock {
    char block_status, process_status;
    char process_code[10];
    int start_address, size;
    struct MemoryBlock *next, *previous;
} MemoryBlock;

typedef struct {
    MemoryBlock *head;
    int total_processes;
} MemoryList;

typedef struct Process {
    char code[10];
    char allocation_status, waiting_status;
    int arrival_date, execution_time, memory_required;
    struct Process *next, *previous;
} Process;

typedef struct {
    Process *head, *tail;
    int total_to_allocate, total_waiting;
} ProcessList;

typedef struct CommentNode {
    char new_process[400], waiting_process[400], completed_process[400];
    char selected_process[400], ready_process[400];
    int timestamp;
    struct CommentNode *next;
} CommentNode;

typedef struct {
    CommentNode *head;
} CommentList;

// Functions to initialize structures
void initialize_memory(MemoryList *memory, int total_memory_size) {
    MemoryBlock *initial_block = malloc(sizeof(MemoryBlock));
    if (!initial_block) {
        printf("Error: Unable to allocate initial memory block!\n");
        exit(EXIT_FAILURE);
    }
    memory->head = initial_block;
    memory->total_processes = 0;
    *initial_block = (MemoryBlock) {'f', '\0', "", 0, total_memory_size, NULL, NULL};
}

void initialize_comments(CommentList *comments) {
    comments->head = NULL;
}

void initialize_processes(ProcessList *process_list) {
    process_list->head = NULL;
    process_list->tail = NULL;
    process_list->total_to_allocate = 0;
    process_list->total_waiting = 0;
}

// Linux-compatible function to set console cursor position
void set_console_cursor(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape code for cursor positioning
}

// Function to validate file format
int validate_file_format(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open the file '%s'!\n", filename);
        return 0;
    }

    char line[100];
    int line_count = 0, semicolon_count = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0) {
            printf("Error: Avoid empty lines in the file!\n");
            fclose(file);
            return 0;
        }
        line_count++;
        for (char *c = line; *c; c++) {
            if (*c == ';') semicolon_count++;
        }
    }
    fclose(file);

    if (line_count == 0) {
        printf("Error: The file is empty!\n");
        return 0;
    }
    if (semicolon_count != 4 * line_count) {
        printf("Error: Semicolons are misplaced in the file!\n");
        return 0;
    }
    return 1;
}

// Function to add a comment
void add_comment(CommentList *comments, int timestamp, const char *message, int type) {
    CommentNode *current = comments->head;
    CommentNode *new_node = malloc(sizeof(CommentNode));
    if (!new_node) {
        printf("Error: Memory allocation failed for comments!\n");
        return;
    }

    *new_node = (CommentNode) { "none", "none", "none", "none", "none", timestamp, NULL };
    if (!current) {
        comments->head = new_node;
    } else {
        while (current->next && current->timestamp != timestamp) {
            current = current->next;
        }
        if (current->timestamp == timestamp) {
            free(new_node);
            new_node = current; // Use existing node for the same timestamp
        } else {
            current->next = new_node;
        }
    }

    switch (type) {
        case 1: snprintf(new_node->new_process, sizeof(new_node->new_process), "New Process: %s loaded", message); break;
        case 2: snprintf(new_node->waiting_process, sizeof(new_node->waiting_process), "Waiting Process: %s on hold", message); break;
        case 3: snprintf(new_node->completed_process, sizeof(new_node->completed_process), "Completed Process: %s finished", message); break;
        case 4: snprintf(new_node->selected_process, sizeof(new_node->selected_process), "Selected Process: %s elected", message); break;
        case 5: snprintf(new_node->ready_process, sizeof(new_node->ready_process), "Ready Process: %s ready", message); break;
    }
}

// Function to print the header for processes
void print_process_header(void) {
    printf("      ╔════════╦══════════╦════════════╦══════════╗\n");
    printf("      ║ Process║ Arr. Date║ Mem Space  ║ Exec Time║\n");
    printf("      ╠════════╬══════════╬════════════╬══════════╣\n");
}

// Function to load processes from a file
void load_processes_from_file(ProcessList *process_list, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open the file '%s'!\n", filename);
        return;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        Process *new_process = malloc(sizeof(Process));
        if (!new_process) {
            printf("Error: Memory allocation failed for process!\n");
            fclose(file);
            return;
        }

        sscanf(line, "%[^;];%d;%d;%d", new_process->code, &new_process->arrival_date, 
               &new_process->memory_required, &new_process->execution_time);

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
    }
    fclose(file);
}

// Function to print comments
void print_comments(const CommentList *comments) {
    const CommentNode *current = comments->head;
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║                  Comments Log                  ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    while (current) {
        printf("Timestamp %d:\n", current->timestamp);
        if (strcmp(current->new_process, "none")) printf("  %s\n", current->new_process);
        if (strcmp(current->waiting_process, "none")) printf("  %s\n", current->waiting_process);
        if (strcmp(current->completed_process, "none")) printf("  %s\n", current->completed_process);
        if (strcmp(current->selected_process, "none")) printf("  %s\n", current->selected_process);
        if (strcmp(current->ready_process, "none")) printf("  %s\n", current->ready_process);
        printf("\n");
        current = current->next;
    }
}

// Function to display processes
void display_processes(const ProcessList *process_list) {
    const Process *current = process_list->head;
    print_process_header();
    while (current) {
        printf("      ║ %-8s║ %-10d║ %-11d║ %-10d║\n", 
               current->code, current->arrival_date, 
               current->memory_required, current->execution_time);
        current = current->next;
    }
    printf("      ╚════════╩══════════╩════════════╩══════════╝\n");
}

// Main function
int main() {
    MemoryList memory;
    ProcessList process_list;
    CommentList comments;

    initialize_memory(&memory, 1024);
    initialize_processes(&process_list);
    initialize_comments(&comments);

    load_processes_from_file(&process_list, "process.txt");
    add_comment(&comments, 1, "Process1", 1);
    add_comment(&comments, 2, "Process2", 2);
    add_comment(&comments, 3, "Process3", 3);

    printf("\nProcesses Loaded:\n");
    display_processes(&process_list);

    printf("\nComments:\n");
    print_comments(&comments);

    return 0;
}
