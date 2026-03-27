#ifndef INSTRUCTIONS_HANDLER_H
#define INSTRUCTIONS_HANDLER_H


/*
 * File: instructions_handler.h
 * Purpose: Declares functions for processing, validating, and encoding machine instructions.
 * This module is responsible for identifying the operation, routing it to the appropriate
 * worker function based on the number of operands (0, 1, or 2), verifying addressing modes,
 * and generating the final binary machine code for the Code Image.
 */

 
#include "validations.h" 
#include "memory_manager.h"
 
int process_machine_instruction(char* line, int index, int label_flag, int line_number, char* the_first_word, char* the_instruction, OneMakro* macrosArray, int total_macros_found, int* IC, AssemblerData* state);
int process_zero_operands(int line_number, char* line, int index, unsigned int opcode, unsigned int funct, int* IC, AssemblerData* state);
int process_one_operand(int line_number, char* line, int index, char* operation_name, OneMakro* macrosArray, int total_macros, unsigned int opcode, unsigned int funct, int* IC, AssemblerData* state);
int process_two_operands(int line_number, char* line, int index, char* operation_name, OneMakro* macrosArray, int total_macros, unsigned int opcode, unsigned int funct, int* IC, AssemblerData* state);
void encode_operation(unsigned int opcode, unsigned int funct, int source_mode, int destination_mode, int* IC, int line_number, AssemblerData* state);
void encode_operand(char* operand, int mode, int* IC, int line_number, AssemblerData* state);

#endif /* INSTRUCTIONS_HANDLER_H */