section .text
global check_memory_availability

; Function: check_memory_availability
; Input: RDI = total free pages
;        RSI = pages needed
; Output: RAX = 1 (enough memory), 0 (not enough memory)
; Description:
; - Checks if the total free pages are greater than or equal to the pages needed.
; - Returns 1 if there are enough pages, otherwise returns 0.

check_memory_availability:
    cmp rsi, rdi        ; Compare pages_needed (RSI) with free_pages (RDI)
    jle enough_memory   ; Jump to enough_memory if RDI >= RSI
    xor rax, rax        ; Set RAX to 0 (not enough memory)
    ret                 ; Return

enough_memory:
    mov rax, 1          ; Set RAX to 1 (enough memory)
    ret                 ; Return
