#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "first_pass.h"
#include "validations.h"
#include "scan_lines.h"
#include "memory_manager.h"




/* Constants to avoid magic numbers */
#define OK_OPERATION 1
#define OK_OPERANDS 1
#define OK_REGISTER 1
#define OK_INSTRUCTION 1
#define OK_LABEL 1
#define OK_INTEGER 1
#define OK_ADDRESSING_MODE_2 1
#define OK_TYPE_OF_OPERANDS 1
#define OK_DATA 1




#define OPERATION_ERROR 0
#define OPERANDS_ERROR 0
#define REGISTER_ERROR 0
#define INSTRUCTION_ERROR 0
#define LABEL_ERROR 0
#define MACRO_ERROR -1
#define INTEGER_ERROR 0
#define ADDRESSING_MODE_2_ERROR 0
#define TYPE_OF_OPERANDS_ERROR 0
#define NO_OPERAND -1
#define DATA_ERROR 0



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

int first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found)
{
    char line[82];                   /* Buffer for reading the current line */
    int IC = 100;                    /* Instruction Counter, starts at 100 */
    int DC = 0;                      /* Data Counter, tracks data storage */
    int line_index = 0;              /* Current index while parsing the line */
    char the_first_word[82] = { 0 };   /* Stores the first extracted word */
    char the_instruction[82] = { 0 }; /* Stores the second extracted word */
    int error_flag = 0;              /* 1 if errors found, 0 otherwise */
    int label_flag ;
    int first_word_length;                      /* Length of the extracted word */
    int line_number = 1;             /* Tracks the current line number */
    int opcode = 0;
    int funct = 0;


    /* Read the source file line by line until End-Of-File (EOF) */
    while ((fgets(line, sizeof(line), amFile) != NULL))
    {
         label_flag = 0;
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
           

            if (strcmp(the_instruction, ".string") == 0)
            {
                /* Add the valid label to the symbol table as a data symbol */
                if (label_flag == 1)
                {
                    add_symbol(the_first_word, DC, 0, 1, 0, 0);
                }
                line_index = skip_the_spaces(line, line_index);

                if (process_of_string(line, line_index, &DC, line_number) == STRING_ERROR)
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
                    add_symbol(the_first_word, DC, 0, 1, 0, 0);
                }
                line_index = skip_the_spaces(line, line_index);

                if (process_and_encode_data(line, line_index, &DC, line_number) == DATA_ERROR)
                {
                    error_flag = 1;
                }
            }
            else if (strcmp(the_instruction, ".extern") == 0)
            {

            }
            else if (strcmp(the_instruction, ".entry") == 0)
            {

            }
            else
            {


            }
        }
        /* Increment line counter before reading the next line */
        line_number++;
    }
    /* Return the final status to the main function */
    return error_flag;
}
