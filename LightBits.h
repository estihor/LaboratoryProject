/*
 * File: LightBits.h
 * Contains the declarations for bit manipulation functions.
 */

unsigned short push_opcode(unsigned int opcode);
unsigned short push_funct(unsigned int funct); 
unsigned short push_register(int register_num);
unsigned short push_source_mode(unsigned int mode);
unsigned short push_destination_mode(unsigned int mode);
