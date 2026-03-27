
/*
 * File: first_pass.h
 * Contains the function prototypes for the assembler's first pass.
 * This phase scans the code to build the symbol table and calculate memory offsets.
 */
#include <stdio.h>
#include "Pre_Assembler.h" /* Required to recognize the OneMakro structure */
#include "memory_manager.h"

int check_label_validity(char* label_name, OneMakro* macrosArray, int total_macros, int line_number, AssemblerData* state);
void encode_the_string_into_the_data_image(char* line, int start_index, int* DC, AssemblerData* state);
int process_of_string(char* line, int index, int* DC, int line_number, AssemblerData* state);
int process_and_encode_data(char* line, int index, int* DC, int line_number, AssemblerData* state);
int process_of_extern(char* line, int index, int label_flag, int line_number, OneMakro* macrosArray, int total_macros_found, AssemblerData* state);
int process_of_entry(char* line, int index, int label_flag, int line_number, OneMakro* macrosArray, int total_macros_found);
int first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found, AssemblerData* state);
