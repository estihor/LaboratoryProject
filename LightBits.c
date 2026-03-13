#include"LightBits.h"
/*
 * Function: push_opcode
 * Receives the operation code (opcode) and shifts it left by 8 bits
 * to its correct position in the 16-bit machine word.
 * * opcode: The numeric value of the operation (e.g., 2 for 'add').
 * Returns: A 16-bit word with only the opcode bits turned on.
 */
unsigned short push_opcode(unsigned int opcode)
{
	return opcode << 8;
}
/*
 * Function: push_funct
 * Receives the function code (funct) and shifts it left by 4 bits
 * to its correct position in the 16-bit machine word.
 * * funct: The numeric value of the function (e.g., 10 for 'add').
 * Returns: A 16-bit word with only the funct bits turned on.
 */

/* this is test 5 */

unsigned short push_funct(unsigned int funct)
{
    return  funct << 4;
}


/*
 * Function: push_register
 * Receives the register number (0-7) and turns on its corresponding bit
 * by shifting '1' to the left (e.g., r3 becomes 1 << 3).
 */
unsigned short push_register(int register_num)
{
    return 1 << register_num;
}

/*
 * Function: push_source_mode
 * Receives the source addressing mode and shifts it to bits 2-3.
 */
unsigned short push_source_mode(unsigned int mode)
{
    return mode << 2;
}

/*
 * Function: push_dest_mode
 * Receives the destination addressing mode and places it in bits 0-1.
 */
unsigned short push_destination_mode(unsigned int mode)
{
    /* No shift needed because it sits at the very beginning (bits 0-1) */
    return mode;
}

