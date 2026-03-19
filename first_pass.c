#define _CRT_SECURE_NO_WARNINGS

/*
 * File: first_pass.c
 * Purpose: Implements the first pass of the assembly process.
 * Scans the .am file, populates the symbol table, and calculates IC & DC.
 */

#include <stdio.h>
#include <string.h>
#include "first_pass.h"
#include "validations.h"
#include "scan_lines.h"
#include "memory_manager.h"


 /* Constants to avoid magic numbers */
/* Constants to avoid magic numbers */
#define OK_OPERATION 1
#define OK_OPERANDS 1
#define OK_REGISTER 1
#define OK_INSTRUCTION 1
#define OK_LABEL 1
#define OK_ADDRESSING_MODE_0 1
#define OK_ADDRESSING_MODE_2 1
#define OK_TYPE_OF_OPERANDS 1


#define OPERATION_ERROR 0
#define OPERANDS_ERROR 0
#define REGISTER_ERROR 0
#define INSTRUCTION_ERROR 0
#define LABEL_ERROR 0
#define MACRO_ERROR -1
#define ADDRESSING_MODE_0_ERROR 0
#define ADDRESSING_MODE_2_ERROR 0
#define TYPE_OF_OPERANDS_ERROR 0
#define NO_OPERAND -1


#define NUM_OF_OPERATIONS 16
#define NUM_OF_REGISTERS 8
#define NUM_OF_INSTRUCTIONS 4


#define ADDRESSING_MODE_0 0
#define ADDRESSING_MODE_1 1
#define ADDRESSING_MODE_2 2
#define ADDRESSING_MODE_3 3
#define LABEL_LENGTH 31


#define OK_STRING 1
#define OK_COMMA 1

#define STRING_ERROR 0
#define COMMA_ERROR 0



/**
 * Helper Function: check_label_validity
 * Validates if the label syntax is correct and ensures it is not a duplicate in the symbol table.
 * The function prints the appropriate error message along with the line number if an error occurs.
 *
 * @param label_name The name of the label extracted from the line.
 * @param macrosArray The array of macros to check against reserved words.
 * @param total_macros The total number of macros currently defined.
 * @param line_number The current line number in the file for error reporting.
 * @return 1 if an error is found, or 0 if the label is completely valid.
 */
int check_label_validity(char* label_name, OneMakro* macrosArray, int total_macros, int line_number)
{
    /* 1. Validate label name syntax (length, starts with letter, valid chars, not reserved) */
    if (is_it_a_valid_label(macrosArray, label_name, total_macros) != OK_LABEL)
    {
        /* Print a general error for an invalid label name */
        printf("Error at line %d: Invalid label name '%s'\n", line_number, label_name);
        return 1; /* Return 1 to signal first_pass that an error occurred */
    }

    /* 2. Check if the label is already defined in the symbol table */
    if (is_label_exists(label_name) == LABEL_ERROR)
    {
        /* Print a duplicate label error */
        printf("Error at line %d: Label '%s' is already defined!\n", line_number, label_name);
        return 1; /* Return 1 to signal an error occurred */
    }

    /* If we reached here - the label is valid, not reserved, and unique. Perfect! */
    return 0;
}

/**
 * encode_string_directive - Encodes a valid .string operand into the data image.
 * This function receives the full line and the starting index of the string
 * operand (which is expected to be the opening quote '"'). It extracts each
 * character, stores its ASCII value in the data array, and increments the
 * Data Counter (DC). Finally, it appends the null terminator '\0'.
 *
 * @param line The full character array representing the current line being parsed.
 * @param start_index The exact index in the line where the opening quote '"' is located.
 * @param DC A pointer to the Data Counter, ensuring updates are saved in the main pass.
 */
void encode_the_string_into_the_data_image(char* line, int start_index, int* DC)
{
    /* Start from start_index + 1 to explicitly skip the opening quote '"' */
    int i = start_index + 1;

    /* Loop through the characters in the line until the closing quote is found */
    while (line[i] != '"')
    {
        /* Step 1: Add the ASCII value of the current character to the data array */
        add_data_word(*DC, (unsigned short)line[i]);

        /* Step 2: Increment the Data Counter for the next memory address */
        (*DC)++;

        /* Step 3: Move to the next character in the line */
        i++;
    }

    /* Assembly requirement: Always append a null terminator ('\0' / ASCII 0)
     * at the end of every string stored in memory. */
    add_data_word(*DC, 0);

    /* Increment DC one last time for the null terminator */
    (*DC)++;
}

 /**
  * first_pass - Scans the .am file to determine instruction and data sizes.
  * @amFile: A pointer to the opened expanded assembly file.
  * @macrosArray: The table of macros.
  * @total_macros_found: Total count of macros in the system.
  * Returns: 1 if errors were found, 0 if clean.
  */
int first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found)
{
    char line[82];                   /* Buffer for reading the current line */
    int IC = 100;                    /* Instruction Counter, starts at 100 */
    int DC = 0;                      /* Data Counter, tracks data storage */
    int line_index = 0;              /* Current index while parsing the line */
    char the_first_word[82] = { 0 };   /* Stores the first extracted word */
    char the_seconed_word[82] = { 0 }; /* Stores the second extracted word */
    int error_flag = 0;              /* 1 if errors found, 0 otherwise */
    int first_word_length;                      /* Length of the extracted word */
    int line_number = 1;             /* Tracks the current line number */
    int opcode=0;
    int funct=0;



    /* Read the source file line by line until End-Of-File (EOF) */
    while ((fgets(line, sizeof(line), amFile) != NULL))
    {
        /* Skip leading spaces to find the first meaningful character */
        line_index = skip_the_spaces(line, 0);

        /* Skip empty lines and comments entirely */
        if (line[line_index] != '\n' && line[line_index] != '\0' && line[line_index] != ';')
        {
            /* Extract the first word and find its length */
            line_index = cut_the_next_word(line, line_index, the_first_word);
            first_word_length = (int)strlen(the_first_word);

            /* Check if the first word is a label definition (ends with a colon) */
            if (first_word_length > 0 && the_first_word[first_word_length - 1] == ':')
            {
                /* Remove the colon ':' to check the actual label name */
                the_first_word[first_word_length - 1] = '\0';

                /* Call the helper function to check label syntax and duplicates */
                if (check_label_validity(the_first_word, macrosArray, total_macros_found, line_number) == 1)
                {
                    error_flag = 1; /* Helper found an error, turn on the error flag */
                }
                else
                {
                    /* The label is valid and new. Move to the next word in the line */
                    line_index = skip_the_spaces(line, line_index);
                    line_index = cut_the_next_word(line, line_index, the_seconed_word);

                    /* Check if the second word is a data or string directive */
                    if (strcmp(the_seconed_word, ".data") == 0 || strcmp(the_seconed_word, ".string") == 0)
                    {
                        /* Add the valid label to the symbol table as a data symbol */
                        add_symbol(the_first_word, DC, 0, 1, 0, 0);
                        line_index = skip_the_spaces(line, line_index);
                        if (strcmp(the_seconed_word, ".string") == 0)
                        {
                                if (is_str_valid(line, line_index) == OK_STRING)
                                {
                                    encode_the_string_into_the_data_image(line, line_index, DC);
                                }
                                else
                                {
                                    /* פה חשוב להדפיס שגיאה ולהדליק דגל שגיאות! */
                                    printf("Error at line %d: Invalid string format.\n", line_number);
                                    error_flag = 1;
                                }
                        }
                        else if (strcmp(the_seconed_word, ".data") == 0)
                        {
                            while (line_index != '\0')
                            {

                            }
                            
                        }
                    }
                    
                }
            }
        }

        /* Increment line counter before reading the next line */
        line_number++;
    }

    /* Return the final status to the main function */
    return error_flag;
}