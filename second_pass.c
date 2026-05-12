#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "memory_manager.h"
#define MAXIMUM_LINE_LENGTH 82  /* Max chars in a line */
#define READ_FORMAT "%81s %81s %81s"
#define MINIMUM_WORDS_IN_ENTRY 2 /* Minimum words required in an entry line */
#define LABEL_NOT_FOUND -1 /* Returned when a label is not found in the label table */
#define DIRECT_ADDRESSING 1/* Direct addressing method (without % sign) */
#define RELATIVE_ADDRESSING 2 /* Relative addressing method (with % sign) */
#define SHIFT_TO_ARE_LOCATION 12 /* Number of bits to shift to reach the ARE location */
#define ARE_EXTERN 1/* ARE value for external label (E) */
#define ARE_R 2 /* ARE value for a positional label (R)*/
#define TWELVE_BITS_MASK 0xFFF /* Mask to isolate the lower 12 bits (for negative numbers) */
#define YES 1  /* Boolean equivalent for TRUE if is it external or entry */     
#define ERROR_FOUND 1 /* Flag value indicating an error was found */
/*
 * Shifts the A, R, or E (Absolute, Relocatable, External) value to its correct bit position.
 *
 * @param REA_number The integer value representing the ARE type (e.g., ARE_EXTERN, ARE_R).
 * @return The shifted value ready to be combined with the machine code using bitwise OR.
 */
unsigned short Push_Bit(unsigned int REA_number)
{
    return (REA_number << SHIFT_TO_ARE_LOCATION);
}
/*
 * Scans the expanded assembly file (.am) for .entry directives and updates the symbol table.
 * If an entry label is not defined in the symbol table, it prints an error and flags it.
 * @param labelsArr The symbol table array containing all defined labels.
 * @param amFile The expanded assembly file (.am) opened for reading.
 * @param labelsArrLength The total number of labels currently in the symbol table.
 * @param error_flag Pointer to an integer flag, set to 1 (ERROR_FOUND) if an undefined entry label is encountered.
 */
void looking_for_entry(label* labelsArr, FILE* amFile, int labelsArrLength, char* FileName, int* error_flag)
{
    char line[MAXIMUM_LINE_LENGTH] = { 0 };
    char word2[MAXIMUM_LINE_LENGTH] = { 0 }; /* Buffer to extract the first word of the line */
    char word1[MAXIMUM_LINE_LENGTH] = { 0 };
    char word3[MAXIMUM_LINE_LENGTH] = { 0 };
    int labelIndex;
    int  LineCounter = 0;
    rewind(amFile);/* Reset the read pointer to the beginning of the file to ensure a full scan */
    while (fgets(line, sizeof(line), amFile) != NULL)
    {
        (LineCounter)++;
        /* Read the first 3 words in the line to check if it's an entry directive */
        if (sscanf(line, READ_FORMAT, word1, word2, word3) >= MINIMUM_WORDS_IN_ENTRY)
        {
            /* Case 1: The .entry directive is the first word in the line (no preceding label) */
            if (strcmp(word1, ".entry") == 0)
            {
                /* Search for the declared label in the symbol table */
                labelIndex = isItInLabelTable(word2, labelsArr, labelsArrLength);
                if (labelIndex != LABEL_NOT_FOUND)
                {
                    /* Label found: turn on its entry flag */
                    labelsArr[labelIndex].is_entry = YES;
                }
                else
                {
                    /* Error: The user declared an entry for a label that is not defined anywhere in the code */
                    printf("Error in line %d: Entry label '%s' not defined\n", LineCounter, word2);
                    *error_flag = ERROR_FOUND;
                }
            }
            /* Case 2: The .entry directive is the second word in the line (there is a preceding label which the compiler should ignore) */
            else  if (strcmp(word2, ".entry") == 0)
            {

                labelIndex = isItInLabelTable(word3, labelsArr, labelsArrLength);
                if (labelIndex != LABEL_NOT_FOUND)
                {
                    labelsArr[labelIndex].is_entry = YES;
                }
                else
                {
                    printf("Error in line %d: Entry label '%s' not defined\n", LineCounter, word3);
                    *error_flag = ERROR_FOUND;
                }
            }
        }
    }
}
/*
 * Searches for a specific label name within the symbol table.
 * @param isItLabel The string representing the label name to search for.
 * @param labelsArr The symbol table array to search within.
 * @param labelsArrLength The total number of labels in the symbol table.
 * @return The index of the label in the array if found, otherwise returns LABEL_NOT_FOUND (-1).
 */
int isItInLabelTable(char* isItLabel, label* labelsArr, int labelsArrLength)
{
    /* Iterate through all elements of the symbol table */
    for (int i = 0; i < labelsArrLength; i++)
    {
        /* In case of an exact string match, return the label's index in the array */
        if (strcmp(isItLabel, labelsArr[i].labelName) == 0)
        {
            return i;
        }
    }
    /* If the loop finishes without a match, the label does not exist in the table */
    return LABEL_NOT_FOUND;
}
/*
 * Completes the machine code generation by resolving missing label addresses in the Code Image.
 * Calculates distances for relative addressing (%) and assigns proper ARE values for direct addressing.
 * @param labelsArr The symbol table array containing defined labels and their addresses.
 * @param CodeImageArr The array representing the instructions (Code Image) to be completed.
 * @param labelsArrLength The total number of labels in the symbol table.
 * @param CodeImageArrLength The total number of code words in the Code Image.
 * @param error_flag Pointer to an integer flag, set to 1 (ERROR_FOUND) if a label isn't found or an invalid addressing method is used.
 */
void Complete_Table(label* labelsArr, CodeImage* CodeImageArr, int labelsArrLength, int CodeImageArrLength, char* FileName, int* error_flag)
{
    char* labelName;
    int WordValue;
    int i;
    int index;
    int Addressing_method;
    /* Scan the code image array to find "holes" - machine words waiting for a label's address */
    for (i = 0; i < CodeImageArrLength; i++)
    {
        /* Check if a label name is stored in the pending field (meaning there is a hole to fill) */
        if (CodeImageArr[i].missing_label[0] != '\0')
        {
            /* Identify the addressing method: The '%' character at the beginning indicates relative addressing (distance) */
            if (CodeImageArr[i].missing_label[0] == '%')
            {
                Addressing_method = RELATIVE_ADDRESSING;
                labelName = &CodeImageArr[i].missing_label[1];
            }
            else
            {
                Addressing_method = DIRECT_ADDRESSING; 
                labelName = CodeImageArr[i].missing_label; 

            }
            /* Find the label in the symbol table to retrieve its actual memory address */
            index = isItInLabelTable(labelName, labelsArr, labelsArrLength);
            if (index != LABEL_NOT_FOUND)
            {
                /* Handle address completion for relative addressing (jump forward/backward) */
                if (Addressing_method == RELATIVE_ADDRESSING)
                {
                    /* Error: Cannot calculate a relative jump distance to an external label, as it is not in the current file */
                    if (labelsArr[index].is_extern == YES)
                    {
                        printf("Error in line %d: The addressing method is inappropriate for an extern label\n", CodeImageArr[i].the_line_of_the_label);
                        *error_flag = ERROR_FOUND;
                    }
                    else
                    {
                        /* Calculate the distance: target address minus the current instruction's address. Apply a mask to prevent bit leakage in negative numbers */
                        WordValue = labelsArr[index].labelAddress - (CodeImageArr[i].Memory_address);
                        CodeImageArr[i].machine_code = ((unsigned short)WordValue & TWELVE_BITS_MASK);
                    }

                }
                /* Handle address completion for direct addressing (absolute address) */
                else if (Addressing_method == DIRECT_ADDRESSING)
                {
                    if (labelsArr[index].is_extern == YES)
                    {
                        /* An external label gets address 0 and its E (Extern) bit is turned on in the ARE region */
                        CodeImageArr[i].machine_code = 0 | Push_Bit(ARE_EXTERN);
                    }
                    else
                    {
                        /* A regular local label gets its address and its R (Relocatable) bit is turned on */
                        CodeImageArr[i].machine_code = labelsArr[index].labelAddress | Push_Bit(ARE_R);
                    }
                }
            }
            else
            {
                /* The required label is not defined in the file or as an extern */
                printf("Error in line %d: label '%s' not found\n", CodeImageArr[i].the_line_of_the_label, CodeImageArr[i].missing_label);
                *error_flag = ERROR_FOUND;
            }
        }
    }
}