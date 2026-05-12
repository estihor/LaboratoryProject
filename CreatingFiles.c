#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "memory_manager.h"
#define ARE_MASK 12288              /* Mask to isolate the ARE bits (11 0000 0000 0000 in binary) */
#define SHIFT_TO_ARE_LOCATION 12    /* Number of bits to shift to align ARE value */
#define ARE_A_VALUE 0               /* Value representing the 'A' (Absolute) status */
#define ARE_E_VALUE 1               /* Value representing the 'E' (Extern) status */
#define TWELVE_BITS_MASK 0xFFF      /* Mask to isolate the lower 12 bits for printing */
#define MEMORY_START 100     /* The initial memory address start for the IC/DC */
#define YES 1                       /* Boolean equivalent for TRUE */
#define NO 0                        /* Boolean equivalent for FALSE */
/*
 * Shifts the ARE bits to the rightmost position for evaluation.
 * @param REA_number The machine code containing the ARE bits in positions 12-13.
 * @return The shifted value representing the extracted ARE number.
 */
unsigned short Push_Bit_For_Letter_extraction(unsigned short REA_number)
{
    return (REA_number >> SHIFT_TO_ARE_LOCATION);
}
/*
 * Extracts and determines the appropriate ARE letter ('A', 'R', or 'E') from a machine word.
 * @param BinaryNumber The full 14-bit machine code word.
 * @return A character ('A', 'R', or 'E') corresponding to the ARE bits of the machine word.
 */
char  Letter_extraction(unsigned short BinaryNumber)
{
    unsigned short Letter;
    /* Apply mask to isolate bits 12-13, then shift them right to get the exact value (0, 1, or 2) */
    Letter = (unsigned short)Push_Bit_For_Letter_extraction(BinaryNumber & ARE_MASK);
    if (Letter == ARE_A_VALUE)
    {
        return 'A';
    }
    else if (Letter == ARE_E_VALUE)
    {
        return 'E';
    }
    return 'R';
}
/*
 * Creates the .ent (entries) output file containing all exported entry labels and their addresses.
 * The file is only created if at least one entry label is found.
 * @param labelsArr The symbol table array containing all defined labels.
 * @param labelsArrLength The total number of labels in the symbol table.
 */
void Creating_ent_File(label* labelsArr, int labelsArrLength, char* FileName)
{
    int i;
    int flag = NO;
    char FILENAME[FILENAME_MAX];
    FILE* file = NULL;
    /* Iterate through the label table to find labels marked as entry */
    for (i = 0; i < labelsArrLength; i++)
    {
        /* Open the file only upon finding the first entry label to avoid empty files */
        if (labelsArr[i].is_entry == YES) {
            if (flag == NO)
            {
                sprintf(FILENAME, "%s.ent", FileName);
                file = fopen(FILENAME, "w");
                if (file == NULL) {
                    printf("Error: Could not create .ent file\n");
                    return;
                }
                flag = YES;
            }
            /* Print the label name and its zero-padded 4-digit address */
            fprintf(file, "%s %04d\n", labelsArr[i].labelName, labelsArr[i].labelAddress);
        }
    }
    /* Close the file safely if it was opened */
    if (file != NULL)
    {
        fclose(file);
    }
}
/*
 * Creates the .ext (externals) output file containing all external label calls and the addresses where they occurred.
 * The file is only created if at least one external label reference is found in the code image.
 * @param CodeImageArr The array representing the translated instruction words.
 * @param CodeImageArrLength The total number of code words in the Code Image.
 * @param labelsArr The symbol table array containing all defined labels.
 * @param labelsArrLength The total number of labels in the symbol table.
 */
void Creating_ext_File(CodeImage* CodeImageArr, int CodeImageArrLength, char* FileName, label* labelsArr, int labelsArrLength)
{
    int i;
    int flag = NO;
    char FILENAME[FILENAME_MAX];
    FILE* file = NULL;
    /* Iterate through the code image to find missing labels that refer to an external label */
    for (i = 0; i < CodeImageArrLength; i++) {
        /* Check if the instruction requires a label, and if that specific label is marked as extern in the symbol table */
        if (CodeImageArr[i].missing_label[0] != '\0' && labelsArr[isItInLabelTable(CodeImageArr[i].missing_label, labelsArr, labelsArrLength)].is_extern == YES)
        {
            /* Open the file only upon finding the first external reference */
            if (flag == NO)
            {
                sprintf(FILENAME, "%s.ext", FileName);
                file = fopen(FILENAME, "w");
                if (file == NULL) {
                    printf("Error: Could not create .ex file\n");
                    return;
                }
                flag = YES;
            }
            /* Print the external label name and the zero-padded 4-digit address where it was called */
            fprintf(file, "%s %04d\n", CodeImageArr[i].missing_label, CodeImageArr[i].Memory_address);
        }
    }
    /* Close the file safely if it was opened */
    if (file != NULL)
    {
        fclose(file);
    }
}
/*
 * Creates the main .ob (object) output file containing the final machine code and data image.
 * @param CodeImageArr The array representing the final, complete code instructions.
 * @param IC The final Instruction Counter value (number of code words).
 * @param DataImageArr The array representing the data section (numbers and strings).
 * @param DC The final Data Counter value (number of data words).
 * @param FileName The base name of the file (without extension) used to create the .ob file.
 */
void Creating_ob_File(CodeImage* CodeImageArr, int IC, DataImage* DataImageArr, int DC, char* FileName)
{
    char fullFileName[FILENAME_MAX];
    FILE* file = NULL;
    int i;
    char are_letter;

    /* Create the filename and open it for writing */
    sprintf(fullFileName, "%s.ob", FileName);
    file = fopen(fullFileName, "w");

    /* A small safety net in case the file doesn't open */
    if (file == NULL) {
        printf("Error: Could not create .ob file\n");
        return;
    }

    /*  Step 1: Print the header line */
    fprintf(file, "      %d %d\n", IC, DC);

    /* Step 2: Print the code array */
    for (i = 0; i < IC; i++) {
        /* Send the word to the function that will extract the corresponding letter */
        are_letter = Letter_extraction(CodeImageArr[i].machine_code);
        fprintf(file, "%04d %03X %c\n", CodeImageArr[i].Memory_address, (CodeImageArr[i].machine_code & TWELVE_BITS_MASK), are_letter);
    }

    /*  Step 3: Print the Data Image array (if it exists)  */
    for (i = 0; i < DC; i++)
    {
        fprintf(file, "%04d %03X A\n", (MEMORY_START + IC + i), (DataImageArr[i].machine_code & TWELVE_BITS_MASK));
    }

    /* Close the file upon completion */
    fclose(file);
}