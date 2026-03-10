#include "validations.h"

/*
 * An array containing all 16 legal operations in our imaginary machine.
 * The assembler uses this lookup table to search for a command's name
 * and retrieve its matching opcode and funct values for binary translation.
 */

/* This is test two */

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