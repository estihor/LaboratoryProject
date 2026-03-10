#include <string.h>
#include "validations.h"

/* Constants to avoid magic numbers */
#define NUM_OF_OPERATIONS 16
#define OK 1
#define OPERATION_ERROR 0
#define OPERANDS_ERROR 0

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
    {"lea", 4,  0},
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
    for (i = 0; i < NUM_OF_OPERATIONS; i++)
    {
        if (strcmp(name, ArrOperations_opcode_and_funct[i].name) == 0)
        {
            *opcode = ArrOperations_opcode_and_funct[i].opcode;
            *funct = ArrOperations_opcode_and_funct[i].funct;
            return OK;
        }
    }
    return OPERATION_ERROR;
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
    for (i = 0; i < NUM_OF_OPERATIONS; i++)
    {
        if (strcmp(name, ArrOperations_and_num_of_operands[i].name) == 0)
        {
            *num_of_operands = ArrOperations_and_num_of_operands[i].num_of_operands;

            return OK;
        }
    }
    return OPERANDS_ERROR;
}