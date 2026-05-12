#include "Pre_Assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "memory_manager.h"
#include "CreatingFiles.h"

/* Defines for macro expansion status */
#define  NO_ERORR_IN_MACRO_DEFINITION 0
#define ERORR_IN_MACRO_DEFINITION 1

int main(int argc, char* argv[])
{
    /* File pointers for the different stages of the assembler */
    FILE* asFile;             /* Pointer for the original source file (.as) */
    FILE* amFile;             /* Pointer for writing the post-macro file (.am) */
    FILE* amFileForFirstPass; /* Pointer for reading the .am file during the passes */

    int i;
    OneMakro* macrosArray;    /* Array to hold all macros found in the source code */
    int total_macros_found;   /* Counter for the number of macros */
    int errorFlag;            /* Flag to track errors during the pre-assembler stage */
    int second_pass_error;    /* Flag to track errors during the second pass */

    /* 1. INITIAL CHECK: Ensure the user provided at least one file name */
    if (argc < 2)
    {
        /* If no arguments were passed, exit the program completely */
        return 1;
    }

    /* 2. MAIN LOOP: Process each file provided in the command line arguments */
    for (i = 1; i < argc; i++)
    {
        /* Print a separator to indicate the start of a new file analysis */
        printf("\n---Starting analysis for file %s.am ---\n", argv[i]);

        /* Initialize the central data structure that holds the symbol table and data images */
        AssemblerData assembler_data = { NULL, 0, NULL, 0, NULL, 0 };

        /* Reset the macro counter for the new file */
        total_macros_found = 0;

        /* 3. PRE-ASSEMBLER SETUP: Open the required files */
        /* Open the original user file for reading */
        asFile = create_file_with_extension(argv[i], 's', "r");
        /* Create the target macro-expanded file for writing */
        amFile = create_file_with_extension(argv[i], 'm', "w");

        /* Safety check: If files are missing or lack permissions, skip to the next file */
        if (asFile == NULL || amFile == NULL)
        {
            printf("Error: Cannot open the file \n");
            continue;
        }

        /* 4. MACRO SEARCH: Scan the source file to map all macros */
        errorFlag = NO_ERORR_IN_MACRO_DEFINITION;
        macrosArray = Macro_word_search(asFile, &total_macros_found, argv[i], &errorFlag);

        /* If illegal macros were found, clean up memory and abort this file */
        if (errorFlag == ERORR_IN_MACRO_DEFINITION)
        {
            fclose(asFile);
            Release_the_macrosArray(macrosArray, total_macros_found);
            continue; /* Skip to the next file in the for-loop */
        }

        /* 5. MACRO EXPANSION: Write the clean code to the .am file */
        Creates_the_file_am(macrosArray, total_macros_found, asFile, amFile);

        /* Close the pre-assembler files since their job is done */
        fclose(asFile);
        fclose(amFile);

        /* 6. ASSEMBLER PASSES SETUP: Open the newly created .am file for reading */
        amFileForFirstPass = create_file_with_extension(argv[i], 'm', "r");

        if (amFileForFirstPass != NULL)
        {
            /* Variable to store the success/failure status of the first pass */
            int first_pass_status;

            /* FIRST PASS                 */
            /* Scan the file, validate syntax, count IC/DC, and build the symbol table */
            first_pass_status = first_pass(amFileForFirstPass, macrosArray, total_macros_found, &assembler_data);
            /* SECOND PASS                */
            second_pass_error = 0;

            /* Rewind the file pointer back to the beginning of the .am file for the second scan */
            rewind(amFileForFirstPass);

            /* Check for .entry directives and update the symbol table accordingly */
            looking_for_entry(assembler_data.symbol_table, amFileForFirstPass, assembler_data.symbol_count, argv[i], &second_pass_error);

            /* Encode remaining operands (labels) and complete the machine code image */
            Complete_Table(assembler_data.symbol_table, assembler_data.code_image, assembler_data.symbol_count, assembler_data.code_count, argv[i], &second_pass_error);

            /* Close the .am file as we are completely done reading the source code */
            fclose(amFileForFirstPass);

          
            /* FILE GENERATION              */
            /* Check if both passes completed flawlessly without any syntax or logical errors */
            if (first_pass_status == 0 && second_pass_error == 0)
            {
                /* Success! Generate the final output files (.ob, .ent, .ext) */
                Creating_ob_File(assembler_data.code_image, assembler_data.code_count, assembler_data.data_image, assembler_data.data_count, argv[i]);
                Creating_ent_File(assembler_data.symbol_table, assembler_data.symbol_count, argv[i]);
                Creating_ext_File(assembler_data.code_image, assembler_data.code_count, argv[i], assembler_data.symbol_table, assembler_data.symbol_count);
            }
        }

        /* 7. FINAL CLEANUP: Free all dynamically allocated memory to prevent memory leaks */
        /* This happens ALWAYS, regardless of whether the file had errors or not */
        Release_the_macrosArray(macrosArray, total_macros_found);
        free_all_memory(&assembler_data);
    }

    /* Program execution completed successfully */
    return 0;
}