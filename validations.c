#include <string.h>
#include "validations.h"

/* Constants to avoid magic numbers */
#define OK_OPERATION 1
#define OK_OPERANDS 1
#define OK_REGISTER 1
#define OPERATION_ERROR 0
#define OPERANDS_ERROR 0
#define REGISTER_ERROR 0

#define NUM_OF_OPERATIONS 16
#define NUM_OF_REGISTERS 8


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


int find_opcode_and_funct(char* name, unsigned int* opcode, unsigned int* funct)
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

int find_num_of_operands(char* name, unsigned int* num_of_operands)
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
char* RegistersTable[NUM_OF_REGISTERS] = {
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
        if (strcmp(name, RegistersTable[i]) == 0)
        {
            return OK_REGISTER; /* Match found! It's a register */
        }
    }

    return REGISTER_ERROR; /* Loop finished without a match - not a register */

}