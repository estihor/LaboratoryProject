#ifndef PRE_ASSEMBLER_H
#define PRE_ASSEMBLER_H
#include <stdio.h>
typedef struct
{
    char* Makroname;
    char* MakroContent;
} OneMakro;

FILE* create_asEnding_Tofile(char* baseFileName);
FILE* create_amEnding_Tofile(char* baseFileName);
OneMakro* Macro_word_search(FILE* asFile, int* MacroCountRequiredForLoop);
void Creates_the_file_am(OneMakro* macrosArray, int total_macros_found, FILE* asFile, FILE* amFile);
void Inserting_a_macro_into_an_array(char* makroName, int* count, OneMakro** arr);
void adding_lines_of_macro_to_arr(OneMakro* currentMacroArr, char* line);
int is_it_a_macro(OneMakro* macrosArray, char* IsITAMakroWord, int totalMacros);
int No_word_after_macro(char* line, int numOfWords);
void Copying_line_to_amfile(char* line, FILE* amFile);
void Release_the_macrosArray(OneMakro* macrosArray, int total_macros_found);
#endif
