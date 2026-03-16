#define _CRT_SECURE_NO_WARNINGS
/*
 * File: first_pass.c
 * Purpose: Implements the first pass of the assembly process.
 * This module is responsible for scanning the expanded (.am) file,
 * populating the symbol table, and calculating memory offsets (IC & DC).
 */

#include <stdio.h>
#include "first_pass.h"
#include "validations.h"
#include "scan_lines.h"

 /**
  * first_pass - Scans the .am file to determine instruction and data sizes.
  * @amFile: A pointer to the opened expanded assembly file.
  * @macrosArray: The table of macros (used to ensure labels are not macro names).
  * @total_macros_found: Total count of macros in the system.
  */
void first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found)
{
    char line[82];    /* Buffer for reading lines (max 80 chars + \n + \0) */
    int IC = 100;     /* Instruction Counter - starts at address 100 */
    int DC = 0;       /* Data Counter - tracks data storage (.data, .string) */
    int index;
    char the_first_word[82] = {0};
    char the_seconed_word[82] = { 0 };
    int label_flag = 0;
    int length;

    /* Read the file line by line until EOF */
    while (fgets(line, sizeof(line), amFile) != NULL)
    {
        index= skip_the_spaces(line, 0);
        if (line[index] == '\n' || line[index] == '\0' || line[index] == ';')
        {
            continue; /* מדלגים על שאר הלולאה ועוברים מיד לשורה הבאה בקובץ! */
        }
        index= cut_the_next_word(line, index, the_first_word);
        length =(int)strlen(the_first_word);
        if (length > 0 && the_first_word[length - 1] == ':')
        { 
            if (is_it_a_valid_label(macrosArray, the_first_word, total_macros_found) == 1)
            {
                label_flag = 1;
                index = skip_the_spaces(line, 0);
                index = cut_the_next_word(line, index, the_seconed_word);

                if (strcmp(the_seconed_word, ".data") == 0 || strcmp(the_seconed_word, ".string") == 0)

                {

                }
                  
            }
        }


    }
}