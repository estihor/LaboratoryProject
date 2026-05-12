#include <string.h>
#include <stdio.h>
#include "validations.h"


/* Constants to avoid magic numbers */
#define OK_OPERATION 1
#define OK_OPERANDS 1
#define OK_REGISTER 1
#define OK_INSTRUCTION 1
#define OK_LABEL 1
#define OK_INTEGER 1
#define OK_ADDRESSING_MODE_2 1
#define OK_TYPE_OF_OPERANDS 1


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


#define NUM_OF_OPERATIONS 16
#define NUM_OF_REGISTERS 8
#define NUM_OF_INSTRUCTIONS 4


#define ADDRESSING_MODE_0 0
#define ADDRESSING_MODE_1 1
#define ADDRESSING_MODE_2 2
#define ADDRESSING_MODE_3 3
#define LABEL_LENGTH 31


/*
 * An array containing all 16 legal operations in our imaginary machine.
 * The assembler uses this lookup table to search for a command's name
 * and retrieve its matching opcode and funct values for binary translation.
 */


Operations1  ArrOperations_opcode_and_funct[] = {
	{"mov", 0, 0},
	{"cmp", 1, 0},
	{"add", 2,10},
	{"sub", 2, 11},
	{"lea", 4, 0},
	{"clr", 5, 10},
	{"not", 5, 11},
	{"inc", 5, 12},
	{"dec", 5, 13},
	{"jmp", 9, 10},
	{"bne", 9, 11},
	{"jsr", 9, 12},
	{"red", 12, 0},
	{"prn", 13, 0},
	{"rts", 14, 0},
	{"stop",15, 0}

};
/*
 * A func that checks a command name to see if it is a valid instruction.
 * If it is valid, it updates its opcode and funct using pointers, and returns 1 (OK).
 * If it is not valid, it returns 0 (OPERATION_ERROR).
 */


int is_it_an_operation_and_find_opcode_and_funct(char* name, unsigned int* opcode, unsigned int* funct)
{
	int i;
	/* Iterate through the operations array to find a matching command name */
	for (i = 0; i < NUM_OF_OPERATIONS; i++)
	{
		if (strcmp(name, ArrOperations_opcode_and_funct[i].name) == 0)
		{
			/* Match found! Update the values using the provided pointers */
			*opcode = ArrOperations_opcode_and_funct[i].opcode;
			*funct = ArrOperations_opcode_and_funct[i].funct;
			return OK_OPERATION; /* Return success status */
		}
	}
	return OPERATION_ERROR; /* Loop finished without a match - invalid operation */
}



/*
 * An array containing the expected number of operands for each command.
 * Used for syntax validation in the First Pass.
 */

Operations2  ArrOperations_and_num_of_operands[] = {
	{"mov",2 },
	{"cmp",2 },
	{"add",2 },
	{"sub",2 },
	{"lea",2 },
	{"clr",1 },
	{"not",1 },
	{"inc",1 },
	{"dec",1 },
	{"jmp",1 },
	{"bne",1 },
	{"jsr",1 },
	{"red",1 },
	{"prn",1 },
	{"rts",0 },
	{"stop",0}

};

/*
 * Searches for a command name and retrieves its required number of operands.
 * If it is valid, it updates num_of_operands via pointer, and returns 1 (OK).
 * If it is not valid, it returns 0 (OPERANDS_ERROR).
 */

int is_it_an_operation_and_find_operands(char* name, unsigned int* num_of_operands)
{
	int i;
	/* Iterate through the array to find the command and its operands count */
	for (i = 0; i < NUM_OF_OPERATIONS; i++)
	{
		if (strcmp(name, ArrOperations_and_num_of_operands[i].name) == 0)
		{
			/* Match found! Update the pointer with the correct number of operands */
			*num_of_operands = ArrOperations_and_num_of_operands[i].num_of_operands;

			return  OK_OPERANDS; /* Return success status */
		}
	}
	return OPERANDS_ERROR; /* Loop finished without a match - invalid operation */
}

/*
* An array containing all valid register names.
*/
char* ArrRegister[NUM_OF_REGISTERS] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
};

/*
 * Checks if a given string is a valid register name.
 * Receives: A pointer to the string (the word being checked).
 * Returns: 1 if it is a register, 0 otherwise.
 */
int is_it_a_register(char* name)
{
	int i;

	/* Iterate through the array to find a match with a register name */
	for (i = 0; i < NUM_OF_REGISTERS; i++)
	{
		if (strcmp(name, ArrRegister[i]) == 0)
		{
			return OK_REGISTER; /* Match found! It's a register */
		}
	}

	return REGISTER_ERROR; /* Loop finished without a match - not a register */

}

char* ArrInstructions[NUM_OF_INSTRUCTIONS] =
{
	".string",".data",".extern" , ".entry",
};

/**
 * Checks if a given string is a valid assembly instruction (directive).
 * The function compares the given string against the array of legal
 * instructions (e.g., ".data", ".string", ".entry", ".extern").
 * * @param name A pointer to the string representing the word to be checked.
 * @return OK_INSTRUCTION (1) if it is a valid instruction, INSTRUCTION_ERROR (0) otherwise.
 */
int is_it_an_instruction(char* name)
{
	int i;

	/* Iterate through the array to find a match with an instruction name */
	for (i = 0; i < NUM_OF_INSTRUCTIONS; i++)
	{
		if (strcmp(name, ArrInstructions[i]) == 0)
		{
			return OK_INSTRUCTION; /* Match found! It's a instruction */
		}
	}

	return INSTRUCTION_ERROR; /* Loop finished without a match - not an instruction */

}

/**
 * Checks if a given string matches an instruction directive WITHOUT the dot
 * (e.g., "data" instead of ".data", "string" instead of ".string").
 *
 * @param name The string to check.
 * @return 1 if it matches a directive name without a dot, 0 otherwise.
 */
int is_instruction_without_dot(char* name)
{
	int i;

	if (name == NULL || name[0] == '\0')
		return 0;

	/* Check for directives WITHOUT the dot (e.g., "data" instead of ".data") */
	for (i = 0; i < NUM_OF_INSTRUCTIONS; i++)
	{
		/* ArrInstructions[i] + 1 skips the dot '.' */
		if (strcmp(name, ArrInstructions[i] + 1) == 0)
		{
			return 1; /* Match found! */
		}
	}
	return 0; /* No match */
}
/**
 * Checks if a given string is a reserved word in the assembly language.
 * A reserved word can be an operation name (e.g., mov, add), a register (e.g., r1, r2),
 * an instruction directive (e.g., .data, .string), or a previously defined macro.
 *
 * @param macrosArray A pointer to the array of currently defined macros.
 * @param name The string to be checked.
 * @param totalMacros The number of macros defined so far.
 * @return 1 if the string is a reserved word (cannot be used as a label), 0 otherwise.
 */
int is_reserved_word(OneMakro* macrosArray, char* name, int totalMacros)
{
	unsigned int num;

	/* Safety check for empty input */
	if (name == NULL || name[0] == '\0')
		return 1;

	/* Check against all reserved categories */
	if (is_it_a_register(name) == OK_REGISTER ||
		is_it_an_operation_and_find_operands(name, &num) == OK_OPERANDS ||
		is_it_an_instruction(name) == OK_INSTRUCTION ||
		is_it_a_macro(macrosArray, name, totalMacros) != MACRO_ERROR ||
		is_instruction_without_dot(name) == 1) /* <--- הקריאה לפונקציה החדשה! */
	{
		return 1; /* It is a reserved word */
	}

	return 0; /* Not a reserved word, potentially a valid label name */
}
/**
 * Helper Function: is_valid_name_syntax
 * Validates the character syntax of a given name (for labels or macros).
 * * @param name The string to check.
 * @param allow_underscore 1 if '_' is allowed (macros), 0 if not (labels).
 * @param line_number Current line for error messages.
 * @param type_name String representing the type ("label" or "macro") for printing.
 * @return 1 if the syntax is valid, 0 if it contains illegal characters.
 */
int is_valid_name_syntax(char* name, int allow_underscore, int line_number, const char* type_name)
{
	int i, len;
	len = (int)strlen(name);

	/* Rule 1: The first character must be an English letter (A-Z or a-z) */
	if (!((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z')))
	{
		printf("Error at line %d: Illegal %s name '%s'. Must start with an English letter.\n", line_number, type_name, name);
		return 0;
	}

	/* Rule 2: Check the rest of the name based on the allow_underscore flag */
	for (i = 1; i < len; i++)
	{
		/* Check all valid conditions using boolean logic */
		int is_letter = ((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'));
		int is_digit = (name[i] >= '0' && name[i] <= '9');
		int is_underscore = (allow_underscore == 1 && name[i] == '_'); /* Allowed only if flag is 1 */

		/* If the character is none of the allowed types, throw an error */
		if (!(is_letter || is_digit || is_underscore))
		{
			if (allow_underscore == 1) {
				printf("Error at line %d: Illegal character found in %s '%s'. Only alphanumeric and '_' allowed.\n", line_number, type_name, name);
			}
			else {
				printf("Error at line %d: Illegal character found in %s '%s'. Only alphanumeric allowed.\n", line_number, type_name, name);
			}
			return 0;
		}
	}

	return 1; /* All checks passed! */
}

/**
 * Validates if a string is a legal label name according to assembly rules.
 * A legal label must:
 * 1. Not be a reserved word (operation, register, instruction, or macro).
 * 2. Start with an English letter.
 * 3. Contain only alphanumeric characters.
 * 4. Not exceed the maximum length (31 characters).
 *
 * @param macrosArray Pointer to the defined macros.
 * @param name The label name to check (should not include a trailing ':').
 * @param totalMacros Number of macros currently in the system.
 * @return OK_LABEL (1) if valid, LABEL_ERROR (0) otherwise.
 */
int is_it_a_valid_label(OneMakro* macrosArray, char* name, int totalMacros, int line_number)
{
	int len;

	/* 0. Basic validation: must not be empty and must not be a reserved word */
	if (name == NULL || name[0] == '\0')
	{
		printf("Error at line %d: Label name is empty.\n", line_number);
		return LABEL_ERROR;
	}

	/* Ensure the name isn't already taken by a register, command, or macro */
	if (is_reserved_word(macrosArray, name, totalMacros))
	{
		printf("Error at line %d: Illegal label name '%s'. It is a reserved system word (operation, register, or macro).\n", line_number, name);
		return LABEL_ERROR;
	}

	/* 1. Check length: A label name cannot exceed 31 characters */
	len = (int)strlen(name);
	if (len > LABEL_LENGTH)
	{
		printf("Error at line %d: Label name '%s' exceeds the maximum allowed length of %d characters.\n", line_number, name, LABEL_LENGTH);
		return LABEL_ERROR;
	}

	/* 2 & 3. Syntax rules (Letters, digits, no underscores) */
	/* Sending '0' because underscores are NOT allowed in labels */
	if (is_valid_name_syntax(name, 0, line_number, "label") == 0)
	{
		return LABEL_ERROR;
	}

	/* 4. Success: All checks passed, the label name is valid */
	return OK_LABEL;
}





