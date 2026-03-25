#include "Pre_Assembler.h"
#include "first_pass.h"
#include "memory_manager.h"
int main(int argc, char* argv[])
{
    FILE* asFile;
    FILE* amFile;
    FILE* amFileForFirstPass;
    int i;
    OneMakro* macrosArray;
    int total_macros_found;

    /* Check if file names were provided in the command line */
    if (argc < 2)
    {
        return 1;
    }
    /* Loop through each file provided by the user */
    for (i = 1; i < argc; i++)
    {
        total_macros_found = 0;/* Reset the macro counter for each new file */
        /* Open the source (.as) and target (.am) files */
        asFile = create_asEnding_Tofile(argv[i]);
        amFile = create_amEnding_Tofile(argv[i],"w");
        /* Safety check: ensure both files were opened successfully */
        if (asFile == NULL || amFile == NULL)
        {
            printf("Error: Cannot open files for %s\n", argv[i]);
            continue;
        }
        /* Pass 1: Find all macros and build the macro table */
        macrosArray = Macro_word_search(asFile, &total_macros_found);
        /* Pass 2: Expand macros and write the new .am file */
        Creates_the_file_am(macrosArray, total_macros_found, asFile, amFile);
        fclose(asFile);
        fclose(amFile);

        /* Re-open the newly created .am file, this time for reading ("r") */
        amFileForFirstPass = create_amEnding_Tofile(argv[i], "r");

        if (amFileForFirstPass != NULL)
        {
            /* Stores the completion status of the first pass (0 for success, 1 for errors) */
            int first_pass_status;

            printf("\n--- Starting First Pass analysis for file: %s.am ---\n", argv[i]);
            /* Run the first pass analysis on the expanded file */
            first_pass_status = first_pass(amFileForFirstPass, macrosArray, total_macros_found);

            /* Close the file immediately after scanning is complete */
            fclose(amFileForFirstPass);

            /* Evaluate the outcome of the first pass */
            if (first_pass_status == 1)
            {
                /* Errors were found! Print a warning and skip to the next file */
                printf("Errors detected in First Pass for file: %s.am. Skipping Second Pass.\n", argv[i]);
            }
            else
            {
                /* Flawless execution! It is safe to proceed to the Second Pass */
                printf("First Pass successful for file: %s.am. Proceeding to Second Pass...\n", argv[i]);

                /* Call second_pass(...) here in the future */
            }

        }

     
        /* Free all dynamically allocated memory for the current file */
        Release_the_macrosArray(macrosArray, total_macros_found);

        /* חובה! ניקוי טבלת הסמלים ותמונות הזיכרון לקראת הקובץ הבא */
        free_all_memory();
    }
    return 0;
}