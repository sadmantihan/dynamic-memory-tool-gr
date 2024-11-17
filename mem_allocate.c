#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

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
    char code[10], allocation_status, waiting_status;
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

void initialize_memory(MemoryList *memory, int total_memory_size) {
    MemoryBlock *initial_block = malloc(sizeof(MemoryBlock));
    if (!initial_block) printf("Insufficient memory!");
    else {
        memory->head = initial_block;
        memory->total_processes = 0;
        *initial_block = (MemoryBlock) {'f', '\0', "", 0, total_memory_size, NULL, NULL};
    }
}

void initialize_comments(CommentList *comments) {
    comments->head = NULL;
}

void initialize_processes(ProcessList *process_list) {
    *process_list = (ProcessList) {NULL, NULL, 0, 0};
}

void set_console_cursor(int x, int y) {
    COORD cursor_position = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor_position);
}

int validate_file_format(char *filename) {
    FILE *file = fopen(filename, "rt");
    if (!file) { printf("Unable to open the file!"); return 0; }

    char line[100];
    int line_count = 0, semicolon_count = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") == 0) {
            printf("Avoid line breaks in the file!");
            fclose(file);
            return 0;
        }
        line_count++;
        for (char *c = line; *c; c++) {
            if (*c == ';') semicolon_count++;
            else if (*c == ' ') {
                printf("Avoid spaces in the file!");
                fclose(file);
                return 0;
            }
        }
    }
    fclose(file);

    if (line_count == 0) { printf("File is empty!"); return 0; }
    if (semicolon_count != 4 * line_count) {
        printf("Semicolons are misplaced in this file!");
        return 0;
    }
    return 1;
}

void add_comment(CommentList *comments, int timestamp, char *message, int type) {
    CommentNode *current_node = comments->head, *new_node = malloc(sizeof(CommentNode));
    if (!new_node) { printf("Error allocating memory for comment"); return; }

    if (!current_node) {
        *new_node = (CommentNode) { "none", "none", "none", "none", "none", timestamp, NULL };
        comments->head = new_node;
    } else {
        while (current_node->next && current_node->timestamp != timestamp) current_node = current_node->next;
        if (current_node->timestamp != timestamp) {
            *new_node = (CommentNode) { "none", "none", "none", "none", "none", timestamp, NULL };
            current_node->next = new_node;
        } else {
            new_node = current_node;
        }
    }

    switch (type) {
        case 1: snprintf(new_node->new_process, sizeof(new_node->new_process), "%s loaded", message); break;
        case 2: snprintf(new_node->waiting_process, sizeof(new_node->waiting_process), "%s on hold", message); break;
        case 3: snprintf(new_node->completed_process, sizeof(new_node->completed_process), "%s finished", message); break;
        case 4: snprintf(new_node->selected_process, sizeof(new_node->selected_process), "%s elected", message); break;
        case 5: snprintf(new_node->ready_process, sizeof(new_node->ready_process), "%s ready", message); break;
    }
}

void print_process_header(void) {
    printf("      -------------------------------------------------\n");
    printf("        Process   Arr. Date   Mem Space   Exec Time\n");
    printf("      -------------------------------------------------\n");
}

void load_processes_from_file(ProcessList *process_list, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { printf("Unable to open process file.\n"); return; }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        Process *new_process = malloc(sizeof(Process));
        if (!new_process) { printf("Memory allocation failed for process.\n"); return; }

        sscanf(line, "%[^;];%d;%d;%d", new_process->code, &new_process->arrival_date, 
               &new_process->memory_required, &new_process->execution_time);

        new_process->allocation_status = 'N';
        new_process->waiting_status = 'N';
        new_process->next = NULL;
        new_process->previous = process_list->tail;

        if (process_list->tail) process_list->tail->next = new_process;
        else process_list->head = new_process;
        
        process_list->tail = new_process;
        process_list->total_to_allocate++;
    }
    fclose(file);
}

void print_comments(CommentList *comments) {
    CommentNode *current = comments->head;
    while (current) {
        printf("Timestamp %d:\n", current->timestamp);
        printf("  New Process: %s\n", current->new_process);
        printf("  Waiting Process: %s\n", current->waiting_process);
        printf("  Completed Process: %s\n", current->completed_process);
        printf("  Selected Process: %s\n", current->selected_process);
        printf("  Ready Process: %s\n\n", current->ready_process);
        current = current->next;
    }
}

void display_processes(ProcessList *process_list) {
    Process *current = process_list->head;
    print_process_header();
    while (current) {
        printf("       %s       %d         %d        %d\n", 
               current->code, current->arrival_date, 
               current->memory_required, current->execution_time);
        current = current->next;
    }
    printf("      -------------------------------------------------\n");
}

int main() {
    MemoryList memory;
    ProcessList process_list;
    CommentList comments;

    initialize_memory(&memory, 1024);
    initialize_processes(&process_list);
    initialize_comments(&comments);

    // Load processes from a sample file
    load_processes_from_file(&process_list, "process.txt");
    
    // Adding comments as sample actions
    add_comment(&comments, 1, "Process1", 1);
    add_comment(&comments, 2, "Process2", 2);
    add_comment(&comments, 3, "Process3", 3);
    
    // Display processes and comments
    printf("\nProcesses Loaded:\n");
    display_processes(&process_list);

    printf("\nComments:\n");
    print_comments(&comments);

    return 0;
} 