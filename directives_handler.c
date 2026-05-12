
/* --- Status codes for Directives (.data, .string, .extern, .entry) --- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "directives_handler.h"
#include "validations.h"
#include "memory_manager.h"
#include "scan_lines.h"
#include "first_pass.h"
#include "Pre_Assembler.h"
#include "operands_validator.h"

#define OK_DATA 1
#define DATA_ERROR 0
#define OK_STRING 1
#define STRING_ERROR 0
#define OK_EXTERN 1
#define EXTERN_ERROR 0
#define OK_ENTRY 1
#define ENTRY_ERROR 0
#define OK_INTEGER 1
#define INTEGER_ERROR 0
#define OK_COMMA 1
#define COMMA_ERROR 0
#define OK_LABEL 1
#define LABEL_ERROR 0

/**
 * encode_string_directive - Encodes a valid.string operand into the data image.
 * This function receives the full line and the starting index of the string
 * operand(which is expected to be the opening quote '"').It extracts each
 * character, stores its ASCII value in the data array, and increments the
 * Data Counter(DC).Finally, it appends the null terminator '\0'.
 *
 * @param line The full character array representing the current line being parsed.
 * @param start_index The exact index in the line where the opening quote '"' is located.
 * @param DC A pointer to the Data Counter, ensuring updates are saved in the main pass.
 */
void encode_the_string_into_the_data_image(char* line, int start_index, int* DC, AssemblerData * state)
{
    /* Start from start_index + 1 to explicitly skip the opening quote '"' */
    int i = start_index + 1;

    /* Loop through the characters in the line until the closing quote is found */
    while (line[i] != '"')
    {
        /* Step 1: Add the ASCII value of the current character to the data array */
        add_data_word(*DC, (unsigned short)line[i], state);

        /* Step 2: Increment the Data Counter for the next memory address */
        (*DC)++;

        /* Step 3: Move to the next character in the line */
        i++;
    }

    /* Assembly requirement: Always append a null terminator ('\0' / ASCII 0)
     * at the end of every string stored in memory. */
    add_data_word(*DC, 0, state);

    /* Increment DC one last time for the null terminator */
    (*DC)++;
}

/**
 * process_string_directive - Validates and encodes a .string directive.
 * Checks if the string starts and ends with quotes. If valid, it encodes
 * the characters into the data image.
 *
 * @param line The full line containing the string directive.
 * @param index The current index in the line (after the ".string" word).
 * @param DC Pointer to the Data Counter.
 * @param line_number Current line number for error reporting.
 * @return OK_STRING if successful, STRING_ERROR if syntax errors exist.
 */
int process_of_string(char* line, int index, int* DC, int line_number, AssemblerData* state)
{
    /* Step 1: Skip spaces after ".string" */
    index = skip_the_spaces(line, index);

    /* Step 2: Validate the string format */
    if (is_str_valid(line, index, line_number) != OK_STRING)
    {

        return STRING_ERROR;
    }

    /* Step 3: String is valid, encode it to memory */
    encode_the_string_into_the_data_image(line, index, DC, state);

    return OK_STRING;
}
/**
 * process_data_directive - Parses, validates, and encodes a .data directive.
 * Reads comma-separated integers, converts them to machine code, and increments DC.
 * Uses flags to avoid 'break' statements, adhering to strict structured programming.
 *
 * @param line The full line containing the data directive.
 * @param index The current index in the line (after the ".data" word).
 * @param DC Pointer to the Data Counter.
 * @param line_number Current line number for error reporting.
 * @return OK_DATA if successful, DATA_ERROR if syntax errors exist.
 */
int process_and_encode_data(char* line, int index, int* DC, int line_number, AssemblerData* state)
{
    char current_number_str[82] = { 0 };
    int num_value;
    int end_of_line = 0; /* Flag to stop the loop gracefully at the end */
    int error_found = 0; /* Flag to stop the loop if an error occurs */

    /* Step 1: Skip spaces after ".data" */
    index = skip_the_spaces(line, index);


    /* Check for an empty data directive */
    if (line[index] == '\n' || line[index] == '\0') {
        printf("Error at line %d: Missing data parameters.\n", line_number);
        return DATA_ERROR;
    }

    /* Step 2: Loop through the line using flags */
    while (end_of_line == 0 && error_found == 0 && line[index] != '\n' && line[index] != '\0')
    {
        /* Extract the number string, stopping at spaces or commas */
        index = cut_the_next_word(line, index, current_number_str);

        /* Validate the extracted string is a legal integer (e.g., "-5", "12") */
        if (is_valid_integer(current_number_str, line_number) == INTEGER_ERROR)
        {
            error_found = 1; /* Turn on error flag to exit loop gracefully */
        }
        else
        {
            /* Number is valid, encode immediately to memory */
            num_value = atoi(current_number_str);
            add_data_word(*DC, num_value, state);
            (*DC)++;

            /* Skip spaces after the number to check the next character */
            index = skip_the_spaces(line, index);

            /* Check if the end of the line is reached */
            if (line[index] == '\n' || line[index] == '\0') {
                end_of_line = 1; /* Turn on flag to finish the loop */
            }
            else
            {
                /* Not at the end, so a comma is expected here */
                index = check_and_skip_comma(line, index);

                if (index == -1) {
                    printf("Error at line %d: Missing comma between data parameters.\n", line_number);
                    error_found = 1;
                }
                else
                {
                    /* Check for a trailing comma (e.g., "5, 8, \n") */
                    index = skip_the_spaces(line, index);
                    if (line[index] == ',') {
                        printf("Error at line %d: Multiple consecutive commas found!\n", line_number);
                        error_found = 1;
                    }
                    if (line[index] == '\n' || line[index] == '\0') {
                        printf("Error at line %d: Extraneous comma at end of line.\n", line_number);
                        error_found = 1;
                    }
                }
            }
        }
    }

    /* Step 3: Return the final status based on the error flag */
    if (error_found == 1) {
        return DATA_ERROR;
    }

    return OK_DATA;
}

/**
 * process_extern_directive - Handles the .extern directive in the first pass.
 * Prints a warning if a label precedes the directive. Extracts the operand,
 * validates its syntax, and inserts it into the symbol table as an external symbol.
 * Detects missing operands or extraneous text after the operand.
 *
 * @param line The full line containing the directive.
 * @param index The current index in the line (after the ".extern" word).
 * @param label_flag Indicates if a label was defined at the start of the line.
 * @param line_number Current line number for error reporting.
 * @param macrosArray The array of macros to check against reserved words.
 * @param total_macros_found The total number of macros currently defined.
 * @return OK_EXTERN if successful, EXTERN_ERROR if syntax errors exist.
 */
int process_of_extern(char* line, int index, int label_flag, int line_number, OneMakro* macrosArray, int total_macros_found, AssemblerData* state)
{
    char the_operand[82] = { 0 };

    /* A label before .extern is meaningless; print a warning but continue */
    if (label_flag == 1)
    {
        printf("Warning at line %d: Label before .extern is ignored\n", line_number);
    }
    /* Extract the external label name */
    index = skip_the_spaces(line, index);
    index = cut_the_next_word(line, index, the_operand);
    /* Check for missing operand */
    if (strlen(the_operand) == 0)
    {
        printf("Error at line %d: Missing operand after .extern directive\n", line_number);
        return EXTERN_ERROR;
    }

    /* Validate the operand label syntax and check that it doesn't already exist */
    if (check_label_validity(the_operand, macrosArray, total_macros_found, line_number, state) == 1)
    {
        return EXTERN_ERROR;
    }

    /* Insert the valid external label into the symbol table */
    add_symbol(the_operand, 0, 0, 0, 0, 1, state);

    /* Check for extraneous text after the operand */
    index = skip_the_spaces(line, index);
    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after .extern operand\n", line_number);
        return EXTERN_ERROR;
    }

    return OK_EXTERN;
}

/**
 * process_entry_directive - Handles the .entry directive in the first pass.
 * Prints a warning if a label precedes the directive. Extracts the operand
 * and validates its syntax. Does not insert into the symbol table in the first pass.
 * Detects missing operands or extraneous text after the operand.
 *
 * @param line The full line containing the directive.
 * @param index The current index in the line (after the ".entry" word).
 * @param label_flag Indicates if a label was defined at the start of the line.
 * @param line_number Current line number for error reporting.
 * @param macrosArray The array of macros to check against reserved words.
 * @param total_macros_found The total number of macros currently defined.
 * @return OK_ENTRY if successful, ENTRY_ERROR if syntax errors exist.
 */
int process_of_entry(char* line, int index, int label_flag, int line_number, OneMakro* macrosArray, int total_macros_found)
{
    char the_operand[82] = { 0 };

    /* A label before .entry is meaningless; print a warning but continue */
    if (label_flag == 1)
    {
        printf("Warning at line %d: Label before .entry is ignored\n", line_number);
    }

    /* Extract the entry label name */
    index = skip_the_spaces(line, index);
    index = cut_the_next_word(line, index, the_operand);

    /* Check for missing operand */
    if (strlen(the_operand) == 0)
    {
        printf("Error at line %d: Missing operand after .entry directive\n", line_number);
        return ENTRY_ERROR;
    }

    /* Validate the operand label syntax ONLY (do not check if it already exists) */
    if (is_it_a_valid_label(macrosArray, the_operand, total_macros_found, line_number) != OK_LABEL)
    {
        return ENTRY_ERROR;
    }

    /* Check for extraneous text after the operand */
    index = skip_the_spaces(line, index);
    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after .entry operand\n", line_number);
        return ENTRY_ERROR;
    }
    return OK_ENTRY;
}

