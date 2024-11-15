#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOTAL_PAGES 25  

// Structure for pages in memory
struct page {
    int address;
    struct page* next;
};

// Structure for program loaded into memory
struct program {
    char name[15];
    int size;
    struct page* pages;
    struct program* next;
};

// Function prototypes
struct page* initialize_pages();
void display_free_pages(struct page* head);
struct program* load_program(struct program* head, char name[], int size, struct page** freePages);
void display_programs(struct program* head);
void display_memory_state(struct program* head);
struct program* remove_program(struct program* head, char name[], struct page** freePages);
int page_count(int size);
int free_page_count(struct page* head);

// Assembly function prototype
extern int check_memory_availability(int freePages, int pagesNeeded);

// Color codes for Linux terminal output
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"

int main() {
    struct page* freePages = initialize_pages();
    struct program* programList = NULL;
    char choice[5], programName[15];
    int programSize;

    printf("\n%s=== Memory Management by Paging ===%s\n", BLUE, RESET);
    
    while (1) {
        printf("\n\033[35m");
        printf("                                 +------------------------------------------------------+\n");
        printf("                                 |                      ~~ MENU ~~                      |\n");
        printf("                                 +------------------------------------------------------+\n");
        printf("                                 |         1 . Load a program                           |\n");
        printf("                                 |         2 . Display free pages                       |\n");
        printf("                                 |         3 . Display loaded programs                  |\n");
        printf("                                 |         4 . Display memory state                     |\n");
        printf("                                 |         5 . Unload a program                         |\n");
        printf("                                 |         9 . Quit                                     |\n");
        printf("                                 +------------------------------------------------------+\n");
        printf("                                 Enter your choice: \033[0m");
        scanf("%s", choice);
        if (strcmp(choice, "1") == 0) {
            printf("Enter program name: ");
            scanf("%s", programName);
            printf("Enter program size (in KB): ");
            scanf("%d", &programSize);
            programList = load_program(programList, programName, programSize, &freePages);

        } else if (strcmp(choice, "2") == 0) {
            display_free_pages(freePages);

        } else if (strcmp(choice, "3") == 0) {
            display_programs(programList);

        } else if (strcmp(choice, "4") == 0) {
            display_memory_state(programList);

        } else if (strcmp(choice, "5") == 0) {
            printf("Enter program name to unload: ");
            scanf("%s", programName);
            programList = remove_program(programList, programName, &freePages);

        } else if (strcmp(choice, "9") == 0) {
            printf("%sExiting...%s\n", RED, RESET);
            break;

        } else {
            printf("%sInvalid choice! Try again.%s\n", RED, RESET);
        }
    }

    return 0;
}

// Initialize free pages for memory
struct page* initialize_pages() {
    struct page* head = NULL, *temp = NULL;
    for (int i = 1; i <= TOTAL_PAGES; i++) {
        struct page* newPage = (struct page*)malloc(sizeof(struct page));
        newPage->address = i;
        newPage->next = NULL;
        if (!head) {
            head = newPage;
            temp = newPage;
        } else {
            temp->next = newPage;
            temp = temp->next;
        }
    }
    return head;
}

// Display all free pages
void display_free_pages(struct page* head) {
    printf("%sFree Pages:%s ", MAGENTA, RESET);
    while (head) {
        printf("%d ", head->address);
        head = head->next;
    }
    printf("\n");
}

// Calculate number of pages required for a program
int page_count(int size) {
    return (size + 99) / 100;  // Each page holds 100 KB, rounding up
}

// Load program into memory
struct program* load_program(struct program* head, char name[], int size, struct page** freePages) {
    int pagesNeeded = page_count(size);
    int availablePages = free_page_count(*freePages);

    // Call assembly function to check if sufficient memory is available
    if (!check_memory_availability(availablePages, pagesNeeded)) {
        printf("%sInsufficient free pages for program %s%s\n", RED, name, RESET);
        return head;
    }

    struct program* newProgram = (struct program*)malloc(sizeof(struct program));
    strcpy(newProgram->name, name);
    newProgram->size = size;
    newProgram->next = NULL;
    newProgram->pages = NULL;

    // Allocate pages to the program
    struct page* temp = NULL;
    for (int i = 0; i < pagesNeeded; i++) {
        struct page* pageToLoad = *freePages;
        *freePages = (*freePages)->next;

        if (temp) {
            temp->next = pageToLoad;
            temp = temp->next;
        } else {
            newProgram->pages = pageToLoad;
            temp = pageToLoad;
        }
    }
    temp->next = NULL;

    // Add program to the program list
    newProgram->next = head;
    printf("%sProgram %s loaded successfully%s\n", GREEN, name, RESET);
    return newProgram;
}

// Display loaded programs
void display_programs(struct program* head) {
    printf("%sLoaded Programs:%s\n", BLUE, RESET);
    while (head) {
        printf("- %s (Size: %d KB)\n", head->name, head->size);
        head = head->next;
    }
}

// Display memory state with programs and free pages
void display_memory_state(struct program* head) {
    printf("%sMemory State:%s\n", BLUE, RESET);
    struct page* usedPages[25] = { NULL };
    while (head) {
        struct page* temp = head->pages;
        while (temp) {
            usedPages[temp->address - 1] = temp;
            temp = temp->next;
        }
        head = head->next;
    }

    printf("Memory Layout:\n");
    for (int i = 0; i < TOTAL_PAGES; i++) {
        if (usedPages[i]) printf("[P] ");
        else printf("[ ] ");
    }
    printf("\n");
}

// Free pages used by a program and remove from program list
struct program* remove_program(struct program* head, char name[], struct page** freePages) {
    struct program* temp = head, *prev = NULL;
    while (temp) {
        if (strcmp(temp->name, name) == 0) {
            // Free pages used by program
            struct page* pgTemp = temp->pages;
            while (pgTemp) {
                struct page* nextPg = pgTemp->next;
                pgTemp->next = *freePages;
                *freePages = pgTemp;
                pgTemp = nextPg;
            }
            // Remove program from list
            if (prev) prev->next = temp->next;
            else head = temp->next;
            free(temp);
            printf("%sProgram %s unloaded successfully%s\n", GREEN, name, RESET);
            return head;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("%sProgram %s not found%s\n", RED, name, RESET);
    return head;
}

// Count free pages
int free_page_count(struct page* head) {
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }
    return count;
}
