/*
 * File: first_pass.h
 * Contains the function prototypes for the assembler's first pass.
 * This phase scans the code to build the symbol table and calculate memory offsets.
 */
#include <stdio.h>
#include "Pre_Assembler.h" /* Required to recognize the OneMakro structure */

int check_label_validity(char* label_name, OneMakro* macrosArray, int total_macros, int line_number);
void encode_the_string_into_the_data_image(char* line, int start_index, int* DC);
int process_of_string(char* line, int index, int* DC, int line_number);
int process_and_encode_data(char* line, int index, int* DC, int line_number);
int first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found);
