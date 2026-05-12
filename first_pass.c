#define _CRT_SECURE_NO_WARNINGS

/*
 * File: first_pass.c
 * Purpose: Implements the first pass of the assembly process.
 * Scans the .am file, populates the symbol table, and calculates IC & DC.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "first_pass.h"
#include "validations.h"
#include "scan_lines.h"
#include "memory_manager.h"
#include "instructions_handler.h"
#include "directives_handler.h"
#include "operands_validator.h"


 /* --- General Syntax and Labels --- */
#define OK_LABEL 1
#define LABEL_ERROR 0
#define LABEL_LENGTH 31

#define VALID_SYNTAX 1
#define SYNTAX_ERROR 0

/* --- Status codes for Machine Instructions --- */
#define OK_INSTRUCTION 1
#define INSTRUCTION_ERROR 0

#define MACRO_ERROR -1

/* --- Status codes from Directives (duplicated for local use) --- */
#define DATA_ERROR 0
#define STRING_ERROR 0
#define EXTERN_ERROR 0
#define ENTRY_ERROR 0
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
int check_label_validity(char* label_name, OneMakro* macrosArray, int total_macros, int line_number, AssemblerData* state)
{
    /* 1. Validate label name syntax (length, starts with letter, valid chars, not reserved) */
    if (is_it_a_valid_label(macrosArray, label_name, total_macros, line_number) != OK_LABEL)
    {
        
        return 1; /* Return 1 to signal first_pass that an error occurred */
    }

    /* 2. Check if the label is already defined in the symbol table */
    if (is_label_exists(label_name, state) == LABEL_ERROR)
    {
        /* Print a duplicate label error */
        printf("Error at line %d: Label '%s' is already defined!\n", line_number, label_name);
        return 1; /* Return 1 to signal an error occurred */
    }

    /* If we reached here - the label is valid, not reserved, and unique. Perfect! */
    return 0;
}




int first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found, AssemblerData* state)
{
    char line[82];                   /* Buffer for reading the current line */
    int IC = 100;                    /* Instruction Counter, starts at 100 */
    int DC = 0;                      /* Data Counter, tracks data storage */
    int line_index = 0;              /* Current index while parsing the line */
    char the_first_word[82] = { 0 };   /* Stores the first extracted word */
    char the_instruction[82] = { 0 }; /* Stores the second extracted word */
    char the_operand[82] = { 0 };
    int error_flag = 0;              /* 1 if errors found, 0 otherwise */
    int label_flag;
    int first_word_length;                      /* Length of the extracted word */
    int line_number = 1;             /* Tracks the current line number */
    int opcode = 0;
    int funct = 0;


    /* Read the source file line by line until End-Of-File (EOF) */
    while ((fgets(line, sizeof(line), amFile) != NULL))
    { 
        if (strchr(line, '\n') == NULL && !feof(amFile))
        {
            printf("Error at line %d: Line exceeds the maximum length of 80 characters.\n", line_number);
            error_flag = 1;

            /* לולאה קטנה שמנקה את ה"עודף" מהקובץ כדי שלא יגלוש הלאה */
            int c;
            while ((c = fgetc(amFile)) != '\n' && c != EOF) {}
        }
        else
        {
            label_flag = 0;

            int skip_rest_of_line = 0;
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
                    if (check_label_validity(the_first_word, macrosArray, total_macros_found, line_number, state) == 1)
                    {
                        error_flag = 1; /* Helper found an error, turn on the error flag */
                        skip_rest_of_line = 1;
                    }
                    else
                    {
                        label_flag = 1;

                    }
                    /* The label is valid and new. Move to the next word in the line */
                    line_index = skip_the_spaces(line, line_index);
                    line_index = cut_the_next_word(line, line_index, the_instruction);

                }
                else
                {
                    /* רק אם לא היו נקודתיים בכלל, אז המילה הראשונה היא הפקודה שלנו! */
                    strcpy(the_instruction, the_first_word);
                }

                if (skip_rest_of_line == 0)
                {
                    if (strcmp(the_instruction, ".string") == 0)
                    {
                        /* Add the valid label to the symbol table as a data symbol */
                        if (label_flag == 1)
                        {
                            add_symbol(the_first_word, DC, 0, 1, 0, 0, state);
                        }
                        line_index = skip_the_spaces(line, line_index);

                        if (process_of_string(line, line_index, &DC, line_number, state) == STRING_ERROR)
                        {
                            error_flag = 1;
                        }
                    }

                    /* Check if it's a data directive */
                    else if (strcmp(the_instruction, ".data") == 0)
                    {
                        /* Add the valid label to the symbol table as a data symbol */
                        if (label_flag == 1)
                        {
                            add_symbol(the_first_word, DC, 0, 1, 0, 0, state);
                        }
                        line_index = skip_the_spaces(line, line_index);

                        if (process_and_encode_data(line, line_index, &DC, line_number, state) == DATA_ERROR)
                        {
                            error_flag = 1;
                        }
                    }
                    else if (strcmp(the_instruction, ".extern") == 0)
                    {
                        if (process_of_extern(line, line_index, label_flag, line_number, macrosArray, total_macros_found, state) == EXTERN_ERROR)
                        {
                            error_flag = 1;
                        }
                    }
                    else if (strcmp(the_instruction, ".entry") == 0)
                    {
                        if (process_of_entry(line, line_index, label_flag, line_number, macrosArray, total_macros_found) == ENTRY_ERROR)
                        {
                            error_flag = 1;
                        }
                    }

                    else if (the_instruction[0] == '.')
                    {

                        printf("Error at line %d: Illegal directive name '%s'.\n", line_number, the_instruction);
                        error_flag = 1;
                    }


                    else
                    {
                        if (process_machine_instruction(line, line_index, label_flag, line_number, the_first_word, the_instruction, macrosArray, total_macros_found, &IC, state) == SYNTAX_ERROR)
                        {
                            error_flag = 1;
                        }


                    }

                }
            }
        }
        /* Increment line counter before reading the next line */
        line_number++;
    }
    /* Update addresses of data symbols before finishing the first pass */
    update_data_symbols_address(IC, state);

    /* Return the final status to the main function */
    return error_flag;
   
}
