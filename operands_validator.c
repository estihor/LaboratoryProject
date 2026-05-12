#include <stdio.h>
#include <string.h>
#include "operands_validator.h"
#include "validations.h"


#define ADDRESSING_MODE_0 0
#define ADDRESSING_MODE_1 1
#define ADDRESSING_MODE_2 2
#define ADDRESSING_MODE_3 3

#define OK_INTEGER 1
#define INTEGER_ERROR 0
#define OK_ADDRESSING_MODE_2 1
#define ADDRESSING_MODE_2_ERROR 0
#define OK_TYPE_OF_OPERANDS 1
#define TYPE_OF_OPERANDS_ERROR 0
#define VALID_OPERAND 1
#define INVALID_OPERAND 0
#define OK_REGISTER 1
#define REGISTER_ERROR 0
#define OK_LABEL 1
#define LABEL_ERROR 0
#define NUM_OF_OPERATIONS 16
#define NO_OPERAND -1



/**
 * Determines the addressing mode of a given operand string.
 * * @param argument - A pointer to the operand string (e.g., "#5", "r3", "LABEL").
 * @return The addressing mode (0, 1, 2, or 3) or -1 if the argument is invalid.
 */
int what_is_the_addressing_mode(char* argument)
{
	/* Safety check: ensure the pointer is not NULL and string is not empty */
	if (argument == NULL || argument[0] == '\0')
	{
		return -1;
	}

	/* Check if it's Addressing Mode 0 (Immediate - Starts with '#') */
	if (argument[0] == '#')
	{
		return ADDRESSING_MODE_0;
	}

	/* Check if it's Addressing Mode 2 (Relative - Starts with '%') */
	if (argument[0] == '%')
	{
		return ADDRESSING_MODE_2;
	}

	/* Check if it's Addressing Mode 3 (Direct Register) */
	if (is_it_a_register(argument) == OK_REGISTER)
	{
		return ADDRESSING_MODE_3;
	}

	/* Default case: If none of the above, it's Addressing Mode 1 (Direct - Label) */
	return ADDRESSING_MODE_1;
}

/* * Validates if a string represents a valid integer for addressing mode 0.
 * An optional '+' or '-' sign is allowed at the beginning, followed only by digits.
 * @param argument A pointer to the argument string (e.g., "+5", "-12", "42").
 * @return OK_ADDRESSING_MODE_0 (1) if valid, ADDRESSING_MODE_0_ERROR (0) otherwise.
 */
int is_valid_integer(char* argument, int line_number)
{
	/* Must initialize to 0 to always start from the first character */
	int i = 0;

	/*  safety check before accessing the string */
	if (argument == NULL || argument[0] == '\0')
	{
		printf("Error at line %d: Missing number for operand.\n", line_number);
		return INTEGER_ERROR;
	}


	/*  Check for an optional '+' or '-' sign. If found, advance 'i' */
	if (argument[0] == '+' || argument[0] == '-')
		i++;

	/*  Edge case check: return an error if there is ONLY a sign without numbers */
	if (argument[i] == '\0')
	{
		printf("Error at line %d: Missing digits after the sign in number '%s'.\n", line_number, argument);
		return INTEGER_ERROR;
	}

	/*  Iterate over the remaining characters.
	 * Loop initialization is omitted so 'i' continues from its current position! */
	for (; i < strlen(argument); i++)
	{
		/* If the current character is not between '0' and '9', it's invalid */
		if (argument[i] < '0' || argument[i] > '9')
		{
			printf("Error at line %d: Illegal non-digit character found in number '%s'.\n", line_number, argument);
			return INTEGER_ERROR;
		}
	}

	/*  If we passed the entire loop without returning an error - it's a valid number! */
	return OK_INTEGER;
}




/*
 * Validates if a string is a valid relative addressing operand (Mode 2).
 * It expects the string to start with '%' followed by a valid label name.
 * @param macrosArray A pointer to the array of currently defined macros.
 * @param argument The operand string to check (e.g., "%LOOP").
 * @param totalMacros The number of macros defined so far.
 * @return OK_ADDRESSING_MODE_2 (1) if valid, ADDRESSING_MODE_2_ERROR (0) otherwise.
 */
int is_addressing_mode_2_valid(OneMakro* macrosArray, char* argument, int totalMacros, int line_number)
{
	/* 1. Safety check before accessing the string */
	if (argument == NULL || argument[0] == '\0')
		return ADDRESSING_MODE_2_ERROR;

	/* 2. Check if the first character is indeed '%' */
	if (argument[0] != '%')
	{
		printf("Error at line %d: Relative addressing operand must start with '%%'.\n", line_number);
		return ADDRESSING_MODE_2_ERROR;
	}

	/* 3. Check if the rest of the string is a valid label.
	 * We use argument + 1 to skip the '%' and send only the label name. */
	if (is_it_a_valid_label(macrosArray, argument + 1, totalMacros, line_number) == OK_LABEL)
	{
		return OK_ADDRESSING_MODE_2; /* It passed! */
	}

	/* 4. If the label check failed, return an error */
	return ADDRESSING_MODE_2_ERROR;
}

Operations3  ArrOperations_addressing_modes[] =
{
	/* Name, {Source Modes}, {Destination Modes} */
	{"mov",  {1, 1, 0, 1},  {0, 1, 0, 1}},
	{"cmp",  {1, 1, 0, 1},  {1, 1, 0, 1}},
	{"add",  {1, 1, 0, 1},  {0, 1, 0, 1}},
	{"sub",  {1, 1, 0, 1},  {0, 1, 0, 1}},
	{"lea",  {0, 1, 0, 0},  {0, 1, 0, 1}},
	{"clr",  {0, 0, 0, 0},  {0, 1, 0, 1}},
	{"not",  {0, 0, 0, 0},  {0, 1, 0, 1}},
	{"inc",  {0, 0, 0, 0},  {0, 1, 0, 1}},
	{"dec",  {0, 0, 0, 0},  {0, 1, 0, 1}},
	{"jmp",  {0, 0, 0, 0},  {0, 1, 1, 0}},
	{"bne",  {0, 0, 0, 0},  {0, 1, 1, 0}},
	{"jsr",  {0, 0, 0, 0},  {0, 1, 1, 0}},
	{"red",  {0, 0, 0, 0},  {0, 1, 0, 1}},
	{"prn",  {0, 0, 0, 0},  {1, 1, 0, 1}},
	{"rts",  {0, 0, 0, 0},  {0, 0, 0, 0}},
	{"stop", {0, 0, 0, 0},  {0, 0, 0, 0}}
};

/**
 * Validates if the given addressing modes are legal for a specific assembly operation.
 * The function searches for the operation in the lookup table and checks if the
 * provided source and destination modes are allowed according to the language rules.
 * * @param operation_name The name of the assembly operation (e.g., "mov", "lea", "stop").
 * @param source_mode The addressing mode of the source operand (0-3), or NO_OPERAND if it doesn't exist.
 * @param destination_mode The addressing mode of the destination operand (0-3), or NO_OPERAND if it doesn't exist.
 * @return OK_TYPE_OF_OPERANDS (1) if the addressing modes are valid, TYPE_OF_OPERANDS_ERROR (0) otherwise.
 */
int is_valid_addressing(char* operation_name, int source_mode, int destination_mode)
{
	int i;

	/* Iterate through the array of operations to find a match */
	for (i = 0; i < NUM_OF_OPERATIONS; i++)
	{
		/* Check if the current operation in the array matches the requested name */
		if (strcmp(operation_name, ArrOperations_addressing_modes[i].name) == 0)
		{
			/* --- Match found! Now validate the operands --- */
			/* 1. Validate the source operand (if one is required/provided) */
			if (source_mode != NO_OPERAND)
			{
				/* If the array contains 0 for this mode, it is an illegal addressing mode */
				if (ArrOperations_addressing_modes[i].arr_source_operand[source_mode] == 0)
					return TYPE_OF_OPERANDS_ERROR;
			}

			/* 2. Validate the destination operand (if one is required/provided) */
			if (destination_mode != NO_OPERAND)
			{
				/* If the array contains 0 for this mode, it is an illegal addressing mode */
				if (ArrOperations_addressing_modes[i].arr_destination_operand[destination_mode] == 0)
					return TYPE_OF_OPERANDS_ERROR;
			}
			/* Both operands were checked and found valid according to the lookup table */
			return OK_TYPE_OF_OPERANDS;
		}
	}
	/* The loop finished but the operation name was not found in the array */
	return TYPE_OF_OPERANDS_ERROR;
}

#define VALID_OPERAND 1
#define INVALID_OPERAND 0

/**
 * validate_operand_by_mode - Validates the syntax of an operand based on its addressing mode.
 * Routes the operand to the appropriate validation function (integer, label, relative, or register).
 *
 * @param operand The string representing the operand to validate.
 * @param mode The addressing mode of the operand (0, 1, 2, or 3).
 * @param macrosArray Pointer to the array of defined macros (for label validation).
 * @param total_macros The total number of macros currently defined.
 * @return VALID_OPERAND (1) if the operand is perfectly valid, INVALID_OPERAND (0) if there is a syntax error.
 */
int validate_operand_by_mode(char* operand, int mode, OneMakro* macrosArray, int total_macros, int line_number)
{
	if (mode == ADDRESSING_MODE_0)
	{
		/* Skip the '#' character by passing operand + 1 */
		if (is_valid_integer(operand + 1, line_number) == INTEGER_ERROR)
		{
			return INVALID_OPERAND; /* Return 0 if it's not a valid number */
		}
	}
	else if (mode == ADDRESSING_MODE_1)
	{
		/* Check if it's a valid label syntax */
		if (is_it_a_valid_label(macrosArray, operand, total_macros, line_number) == LABEL_ERROR)
		{
			return INVALID_OPERAND;
		}
	}
	else if (mode == ADDRESSING_MODE_2)
	{
		/* Check if it's a valid relative mode syntax (e.g., %LOOP) */
		if (is_addressing_mode_2_valid(macrosArray, operand, total_macros, line_number) == ADDRESSING_MODE_2_ERROR)
		{
			return INVALID_OPERAND;
		}
	}
	else if (mode == ADDRESSING_MODE_3)
	{
		/* Check if it's a valid register (e.g., r1) */
		if (is_it_a_register(operand) == REGISTER_ERROR)
		{
			printf("Error at line %d: Illegal register name '%s'. Use only valid registers (r0-r7).\n", line_number, operand);
			return INVALID_OPERAND;
		}
	}
	else
	{
		/* Safety net: If the mode is none of the above (e.g., -1) */
		printf("Error at line %d: Unrecognized operand format.\n", line_number);
		return INVALID_OPERAND;
	}

	/* If we passed all the checks without returning an error, the operand is valid! */
	return VALID_OPERAND;
}



