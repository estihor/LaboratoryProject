#include "Pre_Assembler.h"
int main(int argc, char* argv[])
{
    FILE* asFile;
    FILE* amFile;
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
        amFile = create_amEnding_Tofile(argv[i]);
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

        /* Free all dynamically allocated memory for the current file */
        Release_the_macrosArray(macrosArray, total_macros_found);
    }
    return 0;
}