#define _CRT_SECURE_NO_WARNINGS

/*
 * File: first_pass.c
 * Purpose: Implements the first pass of the assembly process.
 * Scans the .am file, populates the symbol table, validates syntax, and calculates IC & DC.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include "first_pass.h"
#include "validations.h"
#include "scan_lines.h"
#include "memory_manager.h"
#include "LightBits.h"
#include "instructions_handler.h"

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


/**
 * process_machine_instruction - The main manager for machine instructions (e.g., mov, cmp).
 * Identifies the operation, adds its label to the symbol table (if exists), and routes
 * the line to the correct worker function based on the required number of operands.
 *
 * @param line The string representing the entire current line.
 * @param index The current parsing index in the line.
 * @param label_flag 1 if a label was declared at the start of the line, 0 otherwise.
 * @param line_number The current line number in the source file.
 * @param the_first_word The label name (if label_flag is 1).
 * @param the_instruction The name of the operation (e.g., "mov").
 * @param macrosArray Pointer to the array of defined macros.
 * @param total_macros_found The total number of macros currently defined.
 * @param IC Pointer to the Instruction Counter.
 * @return 0 if perfectly valid and encoded, 1 if a syntax error was found.
 */
int process_machine_instruction(char* line, int index, int label_flag, int line_number, char* the_first_word, char* the_instruction, OneMakro* macrosArray, int total_macros_found, int* IC, AssemblerData* state)
{
    unsigned int num_of_operands;
    unsigned int opcode = 0;
    unsigned int funct = 0;
   

    /* 1. Check if it's a valid operation and get operand count. Notice the '&' */
    if (is_it_an_operation_and_find_operands(the_instruction, &num_of_operands) == OPERANDS_ERROR)
    {
        printf("Error at line %d: Invalid operation '%s'\n", line_number, the_instruction);
        return 0;
    }

    /* 2. Valid operation! If there is a label, add it to the symbol table as CODE */
    if (label_flag == 1)
    {
        /* We pass *IC because IC is a pointer, and we need its actual value here */
        add_symbol(the_first_word, *IC, 1, 0, 0, 0, state);
    }

    /* Get the specific opcode and funct for this operation */
    is_it_an_operation_and_find_opcode_and_funct(the_instruction, &opcode, &funct);

    /* 3. Route to the correct worker based on the number of operands */
    if (num_of_operands == 0)
    {
        /* We pass IC directly because it is already a pointer */
        if (process_zero_operands(line_number, line, index, opcode, funct, IC,  state) == SYNTAX_ERROR)
        {
            return 0;
        }
    }
    else if (num_of_operands == 1)
    {
        if (process_one_operand(line_number, line, index, the_instruction, macrosArray, total_macros_found, opcode, funct, IC,state) == SYNTAX_ERROR)
        {
            return 0;
        }
    }
    else if (num_of_operands == 2)
    {
        if (process_two_operands(line_number, line, index, the_instruction, macrosArray, total_macros_found,  opcode, funct, IC,  state) == SYNTAX_ERROR)
        {
            return 0;
        }
    }

    /* If we got here, the entire line's syntax is flawless and encoded! */
    return 1;
}


/**
 * process_zero_operands - Validates and encodes instructions with 0 operands (e.g., rts, stop).
 * Ensures no extraneous text exists, then encodes the operation word.
 *
 * @param line_number The current line number in the source file.
 * @param line The string representing the entire current line.
 * @param index The current index in the line immediately after the instruction word.
 * @param opcode The opcode of the operation.
 * @param funct The funct of the operation.
 * @param IC Pointer to the Instruction Counter.
 * @return VALID_SYNTAX (1) if successful, SYNTAX_ERROR (0) otherwise.
 */
int process_zero_operands(int line_number, char* line, int index, unsigned int opcode, unsigned int funct, int* IC, AssemblerData* state)
{
    index = skip_the_spaces(line, index);

    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after instruction (expected 0 operands)\n", line_number);
        return SYNTAX_ERROR;
    }

    /* Encode the first word. No operands, so we send NO_OPERAND for modes */
    encode_operation(opcode, funct, NO_OPERAND, NO_OPERAND, IC, line_number, state);

    return VALID_SYNTAX;
}


/**
 * process_one_operand - Validates and encodes instructions with 1 operand (e.g., clr, inc).
 * Extracts the operand, validates syntax, checks addressing mode, and encodes the words.
 *
 * @param line_number The current line number.
 * @param line The entire current line.
 * @param index The current index after the instruction.
 * @param operation_name The name of the instruction (e.g., "clr").
 * @param macrosArray Pointer to the array of defined macros.
 * @param total_macros Total number of macros defined.
 * @param p_dest_mode Pointer to return the destination addressing mode.
 * @param opcode The opcode of the operation.
 * @param funct The funct of the operation.
 * @param IC Pointer to the Instruction Counter.
 * @return VALID_SYNTAX (1) if successful, SYNTAX_ERROR (0) otherwise.
 */
int process_one_operand(int line_number, char* line, int index, char* operation_name, OneMakro* macrosArray, int total_macros, unsigned int opcode, unsigned int funct, int* IC, AssemblerData* state)
{
    char the_operand[82] = { 0 };
    int dest_mode;

    /* --- PART 1: EXTRACTION --- */
    index = skip_the_spaces(line, index);
    index = cut_the_next_word(line, index, the_operand);

    if (strlen(the_operand) == 0)
    {
        printf("Error at line %d: Missing operand for instruction '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    index = skip_the_spaces(line, index);
    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after operand\n", line_number);
        return SYNTAX_ERROR;
    }

    /* --- PART 2: CLASSIFICATION AND SYNTAX VALIDATION --- */
    dest_mode = what_is_the_addressing_mode(the_operand);

    if (validate_operand_by_mode(the_operand, dest_mode, macrosArray, total_macros, line_number) == INVALID_OPERAND)
    {
        return SYNTAX_ERROR;
    }

    /* --- PART 3: LOGICAL CROSS-REFERENCE --- */
    if (is_valid_addressing(operation_name, NO_OPERAND, dest_mode) == TYPE_OF_OPERANDS_ERROR)
    {
        printf("Error at line %d: Illegal addressing mode for destination operand in '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    /* --- PART 4: ENCODING --- */
    encode_operation(opcode, funct, NO_OPERAND, dest_mode, IC, line_number, state);
    encode_operand(the_operand, dest_mode, IC, line_number, state);

    return VALID_SYNTAX;
}


/**
 * process_two_operands - Validates and encodes instructions with 2 operands (e.g., mov, add).
 * Ensures correct comma separation, validates both operands, and encodes the resulting words.
 *
 * @param line_number The current line number.
 * @param line The entire current line.
 * @param index The current index after the instruction.
 * @param operation_name The name of the instruction (e.g., "mov").
 * @param macrosArray Pointer to the array of defined macros.
 * @param total_macros Total number of macros defined.
 * @param p_source_mode Pointer to return the source addressing mode.
 * @param p_dest_mode Pointer to return the destination addressing mode.
 * @param opcode The opcode of the operation.
 * @param funct The funct of the operation.
 * @param IC Pointer to the Instruction Counter.
 * @return VALID_SYNTAX (1) if successful, SYNTAX_ERROR (0) otherwise.
 */
int process_two_operands(int line_number, char* line, int index, char* operation_name, OneMakro* macrosArray, int total_macros, unsigned int opcode, unsigned int funct, int* IC, AssemblerData* state)
{
    char first_operand[82] = { 0 };
    char second_operand[82] = { 0 };
    int source_mode=0;
    int dest_mode=0;

    /* --- PART 1: EXTRACTION AND COMMA CHECKING --- */
    index = skip_the_spaces(line, index);
    index = cut_the_next_word(line, index, first_operand);

    if (strlen(first_operand) == 0)
    {
        printf("Error at line %d: Missing source operand for instruction '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    index = check_and_skip_comma(line, index);
    if (index == -1)
    {
        printf("Error at line %d: Missing comma between operands\n", line_number);
        return SYNTAX_ERROR;
    }
    index = skip_the_spaces(line, index);
    index = cut_the_next_word(line, index, second_operand);
    if (strlen(second_operand) == 0)
    {
        printf("Error at line %d: Missing destination operand after comma\n", line_number);
        return SYNTAX_ERROR;
    }

    index = skip_the_spaces(line, index);
    if (line[index] != '\n' && line[index] != '\0')
    {
        printf("Error at line %d: Extraneous text after operands\n", line_number);
        return SYNTAX_ERROR;
    }

    /* --- PART 2: CLASSIFICATION AND SYNTAX VALIDATION --- */
    source_mode = what_is_the_addressing_mode(first_operand);
    dest_mode = what_is_the_addressing_mode(second_operand);

    if (validate_operand_by_mode(first_operand, source_mode, macrosArray, total_macros, line_number) == INVALID_OPERAND)
    {
        
        return SYNTAX_ERROR;
    }

    if (validate_operand_by_mode(second_operand, dest_mode, macrosArray, total_macros, line_number) == INVALID_OPERAND)
    {
       
        return SYNTAX_ERROR;
    }

    /* --- PART 3: LOGICAL CROSS-REFERENCE --- */
    if (is_valid_addressing(operation_name, source_mode, dest_mode) == TYPE_OF_OPERANDS_ERROR)
    {
        printf("Error at line %d: Illegal addressing modes for instruction '%s'\n", line_number, operation_name);
        return SYNTAX_ERROR;
    }

    /* --- PART 4: ENCODING --- */
    encode_operation(opcode, funct, source_mode, dest_mode, IC, line_number, state);
    encode_operand(first_operand, source_mode, IC, line_number, state);
    encode_operand(second_operand, dest_mode, IC, line_number, state);

    return VALID_SYNTAX;
}


/**
 * encode_operation - Encodes the primary instruction word and adds it to the Code Image.
 * Packs the opcode, funct, and addressing modes into a single 16-bit word using bitwise OR.
 *
 * @param opcode The operation's opcode.
 * @param funct The operation's funct.
 * @param source_mode The source operand's addressing mode (or NO_OPERAND).
 * @param destination_mode The destination operand's addressing mode (or NO_OPERAND).
 * @param IC Pointer to the Instruction Counter, incremented after adding the word.
 * @param line_number The current line number.
 */
void encode_operation(unsigned int opcode, unsigned int funct, int source_mode, int destination_mode, int* IC, int line_number, AssemblerData* state)
{
    unsigned short machine_code = 0;

    /* 1. Pack the bits together using bitwise OR (|) */
    /* Note: Ensure push_source_mode and push_destination_mode return 0 if mode is NO_OPERAND (-1) */
    machine_code = (push_opcode(opcode) | push_funct(funct) | push_source_mode(source_mode) | push_destination_mode(destination_mode));

    /* 2. Add the completely built word to the Code Image array */
    add_code_word(*IC, machine_code, NULL, line_number, state);

    /* 3. Promote the Instruction Counter for the next words */
    (*IC)++;
}


/**
 * encode_operand - Encodes a single operand into the memory image based on its addressing mode.
 * Extracts values to build the machine code word, or saves raw label names for the Second Pass.
 *
 * @param operand The string representing the operand (e.g., "#5", "LOOP", "%LOOP", "r3").
 * @param mode The addressing mode of the operand (0, 1, 2, or 3).
 * @param IC Pointer to the Instruction Counter, incremented after adding the word.
 * @param line_number The current line number, saved for error reporting in the Second Pass.
 */
void encode_operand(char* operand, int mode, int* IC, int line_number, AssemblerData* state)
{
    unsigned short machine_code = 0;

    if (mode == ADDRESSING_MODE_0)
    {
        /* Skip the '#' and convert the string to an integer */
        machine_code = (unsigned short)atoi(operand + 1);
        add_code_word(*IC, machine_code, NULL, line_number, state);
    }
    else if (mode == ADDRESSING_MODE_1)
    {
        /* Direct label. Machine code is 0 for now. Save the label name for Second Pass. */
        add_code_word(*IC, 0, operand, line_number, state);
    }
    else if (mode == ADDRESSING_MODE_2)
    {
        /* Relative label. Skip the '%' character and save the label name. */
        add_code_word(*IC, 0, operand + 1, line_number, state);
    }
    else if (mode == ADDRESSING_MODE_3)
    {
        /* Register. Skip the 'r' and push to the correct bits. */
        /* Note: Ensure push_register knows exactly where to push the bits! */
        machine_code = push_register(atoi(operand + 1));
        add_code_word(*IC, machine_code, NULL, line_number, state);
    }

    /* CRITICAL: Advance the Instruction Counter after adding the word! */
    (*IC)++;
}