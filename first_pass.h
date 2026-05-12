
/*
 * File: first_pass.h
 * Contains the function prototypes for the assembler's first pass.
 * This phase scans the code to build the symbol table and calculate memory offsets.
 */
#include <stdio.h>
#include "Pre_Assembler.h" /* Required to recognize the OneMakro structure */
#include "memory_manager.h"

int check_label_validity(char* label_name, OneMakro* macrosArray, int total_macros, int line_number, AssemblerData* state);
int first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found, AssemblerData* state);
