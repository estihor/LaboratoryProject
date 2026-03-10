#include <stdio.h>
#include <stdlib.h>
#include "validations.h"
/* Add the include for the pre-assembler header here when ready */
/* #include "pre_assembler.h" */

/*
 * The main entry point of the assembler program.
 * Receives the names of the assembly files (without extensions) from the command line.
 */
int main(int argc, char* argv[])
{
    int i;

    /* Check if the user provided at least one input file */
    if (argc < 2)
    {
        printf("Error: No input files provided.\n");
        printf("Usage: ./assembler file1 file2 ...\n");
        return 1; /* Return 1 to indicate an error to the operating system */
    }

    /* Iterate through each file provided by the user in the command line */
    for (i = 1; i < argc; i++)
    {
        printf("\n--- Starting processing for file: %s ---\n", argv[i]);

        /* =========================================
         * STEP 1: Pre-Assembler (Macro Deployment)
         * ========================================= */
         /* Call the pre-assembler function here to open macros.
          * If the pre-assembler fails (e.g., illegal macro name),
          * we should continue to the next file. */

          /* =========================================
           * STEP 2: First Pass (Syntax & Symbol Table)
           * ========================================= */
           /* Open the .am file, read it line by line,
            * validate syntax using validations.h,
            * calculate memory addresses, and build the Symbol Table. */

            /* =========================================
             * STEP 3: Second Pass (Binary Encoding)
             * ========================================= */
             /* If the First Pass completed with no errors,
              * read the file again to resolve labels and generate the final machine code. */

        printf("--- Finished processing file: %s ---\n", argv[i]);
    }

    return 0; /* Program finished successfully */
}