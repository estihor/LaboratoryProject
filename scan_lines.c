#include<string.h>

#define OK_STRING 1
#define OK_COMMA 1

#define STRING_ERROR 0
#define COMMA_ERROR 0

/**
 * Skips white spaces (spaces and tabs) in a given string starting from a specific index.
 * * @param line The original line of text.
 * @param index The current position in the line.
 * @return The updated index pointing to the first non-white-space character.
 */
int skip_the_white_spaces(char* line, int index)
{
	/* * Loop as long as the current character is a space or a tab.
	 * We do NOT skip '\n' because it marks the end of the instruction line.
	 */
	while (line[index] == ' ' || line[index] == '\t')
	{
		/* Move the cursor to the next character in the line array */
		index++;
	}

	/* Return the new index so the calling function knows where the actual text begins */
	return index;
}


/**
 * Extracts the next word from the line into a given buffer.
 * Stops when it encounters a delimiter (space, tab, comma, newline, or end of string).
 * * @param line The original line of text.
 * @param index The current position in the line.
 * @param the_cut_word An empty array provided by the caller to store the extracted word.
 * @return The updated index in the original line after extracting the word.
 */
int cut_the_next_word(char* line, int index, char* the_cut_word)
{
    /* 'i' will track our current position inside the new, smaller word buffer */
    int i = 0;

    /* * Keep running as long as it is NOT true that we hit a stop sign.
     * Stop signs are: space, tab, comma, newline, or the end of the string.
     */
    while (!(line[index] == ' ' || line[index] == '\t' || line[index] == ',' ||
        line[index] == '\n' || line[index] == '\0'))
        
    {
        /* Copy the current character from the main line to our smaller word buffer */
        the_cut_word[i] = line[index];

        /* Move forward in our smaller word buffer to be ready for the next character */
        i++;

        /* Move forward in the main line to read the next character */
        index++;
    }

    /* * CRITICAL: Close the newly extracted word with a null-terminator ('\0').
     * This tells C that the string ends here, preventing garbage memory reads.
     */
    the_cut_word[i] = '\0';

    /* Return the updated index so the caller knows exactly where we stopped in the main line */
    return index;
}

/**
 * Skips white spaces and checks if the next character is a comma.
 *
 * @param line The original line of text.
 * @param index The current position in the line.
 * @return The updated index after the comma, or -1 if a comma is missing.
 */
int check_and_skip_comma(char* line, int index)
{
    /* Skip any white spaces before the expected comma */
    index = skip_the_white_spaces(line, index);

    /* Check if the current character is a comma */
    if (line[index] == ',')
    {
        index++; /* Skip the comma */
        return index;
    }

    /* Syntax error: missing comma */
    return -1;
}







int is_str_valid( char* the_string)
{
	int i;
	 int len_str = strlen(the_string);
	if (the_string[0] != '"' || the_string[len_str - 1] != '\"')
		return STRING_ERROR;
	for (i = 1; i < len_str - 1; i++)
	{
		if (! (the_string[i] >= ' ' && the_string[i] <= '~'))
			return STRING_ERROR;
	}
	OK_STRING;
}
