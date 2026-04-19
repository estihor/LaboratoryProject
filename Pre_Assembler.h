#pragma once
#include <stdio.h> /* חשוב שיהיה כאן בשביל FILE */

typedef struct Makro {
    char* Makroname;
    char* MakroContent;
} OneMakro;/* חובה נקודה פסיק כאן! */

/* הצהרות על פונקציות */
void Creates_the_file_am(OneMakro* macrosArray, int total_macros_found, FILE* asFile, FILE* amFile);
void Release_the_macrosArray(OneMakro* macrosArray, int total_macros_found);
void Copying_line_to_amfile(char* line, FILE* amFile);
int No_word_after_macro(char* line, int numOfWords);
int is_it_a_macro(OneMakro* macrosArray, char* IsITAMakroWord, int totalMacros);
void adding_lines_of_macro_to_arr(OneMakro* currentMacroArr, char* line);
OneMakro* Macro_word_search(FILE* asFile, int* MacroCountRequiredForLoop, char* fileName, int* errorFlag);
void Allocating_memory_for_the_macro_array(char* makroName, int* count, OneMakro** arr);
FILE* create_file_with_extension(char* baseFileName, char extension, char* mode);