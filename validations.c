#include <string.h>
#include "validations.h"

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

char* ArrInstructions[NUM_OF_INSTRUCTIONS] = {
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
	int i;

	/* Safety check for empty input */
	if (name == NULL || name[0] == '\0')
		return 1;

	/* Check against all reserved categories */
	if (is_it_a_register(name) == OK_REGISTER ||
		is_it_an_operation_and_find_operands(name, &num) == OK_OPERANDS ||
		is_it_an_instruction(name) == OK_INSTRUCTION ||
		is_it_a_macro(macrosArray, name, totalMacros) != MACRO_ERROR)
	{
		return 1; /* It is a reserved word */
	}

	/* Check for directives WITHOUT the dot (e.g., "data" instead of ".data") */
	for (i = 0; i < NUM_OF_INSTRUCTIONS; i++)
	{
		/* ArrInstructions[i] + 1 skips the dot '.' */
		if (strcmp(name, ArrInstructions[i] + 1) == 0)
		{
			return 1; /* It's a reserved word! */
		}
	}
	return 0; /* Not a reserved word, potentially a valid label name */
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
int is_it_a_valid_label(OneMakro* macrosArray, char* name, int total_macros_found)
{
	int i, len;

	/* 0. Basic validation: must not be empty and must not be a reserved word */
	if (name == NULL || name[0] == '\0')
		return LABEL_ERROR;

	/* Ensure the name isn't already taken by a register, command, or macro */
	if (is_reserved_word(macrosArray, name, total_macros_found))
		return LABEL_ERROR;

	/* 1. Check length: A label name cannot exceed 31 characters */
	len = (int)strlen(name);
	if (len > LABEL_LENGTH)
		return LABEL_ERROR;

	/* 2. Syntax rule: The first character must be an English letter (A-Z or a-z) */
	if (!((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z')))
		return LABEL_ERROR;

	/* 3. Check the rest of the name: only letters and digits are allowed */
	for (i = 1; i < len; i++)
	{
		/* If a character is not a letter and not a digit, it's an invalid label */
		if (!((name[i] >= 'a' && name[i] <= 'z') ||
			(name[i] >= 'A' && name[i] <= 'Z') ||
			(name[i] >= '0' && name[i] <= '9')))
		{
			return LABEL_ERROR;
		}
	}

	/* 4. Success: All checks passed, the label name is valid */
	return OK_LABEL;
}


/**
 * Determines the addressing mode of a given operand string.
 * * @param argument - A pointer to the operand string (e.g., "#5", "r3", "LABEL").
 * @return The addressing mode (0, 1, 2, or 3) or -1 if the argument is invalid.
 */
int what_is_the_addressing_mode( char* argument)
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
int is_valid_integer(char* argument)
{
	/* Must initialize to 0 to always start from the first character */
	int i = 0;

	/*  safety check before accessing the string */
	if (argument == NULL || argument[0] == '\0')
		return ADDRESSING_MODE_0_ERROR;

	/*  Check for an optional '+' or '-' sign. If found, advance 'i' */
	if (argument[0] == '+' || argument[0] == '-')
		i++;

	/*  Edge case check: return an error if there is ONLY a sign without numbers */
	if (argument[i] == '\0')
		return ADDRESSING_MODE_0_ERROR;

	/*  Iterate over the remaining characters.
	 * Loop initialization is omitted so 'i' continues from its current position! */
	for (; i < strlen(argument); i++)
	{
		/* If the current character is not between '0' and '9', it's invalid */
		if (argument[i] < '0' || argument[i] > '9')
			return ADDRESSING_MODE_0_ERROR;
	}

	/*  If we passed the entire loop without returning an error - it's a valid number! */
	return OK_ADDRESSING_MODE_0;
}




/*
 * Validates if a string is a valid relative addressing operand (Mode 2).
 * It expects the string to start with '%' followed by a valid label name.
 * @param macrosArray A pointer to the array of currently defined macros.
 * @param argument The operand string to check (e.g., "%LOOP").
 * @param totalMacros The number of macros defined so far.
 * @return OK_ADDRESSING_MODE_2 (1) if valid, ADDRESSING_MODE_2_ERROR (0) otherwise.
 */
int is_addressing_mode_2_valid(OneMakro* macrosArray, char* argument, int totalMacros)
{
	/* 1. Safety check before accessing the string */
	if (argument == NULL || argument[0] == '\0')
		return ADDRESSING_MODE_2_ERROR;

	/* 2. Check if the first character is indeed '%' */
	if (argument[0] != '%')
		return ADDRESSING_MODE_2_ERROR;

	/* 3. Check if the rest of the string is a valid label.
	 * We use argument + 1 to skip the '%' and send only the label name. */
	if (is_it_a_valid_label(macrosArray, argument + 1, totalMacros) == OK_LABEL)
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

