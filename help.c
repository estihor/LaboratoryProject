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
#include "LightBits.h"


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

#define OK_EXTERN 1
#define EXTERN_ERROR 0

#define OK_ENTRY 1
#define ENTRY_ERROR 0


#define VALID_SYNTAX 1
#define SYNTAX_ERROR 0
#define INVALID_OPERAND 0



/* שימי לב לתוספות בחתימה: the_first_word ו- IC */
int process_machine_instruction(char* line, int index, int label_flag, int line_number, char* the_first_word, char* the_instruction, OneMakro* macrosArray, int total_macros_found, int* IC)
{
    unsigned int num_of_operands;
    int source_mode = NO_OPERAND;
    int dest_mode = NO_OPERAND;

    /* 1. Check if it's a valid operation and get operand count. Notice the '&' */
    if (is_it_an_operation_and_find_operands(the_instruction, &num_of_operands) == OPERANDS_ERROR)
    {
        printf("Error at line %d: Invalid operation '%s'\n", line_number, the_instruction);
        return 1;
    }

    /* 2. Valid operation! If there is a label, add it to the symbol table as CODE */
    if (label_flag == 1)
    {
        /* We pass *IC because IC is a pointer, and we need its actual value here */
        add_symbol(the_first_word, *IC, 1, 0, 0, 0);
    }

    /* 3. Route to the correct worker based on the number of operands */
    if (num_of_operands == 0)
    {
        if (process_zero_operands(line_number, line, index) == SYNTAX_ERROR)
        {
            return 1;
        }
    }
    else if (num_of_operands == 1)
    {
        /* Notice we send 'the_instruction' instead of 'operation_name', and '&dest_mode' */
        if (process_one_operand(line_number, line, index, the_instruction, macrosArray, total_macros_found, &dest_mode) == SYNTAX_ERROR)
        {
            return 1;
        }
    }
    else if (num_of_operands == 2)
    {
        /* Notice we send '&source_mode' and '&dest_mode' */
        if (process_two_operands(line_number, line, index, the_instruction, macrosArray, total_macros_found, &source_mode, &dest_mode) == SYNTAX_ERROR)
        {
            return 1;
        }
    }

    /* If we got here, the entire line's syntax is flawless! */
    return 0;
}






/**
 * process_zero_operands - Validates instructions that take no operands (e.g., rts, stop).
 * This function ensures that the instruction is the last meaningful text on the line.
 * * @param line_number The current line number in the source file, used for error reporting.
 * @param line The string representing the entire current line.
 * @param index The current index in the line immediately after the instruction word.
 * @return VALID_SYNTAX (1) if no extraneous text is found, SYNTAX_ERROR (0) otherwise.
 */
int process_zero_operands(int line_number, char* line, int index)
{
    /* Step 1: Skip any white spaces that might exist after the command word */
    index = skip_the_spaces(line, index);

    /* Step 2: Check if we reached the end of the line ('\n') or end of string ('\0').
     * If we see any other character, it means the user typed extra garbage, which is illegal. */
    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after instruction (expected 0 operands)\n", line_number);
        return SYNTAX_ERROR;
    }

    /* If we survived the check, the syntax is perfect */
    return VALID_SYNTAX;
}

/**
 * process_one_operand - Validates instructions that take exactly 1 operand (e.g., clr, inc).
 * Extracts the operand, validates its syntax, and verifies if its addressing mode is legal.
 * * @param line_number The current line number in the source file.
 * @param line The string representing the entire current line.
 * @param index The current index in the line immediately after the instruction word.
 * @param operation_name The name of the instruction (e.g., "clr").
 * @param macrosArray Pointer to the array of defined macros (for label validation).
 * @param total_macros The total number of macros currently defined.
 * @param p_dest_mode Pointer used to return the identified addressing mode to the caller.
 * @return VALID_SYNTAX (1) if the operand is perfectly valid, SYNTAX_ERROR (0) otherwise.
 */
int process_one_operand(int line_number, char* line, int index, char* operation_name, OneMakro* macrosArray, int total_macros, int* p_dest_mode)
{
    char the_operand[82] = { 0 };

    /* --- PART 1: EXTRACTION --- */

    /* Advance the index past any spaces following the instruction name */
    index = skip_the_spaces(line, index);

    /* Cut the next sequence of characters until a space or end-of-line is met */
    index = cut_the_next_word(line, index, the_operand);

    /* If the extracted string is empty, the user wrote the command without an operand */
    if (strlen(the_operand) == 0)
    {
        printf("Error at line %d: Missing operand for instruction '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    /* Skip spaces again to check what comes AFTER the operand */
    index = skip_the_spaces(line, index);

    /* If there is anything left on the line besides a newline, it's illegal garbage */
    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after operand\n", line_number);
        return SYNTAX_ERROR;
    }

    /* --- PART 2: CLASSIFICATION AND SYNTAX VALIDATION --- */

    /* Look at the first character of the operand (e.g., '#', '%') to guess its mode.
     * We save the result directly into the pointer so the caller function can use it later. */
    *p_dest_mode = what_is_the_addressing_mode(the_operand);

    /* Verify that the string actually obeys the rules of the identified addressing mode */
    if (validate_operand_by_mode(the_operand, *p_dest_mode, macrosArray, total_macros) == INVALID_OPERAND)
    {
        printf("Error at line %d: Invalid operand syntax '%s'\n", line_number, the_operand);
        return SYNTAX_ERROR;
    }

    /* --- PART 3: LOGICAL CROSS-REFERENCE --- */

    /* Does this specific operation allow this specific addressing mode as a destination?
     * Note: We pass NO_OPERAND as the source because 1-operand instructions have no source. */
    if (is_valid_addressing(operation_name, NO_OPERAND, *p_dest_mode) == TYPE_OF_OPERANDS_ERROR)
    {
        printf("Error at line %d: Illegal addressing mode for destination operand in '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    /* All checks passed! */
    return VALID_SYNTAX;
}

/**
 * process_two_operands - Validates instructions that take exactly 2 operands (e.g., mov, add).
 * Extracts both operands, ensures they are separated by a comma, validates their syntax,
 * and verifies if their addressing modes are legal for the given instruction.
 * * @param line_number The current line number in the source file.
 * @param line The string representing the entire current line.
 * @param index The current index in the line immediately after the instruction word.
 * @param operation_name The name of the instruction (e.g., "mov").
 * @param macrosArray Pointer to the array of defined macros.
 * @param total_macros The total number of macros currently defined.
 * @param p_source_mode Pointer used to return the source addressing mode to the caller.
 * @param p_dest_mode Pointer used to return the destination addressing mode to the caller.
 * @return VALID_SYNTAX (1) if both operands and the comma are valid, SYNTAX_ERROR (0) otherwise.
 */
int process_two_operands(int line_number, char* line, int index, char* operation_name, OneMakro* macrosArray, int total_macros, int* p_source_mode, int* p_dest_mode)
{
    char first_operand[82] = { 0 };
    char second_operand[82] = { 0 };

    /* --- PART 1: EXTRACTION AND COMMA CHECKING --- */

    /* Skip spaces and cut the very first word (The Source Operand) */
    index = skip_the_spaces(line, index);
    index = cut_the_next_word(line, index, first_operand);

    /* If nothing was extracted, the user just wrote the command and stopped */
    if (strlen(first_operand) == 0)
    {
        printf("Error at line %d: Missing source operand for instruction '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    /* Look for the required comma separating the two operands.
     * The helper function 'check_and_skip_comma' will return -1 if no comma is found. */
    index = check_and_skip_comma(line, index);
    if (index == -1)
    {
        printf("Error at line %d: Missing comma between operands\n", line_number);
        return SYNTAX_ERROR;
    }

    /* We found the comma! Now cut the next word (The Destination Operand) */
    index = cut_the_next_word(line, index, second_operand);

    /* If the second word is empty, the user wrote something like: "mov r1, \n" */
    if (strlen(second_operand) == 0)
    {
        printf("Error at line %d: Missing destination operand after comma\n", line_number);
        return SYNTAX_ERROR;
    }

    /* Check if there's any garbage text lingering after the second operand */
    index = skip_the_spaces(line, index);
    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after operands\n", line_number);
        return SYNTAX_ERROR;
    }

    /* --- PART 2: CLASSIFICATION AND SYNTAX VALIDATION --- */

    /* Guess the addressing modes for both strings and save them via pointers */
    *p_source_mode = what_is_the_addressing_mode(first_operand);
    *p_dest_mode = what_is_the_addressing_mode(second_operand);

    /* Strictly validate the internal syntax of the Source operand */
    if (validate_operand_by_mode(first_operand, *p_source_mode, macrosArray, total_macros) == INVALID_OPERAND)
    {
        printf("Error at line %d: Invalid source operand syntax '%s'\n", line_number, first_operand);
        return SYNTAX_ERROR;
    }

    /* Strictly validate the internal syntax of the Destination operand */
    if (validate_operand_by_mode(second_operand, *p_dest_mode, macrosArray, total_macros) == INVALID_OPERAND)
    {
        printf("Error at line %d: Invalid destination operand syntax '%s'\n", line_number, second_operand);
        return SYNTAX_ERROR;
    }

    /* --- PART 3: LOGICAL CROSS-REFERENCE --- */

    /* Check if this specific operation allows this exact combination of source and destination modes. */
    if (is_valid_addressing(operation_name, *p_source_mode, *p_dest_mode) == TYPE_OF_OPERANDS_ERROR)
    {
        printf("Error at line %d: Illegal addressing modes for instruction '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    /* Line is flawlessly constructed! */
    return VALID_SYNTAX;


}

/* Added line_number to the parameters, and changed return type to void */
void encode_operation(unsigned int opcode, unsigned int funct, int source_mode, int destination_mode, int* IC, int line_number)
{
    unsigned short machine_code = 0;

    /* 1. Pack the bits together using bitwise OR (|) */
    /* Note: If source or destination mode is NO_OPERAND (-1),
       make sure your push_ functions treat it as 0 so it doesn't mess up the bits! */
    machine_code = (push_opcode(opcode) | push_funct(funct) | push_source_mode(source_mode) | push_destination_mode(destination_mode));

    /* 2. Add the completely built word to the Code Image array.
       We pass *IC (the actual address value), and NULL for the label name. */
    add_code_word(*IC, machine_code, NULL, line_number);

    /* 3. Promote the Instruction Counter for the next words */
    (*IC)++;
}

/**
 * encode_operand - Encodes a single operand into the memory image based on its addressing mode.
 * It extracts values (like integers or register numbers) to build the machine code word,
 * or saves raw label names to be resolved later in the Second Pass.
 * Finally, it adds the word to the Code Image array and increments the Instruction Counter (IC).
 *
 * @param operand The string representing the operand (e.g., "#5", "LOOP", "%LOOP", "r3").
 * @param mode The addressing mode of the operand (0, 1, 2, or 3).
 * @param IC Pointer to the Instruction Counter, incremented after adding the word.
 * @param line_number The current line number, saved for error reporting in the Second Pass.
 */
void encode_operand(char* operand, int mode, int* IC, int line_number)
{
    unsigned short machine_code = 0;

    if (mode == ADDRESSING_MODE_0)
    {
        /* Skip the '#' and convert the string to an integer */
        machine_code = (unsigned short)atoi(operand + 1);
        add_code_word(*IC, machine_code, NULL, line_number);
    }
    else if (mode == ADDRESSING_MODE_1)
    {
        /* Direct label. Machine code is 0 for now. Save the label name for Second Pass. */
        add_code_word(*IC, 0, operand, line_number);
    }
    else if (mode == ADDRESSING_MODE_2)
    {
        /* Relative label. Skip the '%' character and save the label name. */
        add_code_word(*IC, 0, operand + 1, line_number);
    }
    else if (mode == ADDRESSING_MODE_3)
    {
        /* Register. Skip the 'r' and push to the correct bits. */
        machine_code = push_register(operand + 1);
        add_code_word(*IC, machine_code, NULL, line_number);
    }

    /* CRITICAL: Advance the Instruction Counter after adding the word! */
    (*IC)++;
}