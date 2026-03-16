
/*
 * File: memory_manager.h
 * Purpose: Defines structures and functions for managing the assembler's memory tables.
 */

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

 /* --- 1. Symbol Table Structure --- */
typedef struct {
    char name[32];      /* Symbol name */
    int value;          /* Address value (IC or DC) */
    int is_code;        /* Flag: 1 if it's a code instruction */
    int is_data;        /* Flag: 1 if it's a data directive */
    int is_entry;       /* Flag: 1 if defined as .entry */
    int is_extern;      /* Flag: 1 if defined as .extern */
} Symbol;

/* --- 2. Code Image Structure --- */
typedef struct {
    int Memory_address;           /* Memory address (e.g., 100, 101...) */
    unsigned short machine_code;  /* The 16-bit binary machine word */
    char missing_label[32];       /* Name of the label to be resolved in Second Pass */
    int the_line_of_the_label;    /* Source line number for error reporting */
} CodeImage;

/* --- 3. Data Image Structure --- */
typedef struct {
    int address;                  /* Memory address (DC) */
    unsigned short machine_word;  /* The 16-bit binary data word */
} DataImage;

/* --- Function Prototypes --- */

/* Adds a symbol to the dynamic symbol table */
void add_symbol(char* name, int value, int is_code, int is_data, int is_entry, int is_extern);
/* Adds a machine word to the code image array */
void add_code_word(int address, unsigned short code, char* label, int line);
/* Adds a data word to the data image array */
void add_data_word(int address, unsigned short word);
/* Frees all dynamically allocated memory */
void free_all_memory();

#endif