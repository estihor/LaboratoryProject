/*
 * File: first_pass.c
 * Purpose: Implements the first pass of the assembly process.
 * This module is responsible for scanning the expanded (.am) file,
 * populating the symbol table, and calculating memory offsets (IC & DC).
 */

#include <stdio.h>
#include "first_pass.h"
#include "validations.h"
#include "scan_lines.h"

 /**
  * first_pass - Scans the .am file to determine instruction and data sizes.
  * @amFile: A pointer to the opened expanded assembly file.
  * @macrosArray: The table of macros (used to ensure labels are not macro names).
  * @total_macros_found: Total count of macros in the system.
  */
void first_pass(FILE* amFile, OneMakro* macrosArray, int total_macros_found)
{
    char line[82];    /* Buffer for reading lines (max 80 chars + \n + \0) */
    int IC = 100;     /* Instruction Counter - starts at address 100 */
    int DC = 0;       /* Data Counter - tracks data storage (.data, .string) */

    /* Read the file line by line until EOF */
    while (fgets(line, sizeof(line), amFile) != NULL)
    {
        /* * Logic will be implemented here:
         * 1. Skip leading whitespaces.
         * 2. Identify and validate labels.
         * 3. Distinguish between directives (.data) and instructions (mov).
         * 4. Update IC and DC accordingly.
         */
    }
}