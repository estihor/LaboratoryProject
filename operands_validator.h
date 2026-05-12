#pragma once

#include "Pre_Assembler.h" 
#include "memory_manager.h" 
int what_is_the_addressing_mode(char* argument);
int is_valid_integer(char* argument, int line_number);
int is_addressing_mode_2_valid(OneMakro* macrosArray, char* argument, int totalMacros, int line_number);
int is_valid_addressing(char* operation_name, int source_mode, int destination_mode);
int validate_operand_by_mode(char* operand, int mode, OneMakro* macrosArray, int total_macros, int line_number);
