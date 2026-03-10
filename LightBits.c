
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
unsigned short push_funct(unsigned int funct)
{
    return  funct << 4;
}
