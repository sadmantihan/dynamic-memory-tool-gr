#ifndef MEM_ALLOCATE_H
#define MEM_ALLOCATE_H

#include <stdio.h>

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

// Function Prototypes
void initialize_memory(MemoryList *memory, int total_memory_size);
void initialize_comments(CommentList *comments);
void initialize_processes(ProcessList *process_list);
void set_console_cursor(int x, int y);
int validate_file_format(const char *filename);
void add_comment(CommentList *comments, int timestamp, const char *message, int type);
void print_process_header(void);
void load_processes_from_file(ProcessList *process_list, const char *filename);
void print_comments(const CommentList *comments);
void display_processes(const ProcessList *process_list);

#endif // MEM_ALLOCATE_H
