

/*
 * This struct defines the structure of a single machine operation.
 * name: The name of the assembly command (e.g., "mov", "add").
 * opcode: The primary operation code.
 * funct:  The secondary function code (0 if not applicable).
 */
typedef struct {
    char* name;
    unsigned int opcode;
    unsigned int funct;
} Operations1;

/*
 * This struct defines the structure for checking operand limits.
 * name: The name of the assembly command.
 * num_of_operands: The exact number of operands this command requires.
 */
typedef struct {
    char* name;
    unsigned int num_of_operands;

} Operations2;


 /* * File: operations.h
  * Contains declarations for searching assembly operations.
  */
int find_opcode_and_funct(char* name, unsigned int* opcode, unsigned int* funct);
int find_num_of_operands(char* name, unsigned int* num_of_operands);
int is_it_a_register(char* name);
int is_it_an_instruction(char* name);
int what_is_the_addressing_mode(char* argument)