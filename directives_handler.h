#ifndef DIRECTIVES_HANDLER_H
#define DIRECTIVES_HANDLER_H


#include "Pre_Assembler.h" 
#include "memory_manager.h" 




void encode_the_string_into_the_data_image(char* line, int start_index, int* DC, AssemblerData* state);
int process_of_string(char* line, int index, int* DC, int line_number, AssemblerData* state);
int process_and_encode_data(char* line, int index, int* DC, int line_number, AssemblerData* state);
int process_of_extern(char* line, int index, int label_flag, int line_number, OneMakro* macrosArray, int total_macros_found, AssemblerData* state);
int process_of_entry(char* line, int index, int label_flag, int line_number, OneMakro* macrosArray, int total_macros_found);

#endif