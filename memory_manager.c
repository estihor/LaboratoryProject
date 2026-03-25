#define _CRT_SECURE_NO_WARNINGS
/*
 * File: memory_manager.c
 * Purpose: Implementation of dynamic memory management for assembler tables.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"
#define OK_LABEL  1

#define LABLE_ERROR  0

 /* * Internal dynamic arrays.
  * These pointers store the base addresses of our dynamic memory blocks.
  */
label* symbol_table = NULL;
int symbol_count = 0;

CodeImage* code_image = NULL;
int code_count = 0;

DataImage* data_image = NULL;
int data_count = 0;

/* --- Implementation: Add to Symbol Table --- */
void add_symbol(char* name, int value, int is_code, int is_data, int is_entry, int is_extern) {
    label* temp;

    /* 1. Increase the logical count of labels */
    symbol_count++;

    /* 2. Resize the array to fit the new label structure using realloc */
    temp = (label*)realloc(symbol_table, symbol_count * sizeof(label));

    /* 3. Memory safety check */
    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Symbol Table)\n");
        exit(1);
    }

    symbol_table = temp;

    /* 4. Populate the new label entry using the updated field names: labelName and labelAddress */
    strcpy(symbol_table[symbol_count - 1].labelName, name);
    symbol_table[symbol_count - 1].labelAddress = value;
    symbol_table[symbol_count - 1].is_code = is_code;
    symbol_table[symbol_count - 1].is_data = is_data;
    symbol_table[symbol_count - 1].is_entry = is_entry;
    symbol_table[symbol_count - 1].is_extern = is_extern;
}

/* --- Implementation: Add to Code Image --- */
void add_code_word(int address, unsigned short code, char* label_name, int line) 
{
    CodeImage* temp;

    /* 1. Increase the count of machine instructions */
    code_count++;

    /* 2. Expand the code image array */
    temp = (CodeImage*)realloc(code_image, code_count * sizeof(CodeImage));

    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Code Image)\n");
        exit(1);
    }

    code_image = temp;

    /* 3. Populate the entry with updated field names */
    code_image[code_count - 1].Memory_address = address;
    code_image[code_count - 1].machine_code = code;
    code_image[code_count - 1].the_line_of_the_label = line;

    /* 4. Handle unresolved labels for the Second Pass */
    if (label_name != NULL) {
        strcpy(code_image[code_count - 1].missing_label, label_name);
    }
    else {
        code_image[code_count - 1].missing_label[0] = '\0';
    }
}

/* --- Implementation: Add to Data Image --- */
void add_data_word(int address, unsigned short word) {
    DataImage* temp;

    /* 1. Increase the count of data words */
    data_count++;

    /* 2. Expand the data image array */
    temp = (DataImage*)realloc(data_image, data_count * sizeof(DataImage));

    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Data Image)\n");
        exit(1);
    }

    data_image = temp;

    /* 3. Store data using updated field names: MemoryAddress and machine_code */
    data_image[data_count - 1].MemoryAddress = address;
    data_image[data_count - 1].machine_code = word;
}

/* --- Implementation: Free all memory --- */
void free_all_memory() {
    /* * Safely release all allocated memory and reset pointers to NULL.
     * This prevents memory leaks and "dangling pointers".
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

int is_label_exists(char* search_name)
{
    int i; 
    for (i = 0; i < symbol_count; i++)
    {
        if (strcmp(symbol_table[i].labelName, search_name)==0)
            return LABLE_ERROR;
    }
    return OK_LABEL;
}

/* Updates the final addresses of data symbols by adding the final IC */
void update_data_symbols_address(int final_IC)
{
    int i;
    for (i = 0; i < symbol_count; i++)
    {
        /* Check if the symbol is a data symbol (.data or .string) */
        if (symbol_table[i].is_data == 1)
        {
            /* Add the final IC to its relative DC address */
            symbol_table[i].labelAddress += final_IC;
        }
    }
}
