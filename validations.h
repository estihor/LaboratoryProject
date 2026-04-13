#pragma once
#include "Pre_Assembler.h"
/*
 * This struct defines the structure of a single machine operation.
 * name: The name of the assembly command (e.g., "mov", "add").
 * opcode: The primary operation code.
 * funct:  The secondary function code (0 if not applicable).
 */
typedef struct 
{
    char* name;
    unsigned int opcode;
    unsigned int funct;
} Operations1;

/*
 * This struct defines the structure for checking operand limits.
 * name: The name of the assembly command.
 * num_of_operands: The exact number of operands this command requires.
 */
typedef struct 
{
    char* name;
    unsigned int num_of_operands;

} Operations2;

typedef struct 
{
    char* name;
    int arr_source_operand[4];
    int arr_destination_operand[4];
} Operations3;



 /* * File: operations.h
  * Contains declarations for searching assembly operations.
  */
int is_it_an_operation_and_find_opcode_and_funct(char* name, unsigned int* opcode, unsigned int* funct);
int is_it_an_operation_and_find_operands(char* name, unsigned int* num_of_operands);
int is_it_a_register(char* name);
int is_it_an_instruction(char* name);
int is_reserved_word(OneMakro* macrosArray, char* name, int totalMacros);
int what_is_the_addressing_mode(char* argument);
int is_it_a_valid_label(OneMakro* macrosArray, char* name, int totalMacros, int line_number);
int is_valid_integer(char* argument, int line_number);
int is_addressing_mode_2_valid(OneMakro* macrosArray, char* argument, int totalMacros, int line_number);
int is_valid_addressing(char* operation_name, int source_mode, int destination_mode);
int is_it_a_macro(OneMakro* macrosArray, char* name, int totalMacros);
int validate_operand_by_mode(char* operand, int mode, OneMakro* macrosArray, int total_macros, int line_number);
