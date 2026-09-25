# Dynamic Memory Management Tool

A collection of C programs (with an x86-64 NASM assembly routine) built for an Operating Systems course, simulating core OS memory management techniques: **contiguous allocation**, **paging**, and **memory leak detection**.

## Overview

This project contains three standalone simulators:

| Program | Source | Description |
|---|---|---|
| **Contiguous Allocator** | `mem_allocate.c` | Simulates memory allocation using a doubly linked list of memory blocks, supporting **First-Fit** and **Best-Fit** allocation strategies with block splitting. |
| **Paging Simulator** | `paging.c` + `memory_utils.asm` | Simulates page-based memory management (25 fixed-size pages), loading/unloading programs into pages, with the availability check written in **x86-64 assembly**. |
| **Memory Leak Detector** | `mem_leak_detector.c` | A lightweight tool that tracks every `malloc` call in a linked list, reports unfreed blocks, and can bulk-free tracked memory. |

## Features

### Contiguous Allocation (`mem_allocate.c`)
- Interactive menu to add processes (code, arrival time, memory required, execution time)
- **First-Fit** and **Best-Fit** allocation algorithms
- Automatic block splitting when a process doesn't need a full free block
- Console-rendered memory map table

### Paging Simulator (`paging.c`)
- Fixed pool of 25 pages (100 KB each)
- Load/unload programs by name and size, with pages allocated/freed as a linked list
- Visual memory layout (`[P]` = used page, `[ ]` = free page)
- Calls into a **NASM assembly function** (`check_memory_availability`) to verify enough free pages exist before loading a program — a small demo of mixing C and assembly via the System V calling convention (`RDI`/`RSI` registers)

### Memory Leak Detector (`mem_leak_detector.c`)
- Tracks every allocated pointer and its size in a linked list
- Reports all currently unfreed allocations (address + size)
- Frees all tracked memory in one action
- On exit, automatically reports any remaining leaks

## Tech Stack

- **C** (all core logic)
- **x86-64 NASM assembly** (`memory_utils.asm`) — linked into the paging simulator
- Precompiled binaries included: `mem_allocate`, `mem_leak_detector`, `memory_manager` (Linux ELF) and `mem_allocate.exe` (Windows)

## Building from Source

### Requirements
- `gcc` (or any C compiler)
- `nasm` (for assembling `memory_utils.asm`)
- Linux/WSL recommended (the programs use ANSI color escape codes for terminal output)

### 1. Contiguous Allocator
```bash
gcc mem_allocate.c -o mem_allocate
./mem_allocate
```

### 2. Memory Leak Detector
```bash
gcc mem_leak_detector.c -o mem_leak_detector
./mem_leak_detector
```

### 3. Paging Simulator (C + Assembly)
```bash
nasm -f elf64 memory_utils.asm -o memory_utils.o
gcc paging.c memory_utils.o -o memory_manager
./memory_manager
```

> On Windows, assemble with a Windows-targeted NASM output format (e.g. `-f win64`) and link with a matching toolchain (e.g. MinGW), or run the included `mem_allocate.exe`.

## Usage

Each program launches an interactive text menu. For example, the paging simulator:

```
=== Memory Management by Paging ===

 +------------------------------------------------------+
 |                        ~~ MENU ~~                     |
 +------------------------------------------------------+
 |  1 . Load a program                                   |
 |  2 . Display free pages                                |
 |  3 . Display loaded programs                           |
 |  4 . Display memory state                              |
 |  5 . Unload a program                                  |
 |  9 . Quit                                              |
 +------------------------------------------------------+
```

Screenshots of the paging simulator in action are included in the repo (`pagination1.png`, `pagination2.png`).

## Project Structure

```
dynamic-memory-tool-gr/
├── mem_allocate.c          # First-Fit / Best-Fit contiguous allocation
├── mem_allocate.h
├── mem_leak_detector.c      # Memory leak tracking tool
├── mem_leak_detector.h
├── paging.c                 # Paging-based memory simulator
├── memory_utils.asm         # NASM routine: check_memory_availability
├── pagination1.png           # Screenshot
├── pagination2.png           # Screenshot
└── .vscode/                  # Editor config
```

## Course Context

Built as part of an Operating Systems (CSE-512) Lab course project, focused on demonstrating memory management strategies (contiguous allocation, paging) and low-level memory tracking, including direct interoperation between C and x86-64 assembly.

