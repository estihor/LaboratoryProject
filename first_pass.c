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

#define OK_LABEL  1
#define LABLE_ERROR  0

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
    int length;                      /* Length of the extracted word */
    int line_number = 1;             /* Tracks the current line number */

    /* Read the source file line by line */
    while ((fgets(line, sizeof(line), amFile) != NULL))
    {
        line_index = skip_the_spaces(line, 0);

        /* Skip empty lines and comments */
        if (line[line_index] != '\n' && line[line_index] != '\0' && line[line_index] != ';')
        {
            line_index = cut_the_next_word(line, line_index, the_first_word);
            length = (int)strlen(the_first_word);

            /* Check if the first word is a label definition */
            if (length > 0 && the_first_word[length - 1] == ':')
            {
                the_first_word[length - 1] = '\0'; /* Remove the colon ':' */

                /* Validate label syntax */
                if (is_it_a_valid_label(macrosArray, the_first_word, total_macros_found) == OK_LABEL)
                {
                    /* Check for duplicate labels */
                    if (is_label_exists(the_first_word) == LABLE_ERROR)
                    {
                        printf("Error at line %d: Label '%s' is already defined!\n", line_number, the_first_word);
                        error_flag = 1; /* Turn on error flag */
                    }
                    else
                    {
                        line_index = skip_the_spaces(line, line_index);
                        line_index = cut_the_next_word(line, line_index, the_seconed_word);

                        /* Check if it's a data directive */
                        if (strcmp(the_seconed_word, ".data") == 0 || strcmp(the_seconed_word, ".string") == 0)
                        {
                            /* Add valid label to symbol table as data */
                            add_symbol(the_first_word, DC, 0, 1, 0, 0);

                            /* -------- Step 7 goes here -------- */
                        }
                    }
                }
                else
                {
                    /* Invalid label syntax */
                    printf("Error at line %d: Invalid label name '%s'\n", line_number, the_first_word);
                    error_flag = 1;
                }
            }
        }

        line_number++; /* Increment line counter */
    }

    return error_flag;
}