#include "Pre_Assembler.h"
#include "first_pass.h"
#include "second_pass.h"
#include "memory_manager.h"
#include "CreatingFiles.h"
#define  NO_ERORR_IN_MACRO_DEFINITION 0
#define ERORR_IN_MACRO_DEFINITION 1
int main(int argc, char* argv[])
{
    FILE* asFile;
    FILE* amFile;
    FILE* amFileForFirstPass;
    int i;
    OneMakro* macrosArray;
    int total_macros_found;
    int errorFlag;
    int second_pass_error;
    /* Check if file names were provided in the command line */
    if (argc < 2)
    {
        return 1;
    }
    /* Loop through each file provided by the user */
    for (i = 1; i < argc; i++)
    {
        AssemblerData assembler_data = { NULL, 0, NULL, 0, NULL, 0 };
        total_macros_found = 0;/* Reset the macro counter for each new file */
        /* Open the source (.as) and target (.am) files */
        asFile = create_file_with_extension(argv[i], 's', "r");
        amFile = create_file_with_extension(argv[i], 'm', "w");
        /* Safety check: ensure both files were opened successfully */
        if (asFile == NULL || amFile == NULL)
        {
            printf("Error: Cannot open files for %s\n", argv[i]);
            continue;
        }
        /* Pass 1: Find all macros and build the macro table */
        errorFlag = NO_ERORR_IN_MACRO_DEFINITION;
        macrosArray = Macro_word_search(asFile, &total_macros_found, argv[i], &errorFlag);
        if (errorFlag == ERORR_IN_MACRO_DEFINITION)
        {
            fclose(asFile);
            Release_the_macrosArray(macrosArray, total_macros_found);
            continue;
        }
        /* Pass 2: Expand macros and write the new .am file */
        Creates_the_file_am(macrosArray, total_macros_found, asFile, amFile);
        fclose(asFile);
        fclose(amFile);
        amFileForFirstPass = create_file_with_extension(argv[i], 'm', "r");

        if (amFileForFirstPass != NULL)
        {
            /* Stores the completion status of the first pass (0 for success, 1 for errors) */
            int first_pass_status;

            printf("\n--- Starting First Pass analysis for file: %s.am ---\n", argv[i]);
            /* Run the first pass analysis on the expanded file */

            first_pass_status = first_pass(amFileForFirstPass, macrosArray, total_macros_found, &assembler_data);

            /* Evaluate the outcome of the first pass */
            if (first_pass_status == 1)
            {
                /* Errors were found! Print a warning and skip to the next file */
                printf("Errors detected in First Pass for file: %s.am. Skipping Second Pass.\n", argv[i]);
                /* להגיד לאסתי שהוספתי וששיניתי ל continue  else במקום ה  */
                fclose(amFileForFirstPass);
                Release_the_macrosArray(macrosArray, total_macros_found);
                free_all_memory(&assembler_data);
                continue;
            }
            /* Flawless execution! It is safe to proceed to the Second Pass */
            printf("First Pass successful for file: %s.am. Proceeding to Second Pass...\n", argv[i]);
            second_pass_error = 0;
            rewind(amFileForFirstPass); /* החזרת סמן הקריאה להתחלה */
            looking_for_entry(assembler_data.symbol_table, amFileForFirstPass, assembler_data.symbol_count, argv[i], &second_pass_error);
            Complete_Table(assembler_data.symbol_table, assembler_data.code_image, assembler_data.symbol_count, assembler_data.code_count, argv[i], &second_pass_error);
            fclose(amFileForFirstPass);
            if (second_pass_error == 0)
            {
                Creating_ob_File(assembler_data.code_image, assembler_data.code_count, assembler_data.data_image, assembler_data.data_count, argv[i]);
                Creating_ent_File(assembler_data.symbol_table, assembler_data.symbol_count, argv[i]);
                Creating_ext_File(assembler_data.code_image, assembler_data.code_count, argv[i], assembler_data.symbol_table, assembler_data.symbol_count);
                printf("Second Pass and output files generated successfully for %s!\n", argv[i]);
            }
            else
            {
                printf("Errors detected in Second Pass. Output files will NOT be created.\n");
            }

        }
        /* Free all dynamically allocated memory for the current file */
        Release_the_macrosArray(macrosArray, total_macros_found);
        free_all_memory(&assembler_data);
    }
    return 0;
}