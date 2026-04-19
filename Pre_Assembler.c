#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Pre_Assembler.h"
#include "validations.h"
#define MAXIMUM_LINE_LENGTH 82       /* אורך שורה מקסימלי כולל תווים מיוחדים */
#define MAXIMUM_WORD_LENGTH 82       /* אורך מקסימלי למילה בודדת (שם מקרו/פקודה) */
#define  MACRO_NOT_FOUND -1              /* כשלא נמצא מקרו במערך */
#define SUCCESS 1                 /* סימון להצלחה (למשל בוולידציה) */
#define FAILURE 0
#define Memory_allocation_failed 1
#define YES 1               
#define  NO 0
#define STRCMP_SUCCESS 0
#define EXPECTED_MACRO_DEF_NUM_WORD 2      /* בשורה כמו: mcro MY_MACRO */
#define EXPECTED_NUM_WORDS_MACRO_CALL 1     /* בשורה של קריאה למקרו בלבד */
#define EXPECTED_NUM_WORDS_ENDMACRO 1 
#define Initial_macro_size 1     /* גודל התחלתי להקצאת תוכן מקרו */
#define ERORR_IN_MACRO_DEFINITION 1
/*
 * Safely frees all dynamically allocated memory used by the macros array.
 * Performs a deep free, releasing the memory for macro names, contents, and the array itself.
 * * @param macrosArray The dynamic array to be freed.
 * @param total_macros_found The total number of macros stored in the array.
 */
void Release_the_macrosArray(OneMakro* macrosArray, int total_macros_found)
{
    int i; /* הכרזת משתנים חובה להיות השורה הראשונה בפונקציה! */

    if (macrosArray == NULL)
    {
        return;
    }

    for (i = 0; i < total_macros_found; i++)
    {
        if (macrosArray[i].MakroContent != NULL)
        {
            free(macrosArray[i].MakroContent);
        }

        if (macrosArray[i].Makroname != NULL)
        {
            free(macrosArray[i].Makroname);
        }
    } /* כאן מסתיימת הלולאה */

    /* רק אחרי שכל התאים נוקו, משחררים את המערך הגדול! */
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
    int InMacro = NO;
    int macroIndex;
    char IsITAMakroWord[MAXIMUM_WORD_LENGTH] = { 0 };
    char line[MAXIMUM_LINE_LENGTH] = { 0 };
    /* Reset the read pointer to the beginning of the source file */
    rewind(asFile);
    while (fgets(line, sizeof(line), asFile) != NULL)
    {
        if (sscanf(line, "%81s", IsITAMakroWord) == 1)
        {   /* Skip the macro definition opening line */
            if (strcmp(IsITAMakroWord, "mcro") == STRCMP_SUCCESS)
            {
                InMacro = YES;
                continue;
            }
            if (strcmp(IsITAMakroWord, "endmcro") == STRCMP_SUCCESS) /* Skip the macro definition closing line */
            {
                InMacro = NO;
                continue;
            }
            if (InMacro == YES) /* Skip all content lines inside the macro definition */
            {
                continue;
            }
            /* Check if the current word is a call to a known macro */
            macroIndex = is_it_a_macro(macrosArray, IsITAMakroWord, total_macros_found);
            if (macroIndex != MACRO_NOT_FOUND)
            {
                if (No_word_after_macro(line, EXPECTED_NUM_WORDS_MACRO_CALL) == SUCCESS)/* Ensure there are no extra words after the macro call */
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
    char CheckWord1[MAXIMUM_WORD_LENGTH];
    char CheckWord2[MAXIMUM_WORD_LENGTH];
    char CheckWord3[MAXIMUM_WORD_LENGTH];
    if (sscanf(line, "%s %s %s", CheckWord1, CheckWord2, CheckWord3) == numOfWords)
    {
        return SUCCESS;
    }
    return FAILURE;
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
        if (strcmp(macrosArray[i].Makroname, IsITAMakroWord) == STRCMP_SUCCESS)
        {
            return i;
        }
    }
    return MACRO_NOT_FOUND;
}

FILE* create_file_with_extension(char* baseFileName, char extension, char* mode)
{
    int len;
    FILE* pointerF;
    char fileName[FILENAME_MAX];
    strcpy(fileName, baseFileName);
    len = (int)strlen(fileName);
    /* Add the extension */
    fileName[len] = '.';
    fileName[len + 1] = 'a';
    fileName[len + 2] = extension;
    fileName[len + 3] = '\0';
    /* Opening the file according to the received status */
    pointerF = fopen(fileName, mode);
    return pointerF;
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
    char* temp = (char*)realloc(currentMacroArr->MakroContent, (tillNowLength + lineLength) * sizeof(char) + Initial_macro_size);
    if (temp == NULL)
    {
        printf("An attempt to allocate memory failed");
        exit(Memory_allocation_failed);
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
OneMakro* Macro_word_search(FILE* asFile, int* MacroCountRequiredForLoop, char* fileName, int* errorFlag)
{
    char line[MAXIMUM_LINE_LENGTH] = { 0 };          /* Buffer to store the current line being read */
    char IsITAMakroWord[MAXIMUM_WORD_LENGTH] = { 0 }; /* Buffer to extract the first word of the line */
    char makroName[MAXIMUM_WORD_LENGTH] = { 0 };     /* Buffer to store the extracted macro name */
    OneMakro* macro_array = NULL;
    int  LineCounter = 0;
    int macroCounter = 0;
    int insideMacro = NO; /* Flag indicating if we are currently inside a macro definition */
    unsigned int opcode;/*For the function find_opcode_and_funct */
    unsigned int funct; /*For the function find_opcode_and_funct */
    /* Read the file line by line until the end */
    while (fgets(line, sizeof(line), asFile) != NULL)
    {
        LineCounter++;
        /* Attempt to extract the first word from the line */
        if (sscanf(line, "%81s", IsITAMakroWord) == 1)
        {   /* Case 1: Found the start of a new macro definition */
            if (insideMacro == NO && strcmp(IsITAMakroWord, "mcro") == STRCMP_SUCCESS)
            {
                if (sscanf(line, "mcro %s", makroName) == 1) {
                    /* Validate that the macro name is not a reserved word (register/instruction)
                       and that there is no trailing garbage after the name */
                    if (is_it_a_register(makroName) == FAILURE && is_it_an_instruction(makroName) == FAILURE
                        && is_it_an_operation_and_find_opcode_and_funct(makroName, &opcode, &funct) == FAILURE && No_word_after_macro(line, EXPECTED_MACRO_DEF_NUM_WORD) == SUCCESS)
                    {
                        /* Allocate memory for the new macro and update the counter */
                        Allocating_memory_for_the_macro_array(makroName, &macroCounter, &macro_array);
                        insideMacro = YES;/* Turn on the macro flag */
                        continue;
                    }
                    else
                    {
                        printf("Error in file %s, line %d: Invalid macro name '%s' or extraneous text after definition.\n", fileName, LineCounter, makroName);
                        *errorFlag = ERORR_IN_MACRO_DEFINITION;
                        return NULL;
                    }
                }
                else
                {
                    printf("Error in file %s, line %d: Missing macro name after 'mcro'.\n", fileName, LineCounter);
                    *errorFlag = ERORR_IN_MACRO_DEFINITION;
                    return NULL;
                }
            }
            /* Case 2: Found the end of the current macro definition */
            if (insideMacro == YES && strcmp(IsITAMakroWord, "endmcro") == STRCMP_SUCCESS)
            {
                if ((No_word_after_macro(line, EXPECTED_NUM_WORDS_ENDMACRO) == SUCCESS))
                {
                    insideMacro = NO; /* מעדכנים דגל שיצאנו */
                    continue;
                }
                else
                {
                    printf("Error in file %s, line %d: Extraneous text after 'endmcro'.\n", fileName, LineCounter);
                    *errorFlag = ERORR_IN_MACRO_DEFINITION;
                    return NULL;
                }
            }
            /* Case 3: We are inside a valid macro, so add the line to its content */
            if (insideMacro == YES)
            {
                /* Send the address of the last created macro struct to append the line */
                adding_lines_of_macro_to_arr(&(macro_array[macroCounter - 1]), line);
            }
        }
    }
    *MacroCountRequiredForLoop = macroCounter; /*Return the total number of valid macros found to the main function*/
    return macro_array;
}


/*
 * Allocates and initializes memory for a newly found macro in the dynamic array.
 * Increases the array size and allocates memory for the macro's name and content strings.
 * * @param makroName The name of the new macro to be inserted.
 * @param count Pointer to the current count of macros (will be incremented).
 * @param arr Pointer to the dynamic array of macros to be reallocated.
 */
    void Allocating_memory_for_the_macro_array(char* makroName, int* count, OneMakro * *arr)
    {
        *count = *count + 1; /* Increment the array size counter* /
            /* Reallocate memory for the main array to fit the new macro struct */
            OneMakro * temp = (OneMakro*)realloc(*arr, (*count) * sizeof(OneMakro));
        if (temp == NULL)
        {
            printf("An attempt to allocate memory failed");
            exit(Memory_allocation_failed);
        }
        *arr = temp;
        /* Allocate memory for the macro's name string (+1 for the null terminator) */
        (*arr)[*count - 1].Makroname = (char*)malloc(((int)strlen(makroName) + 1) * sizeof(char));
        if ((*arr)[*count - 1].Makroname == NULL)
        {
            printf("An attempt to allocate memory failed");
            exit(Memory_allocation_failed);
        }
        /* Copy the extracted name into the newly allocated memory */
        strcpy((*arr)[*count - 1].Makroname, makroName);
        /* Allocate initial memory for the macro's content (start as an empty string) */
        (*arr)[*count - 1].MakroContent = (char*)malloc(Initial_macro_size * sizeof(char));
        if ((*arr)[*count - 1].MakroContent == NULL)
        {
            printf("An attempt to allocate memory failed");
            exit(Memory_allocation_failed);
        }
        (*arr)[*count - 1].MakroContent[0] = '\0';
    }
