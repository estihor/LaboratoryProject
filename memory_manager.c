#define _CRT_SECURE_NO_WARNINGS
/*
 * File: memory_manager.c
 * Purpose: Implementation of dynamic memory management for assembler tables.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"

 /* * Global pointers to the dynamic arrays.
  * These are "private" to this file (static-like behavior).
  */
Symbol* symbol_table = NULL;
int symbol_count = 0;

CodeImage* code_image = NULL;
int code_count = 0;

DataImage* data_image = NULL;
int data_count = 0;

/* --- Implementation: Add to Symbol Table --- */
void add_symbol(char* name, int value, int is_code, int is_data, int is_entry, int is_extern) {
    Symbol* temp;

    /* 1. Increase the logical count of symbols */
    symbol_count++;

    /* 2. Resize the array to fit the new symbol.
       realloc preserves existing data and allocates a new block if necessary. */
    temp = (Symbol*)realloc(symbol_table, symbol_count * sizeof(Symbol));

    /* 3. Safety check: If realloc fails, it returns NULL. We must exit to prevent crashes. */
    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Symbol Table)\n");
        exit(1);
    }

    /* 4. Update the global pointer to the new (possibly moved) memory address */
    symbol_table = temp;

    /* 5. Fill the last slot in the array (index is count - 1) with the provided data */
    strcpy(symbol_table[symbol_count - 1].name, name);
    symbol_table[symbol_count - 1].value = value;
    symbol_table[symbol_count - 1].is_code = is_code;
    symbol_table[symbol_count - 1].is_data = is_data;
    symbol_table[symbol_count - 1].is_entry = is_entry;
    symbol_table[symbol_count - 1].is_extern = is_extern;
}

/* --- Implementation: Add to Code Image --- */
void add_code_word(int address, unsigned short code, char* label, int line) {
    CodeImage* temp;

    /* 1. Increase the count of machine words in the code image */
    code_count++;

    /* 2. Expand the dynamic array to accommodate the new machine word structure */
    temp = (CodeImage*)realloc(code_image, code_count * sizeof(CodeImage));

    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Code Image)\n");
        exit(1);
    }

    code_image = temp;

    /* 3. Populate the new entry's fields */
    code_image[code_count - 1].Memory_address = address;
    code_image[code_count - 1].machine_code = code;
    code_image[code_count - 1].the_line_of_the_label = line;

    /* 4. If a label name was provided (unresolved symbol), copy it for the second pass.
       Otherwise, initialize it as an empty string to avoid junk data. */
    if (label != NULL) {
        strcpy(code_image[code_count - 1].missing_label, label);
    }
    else {
        code_image[code_count - 1].missing_label[0] = '\0';
    }
}

/* --- Implementation: Add to Data Image --- */
void add_data_word(int address, unsigned short word) {
    DataImage* temp;

    /* 1. Increase the count of data words (.data or .string values) */
    data_count++;

    /* 2. Expand the data array using realloc */
    temp = (DataImage*)realloc(data_image, data_count * sizeof(DataImage));

    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Data Image)\n");
        exit(1);
    }

    data_image = temp;

    /* 3. Store the address and the binary representation of the data */
    data_image[data_count - 1].address = address;
    data_image[data_count - 1].machine_word = word;
}

/* --- Implementation: Free all memory --- */
void free_all_memory() {
    /* * For each dynamic array, we check if it was allocated (not NULL),
     * free the memory block, and then reset the pointer and counter
     * to ensure the manager is ready for a fresh start (e.g., next file).
     */
    if (symbol_table != NULL) {
        free(symbol_table);
        symbol_table = NULL;
        symbol_count = 0;
    }

    if (code_image != NULL) {
        free(code_image);
        code_image = NULL;
        code_count = 0;
    }

    if (data_image != NULL) {
        free(data_image);
        data_image = NULL;
        data_count = 0;
    }
}