#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "memory_manager.h"
unsigned short Push_Bit(unsigned int REA_number)
{
    return (REA_number << 12);
}
void looking_for_entry(label* labelsArr, FILE* amFile, int labelsArrLength, char* FileName, int* error_flag)
{
    char line[82] = { 0 };
    char word2[82] = { 0 }; /* Buffer to extract the first word of the line */
    char word1[82] = { 0 };
    char word3[82] = { 0 };
    int labelIndex;
    int  LineCounter = 0;
    rewind(amFile);
    while (fgets(line, sizeof(line), amFile) != NULL)
    {
        (LineCounter)++;
        /* Attempt to extract the first word from the line */
        if (sscanf(line, "%81s %81s %81s", word1, word2, word3) >= 2)
        {
            if (strcmp(word1, ".entry") == 0)
            {

                labelIndex = isItInLabelTable(word2, labelsArr, labelsArrLength);
                if (labelIndex != -1)
                {
                    labelsArr[labelIndex].is_entry = 1;
                }
                else
                {
                    printf("Error in line %d: Entry label '%s' not defined\n", LineCounter, word2);
                    *error_flag = 1;
                }
            }
            else  if (strcmp(word2, ".entry") == 0)
            {

                labelIndex = isItInLabelTable(word3, labelsArr, labelsArrLength);
                if (labelIndex != -1)
                {
                    labelsArr[labelIndex].is_entry = 1;
                }
                else
                {
                    printf("Error in line %d: Entry label '%s' not defined\n", LineCounter, word3);
                    *error_flag = 1;
                }
            }
        }
    }
}
int isItInLabelTable(char* isItLabel, label* labelsArr, int labelsArrLength)
{
    for (int i = 0; i < labelsArrLength; i++)
    {
        if (strcmp(isItLabel, labelsArr[i].labelName) == 0)
        {
            return i;
        }
    }
    return -1;
}
void Complete_Table(label* labelsArr, CodeImage* CodeImageArr, int labelsArrLength, int CodeImageArrLength, char* FileName, int* error_flag)
{
    char* labelName;
    int WordValue;
    int i;
    int index;
    int Addressing_method;
    for (i = 0; i < CodeImageArrLength; i++)
    {
        if (CodeImageArr[i].missing_label[0] != '\0')
        {

            if (CodeImageArr[i].missing_label[0] == '%')
            {
                Addressing_method = 2;
                labelName = &CodeImageArr[i].missing_label[1];
            }
            else
            {
                Addressing_method = 1; /* סימון שזו שיטה 1 */
                labelName = CodeImageArr[i].missing_label; /* חיפוש השם כפי שהוא */

            }
            index = isItInLabelTable(labelName, labelsArr, labelsArrLength);
            if (index != -1)
            {
                if (Addressing_method == 2)
                {
                    if (labelsArr[index].is_extern == 1)
                    {
                        printf("Error in line %d: The addressing method is inappropriate for an extern label\n", CodeImageArr[i].the_line_of_the_label);
                        *error_flag = 1;
                    }
                    else
                    {
                        WordValue = labelsArr[index].labelAddress - (CodeImageArr[i].Memory_address);
                        CodeImageArr[i].machine_code = ((unsigned short)WordValue & 0xFFF);
                    }

                }
                else if (Addressing_method == 1)
                {
                    if (labelsArr[index].is_extern == 1)
                    {
                        CodeImageArr[i].machine_code = 0 | Push_Bit(1);
                    }
                    else
                    {
                        CodeImageArr[i].machine_code = labelsArr[index].labelAddress | Push_Bit(2);
                    }
                }
            }
            else
            {
                printf("Error in line %d: label '%s' not found\n", CodeImageArr[i].the_line_of_the_label, CodeImageArr[i].missing_label);
                *error_flag = 1;
            }
        }
    }
}