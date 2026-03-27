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

 
/* --- Implementation: Add to Symbol Table --- */
void add_symbol(char* name, int value, int is_code, int is_data, int is_entry, int is_extern, AssemblerData* state)
{
    label* temp;

    /* 1. Increase the logical count of labels */
    state->symbol_count++;

    /* 2. Resize the array to fit the new label structure using realloc */
    temp = (label*)realloc(state->symbol_table, state->symbol_count * sizeof(label));

    /* 3. Memory safety check */
    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Symbol Table)\n");
        exit(1);
    }

    state->symbol_table = temp;

    /* 4. Populate the new label entry using the updated field names: labelName and labelAddress */
    strcpy(state->symbol_table[state->symbol_count - 1].labelName, name);
    state->symbol_table[state->symbol_count - 1].labelAddress = value;
    state->symbol_table[state->symbol_count - 1].is_code = is_code;
    state->symbol_table[state->symbol_count - 1].is_data = is_data;
    state->symbol_table[state->symbol_count - 1].is_entry = is_entry;
    state->symbol_table[state->symbol_count - 1].is_extern = is_extern;
}

/* --- Implementation: Add to Code Image --- */
void add_code_word(int address, unsigned short code, char* label_name, int line, AssemblerData* state)
{
    CodeImage* temp;

    /* 1. Increase the count of machine instructions */
    state->code_count++;

    /* 2. Expand the code image array */
    temp = (CodeImage*)realloc(state->code_image, state->code_count * sizeof(CodeImage));

    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Code Image)\n");
        exit(1);
    }

    state->code_image = temp;

    /* 3. Populate the entry with updated field names */
    state->code_image[state->code_count - 1].Memory_address = address;
    state->code_image[state->code_count - 1].machine_code = code;
    state->code_image[state->code_count - 1].the_line_of_the_label = line;

    /* 4. Handle unresolved labels for the Second Pass */
    if (label_name != NULL) {
        strcpy(state->code_image[state->code_count - 1].missing_label, label_name);
    }
    else {
        state->code_image[state->code_count - 1].missing_label[0] = '\0';
    }
}

/* --- Implementation: Add to Data Image --- */
void add_data_word(int address, unsigned short word , AssemblerData* state) 
{
    DataImage* temp;

    /* 1. Increase the count of data words */
    state->data_count++;

    /* 2. Expand the data image array */
    temp = (DataImage*)realloc(state->data_image, state->data_count * sizeof(DataImage));

    if (temp == NULL) {
        printf("Fatal Error: Memory allocation failed (Data Image)\n");
        exit(1);
    }

    state->data_image = temp;

    /* 3. Store data using updated field names: MemoryAddress and machine_code */
    state->data_image[state->data_count - 1].MemoryAddress = address;
    state->data_image[state->data_count - 1].machine_code = word;
}

/* --- Implementation: Free all memory --- */
void free_all_memory(AssemblerData* state) {
    /* * Safely release all allocated memory and reset pointers to NULL.
     * This prevents memory leaks and "dangling pointers".
     */
    if (state->symbol_table != NULL) {
        free(state->symbol_table);
        state->symbol_table = NULL;
        state->symbol_count = 0;
    }

    if (state->code_image != NULL) {
        free(state->code_image);
        state->code_image = NULL;
        state->code_count = 0;
    }

    if (state->data_image != NULL) {
        free(state->data_image);
        state->data_image = NULL;
        state->data_count = 0;
    }
}

int is_label_exists(char* search_name,  AssemblerData* state)
{
    int i; 
    for (i = 0; i < state->symbol_count; i++)
    {
        if (strcmp(state->symbol_table[i].labelName, search_name)==0)
            return LABLE_ERROR;
    }
    return OK_LABEL;
}

/* Updates the final addresses of data symbols by adding the final IC */
void update_data_symbols_address(int final_IC, AssemblerData* state)
{
    int i;
    for (i = 0; i < state->symbol_count; i++)
    {
        /* Check if the symbol is a data symbol (.data or .string) */
        if (state->symbol_table[i].is_data == 1)
        {
            /* Add the final IC to its relative DC address */
            state->symbol_table[i].labelAddress += final_IC;
        }
    }
}
