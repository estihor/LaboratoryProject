/*
 * File: scan_lines.h
 * Contains the declarations for text scanning, parsing, and syntax checking functions.
 */
int skip_the_spaces(char* line, int index);
int cut_the_next_word(char* line, int index, char* the_cut_word);
int check_and_skip_comma(char* line, int index);
int is_str_valid(char* line, int index);
