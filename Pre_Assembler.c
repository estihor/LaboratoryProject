#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Pre_Assembler.h"
#include "validations.h"
/*
 * Safely frees all dynamically allocated memory used by the macros array.
 * Performs a deep free, releasing the memory for macro names, contents, and the array itself.
 * * @param macrosArray The dynamic array to be freed.
 * @param total_macros_found The total number of macros stored in the array.
 */
void Release_the_macrosArray(OneMakro* macrosArray, int total_macros_found)
{
    int i;
    /* בדיקה שהמצביע לא ריק לפני שמתחילים */
    if (macrosArray == NULL)
    {
        return;
    }

    for (i = 0; i < total_macros_found; i++)
    {
        /* בדיקה פנימית שכל שדה אכן הוקצה לפני שמשחררים */
        if (macrosArray[i].MakroContent != NULL) {
            free(macrosArray[i].MakroContent);
            macrosArray[i].MakroContent = NULL; /* הגנה מפני שחרור כפול */
        }

        if (macrosArray[i].Makroname != NULL) {
            free(macrosArray[i].Makroname);
            macrosArray[i].Makroname = NULL;
        }
    }
    free(macrosArray);
}
/*
 * Reads the source file a second time and creates the expanded output file (.am).
 * Omits macro definitions and replaces macro calls with their respective content.
 * * @param macrosArray The array containing all stored macros.
 * @param total_macros_found The total number of macros in the array.
 * @param asFile The original source assembly file (.as) opened for reading.
 * @param amFile The target expanded assembly file (.am) opened for writing.
 */
void Creates_the_file_am(OneMakro* macrosArray, int total_macros_found, FILE* asFile, FILE* amFile)
{
    int InMacro = 0;
    int macroIndex;
    char IsITAMakroWord[82] = {0};
    char line[82];
    /* Reset the read pointer to the beginning of the source file */
    rewind(asFile);
    while (fgets(line, sizeof(line), asFile) != NULL)
    {
        if (sscanf(line, "%s", IsITAMakroWord) == 1)
        {   /* Skip the macro definition opening line */
            if (strcmp(IsITAMakroWord, "mcro") == 0)
            {
                InMacro = 1;
                continue;
            }
            if (strcmp(IsITAMakroWord, "endmcro") == 0) /* Skip the macro definition closing line */
            {
                InMacro = 0;
                continue;
            }
            if (InMacro == 1) /* Skip all content lines inside the macro definition */
            {
                continue;
            }
            /* Check if the current word is a call to a known macro */
            macroIndex = is_it_a_macro(macrosArray, IsITAMakroWord, total_macros_found);
            if (macroIndex != -1)
            {
                if (No_word_after_macro(line, 1) == 1)/* Ensure there are no extra words after the macro call */
                {   /* Write the expanded macro content to the target file */
                    Copying_line_to_amfile(macrosArray[macroIndex].MakroContent, amFile);
                }
                else /* Invalid macro call with trailing garbage, skip it */
                {
                    continue;
                }
            }
            else /* It's a regular assembly line, copy it as is */
            {
                Copying_line_to_amfile(line, amFile);
            }
        }
        else /* Empty line or line with only whitespaces, copy it as is */
        {
            Copying_line_to_amfile(line, amFile);
        }
    }
}
/*
 * Writes a given string (line of code or macro content) to the expanded output file.
 * * @param line The string to be written.
 * @param amFile The target file pointer opened for writing.
 */
void Copying_line_to_amfile(char* line, FILE* amFile)
{
    fprintf(amFile, "%s", line);
}
/*
 * Validates a line to ensure it contains exactly the expected number of words.
 * Used to detect trailing garbage characters after macro names or definitions.
 * * @param line The line of text to validate.
 * @param numOfWords The exact number of words expected in the line.
 * @return 1 if the line contains exactly the expected number of words, 0 otherwise.
 */
int No_word_after_macro(char* line, int numOfWords)
{
    char CheckWord1[82];
    char CheckWord2[82];
    char CheckWord3[82];
    if (sscanf(line, "%s %s %s", CheckWord1, CheckWord2, CheckWord3) == numOfWords)
    {
        return 1;
    }
    return 0;
}
/*
 * Checks if a given word matches the name of any macro stored in the array.
 * * @param macrosArray The array containing all valid macros.
 * @param IsITAMakroWord The word to search for in the array.
 * @param totalMacros The total number of macros in the array.
 * @return The index of the macro in the array if found, or -1 if the word is not a macro.
 */
int is_it_a_macro(OneMakro* macrosArray, char* IsITAMakroWord, int totalMacros)
{
    int i;
    for (i = 0; i < totalMacros; i++)
    {
        if (strcmp(macrosArray[i].Makroname, IsITAMakroWord) == 0)
        {
            return i;
        }
    }
    return -1;
}
/*
 * Generates a file name with a ".as" extension and opens it for reading.
 * * @param baseFileName The base name of the file (without extension).
 * @return A file pointer to the opened ".as" file, or NULL if it fails.
 */
FILE* create_asEnding_Tofile(char* baseFileName)
{
    int len;
    FILE* pointerBaseF;
    char fileName[FILENAME_MAX];
    strcpy(fileName, baseFileName);
    len = (int)strlen(fileName);
    fileName[len] = '.';
    fileName[len + 1] = 'a';
    fileName[len + 2] = 's';
    fileName[len + 3] = '\0';
    pointerBaseF = fopen(fileName, "r");
    return pointerBaseF;
}
/*
 * Generates a file name with a ".am" extension and opens it for writing.
 * * @param baseFileName The base name of the file (without extension).
 * @return A file pointer to the created ".am" file, or NULL if it fails.
 */
FILE* create_amEnding_Tofile(char* baseFileName, char* mode)
{
    int len1;
    FILE* pointerNewF;
    char newfilename[FILENAME_MAX];
    strcpy(newfilename, baseFileName);
    len1 =(int)strlen(newfilename);
    newfilename[len1] = '.';
    newfilename[len1 + 1] = 'a';
    newfilename[len1 + 2] = 'm';
    newfilename[len1 + 3] = '\0';
    pointerNewF = fopen(newfilename, mode);
    return pointerNewF;

}
/*
 * Appends a new line of text to the content of a specific macro.
 * Dynamically resizes the memory allocated for the macro's content string to fit the new line.
 * * @param currentMacroArr Pointer to the specific macro struct being updated.
 * @param line The string (line of code) to append to the macro's content.
 */
void adding_lines_of_macro_to_arr(OneMakro* currentMacroArr, char* line)
{   /* Calculate the current length of the stored content */
    int tillNowLength = (int)strlen(currentMacroArr->MakroContent);
    int lineLength = (int)strlen(line);/* Calculate the length of the new line to be appended */
    /* Reallocate the content string to accommodate both lengths plus the null terminator */
    char* temp = (char*)realloc(currentMacroArr->MakroContent, (tillNowLength + lineLength) * sizeof(char) + 1);
    if (temp == NULL)
    {
        printf("An attempt to allocate memory failed");
        exit(1);
    }
    currentMacroArr->MakroContent = temp;
    /* Append the new line to the end of the existing content */
    strcat(currentMacroArr->MakroContent, line);

}
/*
 * Scans the source file to find macro definitions (mcro ... endmcro).
 * Allocates memory dynamically and builds an array containing the names and contents of all valid macros.
 * * @param asFile The source assembly file (.as) to scan.
 * @param MacroCountRequiredForLoop Pointer to an integer to store the total number of macros found.
 * @return A pointer to the dynamically allocated array of macros (OneMakro*).
 */
OneMakro* Macro_word_search(FILE* asFile, int* MacroCountRequiredForLoop)
{
    char line[82];          /* Buffer to store the current line being read */
    char IsITAMakroWord[82] = {0}; /* Buffer to extract the first word of the line */
    char makroName[82] = {0};     /* Buffer to store the extracted macro name */
    OneMakro* macro_array = NULL;
    int macroCounter = 0;
    int insideMacro = 0; /* Flag indicating if we are currently inside a macro definition */
    unsigned int opcode;/*For the function find_opcode_and_funct */
    unsigned int funct; /* For the function find_opcode_and_funct */
        /* Read the file line by line until the end */
        while (fgets(line, sizeof(line), asFile) != NULL)
        {   /* Attempt to extract the first word from the line */
            if (sscanf(line, "%s", IsITAMakroWord) == 1)
            {   /* Case 1: Found the start of a new macro definition */
                if (insideMacro == 0 && strcmp(IsITAMakroWord, "mcro") == 0)
                {
                   (void) sscanf(line, "mcro %s", makroName);
                    /* Validate that the macro name is not a reserved word (register/instruction)
                       and that there is no trailing garbage after the name */
                    if (is_it_a_register(makroName) == 0 && is_it_an_instruction(makroName) == 0
                        && is_it_an_operation_and_find_opcode_and_funct(makroName, &opcode, &funct) == 0 && No_word_after_macro(line, 2) == 1)
                    {
                        /* Allocate memory for the new macro and update the counter */
                        Inserting_a_macro_into_an_array(makroName, &macroCounter, &macro_array);
                        insideMacro = 1;/* Turn on the macro flag */
                        continue;
                    }
                }
                /* Case 2: Found the end of the current macro definition */
                if (insideMacro == 1 && strcmp(IsITAMakroWord, "endmcro") == 0)
                {
                    insideMacro = 0; /* מעדכנים דגל שיצאנו */
                    continue;
                }
                /* Case 3: We are inside a valid macro, so add the line to its content */
                if (insideMacro == 1)
                {
                    /* Send the address of the last created macro struct to append the line */
                    adding_lines_of_macro_to_arr(&(macro_array[macroCounter - 1]), line);
                }
            }
        }
    *MacroCountRequiredForLoop = macroCounter; /* Return the total number of valid macros found to the main function */
        return macro_array;
}
/*
 * Allocates and initializes memory for a newly found macro in the dynamic array.
 * Increases the array size and allocates memory for the macro's name and content strings.
 * * @param makroName The name of the new macro to be inserted.
 * @param count Pointer to the current count of macros (will be incremented).
 * @param arr Pointer to the dynamic array of macros to be reallocated.
 */
void Inserting_a_macro_into_an_array(char* makroName, int* count, OneMakro** arr)
{
    (*count)++; /* Increment the array size counter */
        /* Reallocate memory for the main array to fit the new macro struct */
        OneMakro * temp = (OneMakro*)realloc(*arr, (*count) * sizeof(OneMakro));
    if (temp == NULL)
    {
        printf("An attempt to allocate memory failed");
        exit(1);
    }
    *arr = temp;
    /* Allocate memory for the macro's name string (+1 for the null terminator) */
    (*arr)[*count - 1].Makroname = (char*)malloc((strlen(makroName) + 1) * sizeof(char));
    if ((*arr)[*count - 1].Makroname == NULL)
    {
        printf("An attempt to allocate memory failed");
        exit(1);
    }
    /* Copy the extracted name into the newly allocated memory */
    strcpy((*arr)[*count - 1].Makroname, makroName);
    /* Allocate initial memory for the macro's content (start as an empty string) */
    (*arr)[*count - 1].MakroContent = (char*)malloc(1 * sizeof(char));
    if ((*arr)[*count - 1].MakroContent == NULL)
    {
        printf("An attempt to allocate memory failed");
        exit(1);
    }
    (*arr)[*count - 1].MakroContent[0] = '\0';
}