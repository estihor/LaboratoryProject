#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "second_pass.h"
#include "memory_manager.h"
unsigned short Push_Bit_For_Letter_extraction(unsigned short REA_number)
{
    return (REA_number >> 12);
}
char  Letter_extraction(unsigned short BinaryNumber)
{
    unsigned short Letter;
    Letter = (unsigned short)Push_Bit_For_Letter_extraction(BinaryNumber & 12288);
    if (Letter == 0)
    {
        return 'A';
    }
    else if (Letter == 1)
    {
        return 'E';
    }
    return 'R';
}

void Creating_ent_File(label* labelsArr, int labelsArrLength, char* FileName)
{
    int i;
    int flag = 0;
    char FILENAME[FILENAME_MAX];
    FILE* file = NULL;
    for (i = 0; i < labelsArrLength; i++) {
        if (labelsArr[i].is_entry == 1) {
            if (flag == 0)
            { /* פותח קובץ רק בפעם הראשונה שמוצא */
                sprintf(FILENAME, "%s.ent", FileName);
                file = fopen(FILENAME, "w");
                if (file == NULL) {
                    printf("Error: Could not create .ent file\n");
                    return;
                }
                flag = 1;
            }
            fprintf(file, "%s %04d\n", labelsArr[i].labelName, labelsArr[i].labelAddress);
        }
    }
    if (file != NULL)
    {
        fclose(file);
    }
}
void Creating_ext_File(CodeImage* CodeImageArr, int CodeImageArrLength, char* FileName, label* labelsArr, int labelsArrLength)
{
    int i;
    int flag = 0;
    char FILENAME[FILENAME_MAX];
    FILE* file = NULL;
    for (i = 0; i < CodeImageArrLength; i++) {
        if (CodeImageArr[i].missing_label[0] != '\0' && labelsArr[isItInLabelTable(CodeImageArr[i].missing_label, labelsArr, labelsArrLength)].is_extern == 1)
        {
            if (flag == 0)
            { /* פותח קובץ רק בפעם הראשונה שמוצא */
                sprintf(FILENAME, "%s.ext", FileName);
                file = fopen(FILENAME, "w");
                if (file == NULL) {
                    printf("Error: Could not create .ex file\n");
                    return;
                }
                flag = 1;
            }
            fprintf(file, "%s %04d\n", CodeImageArr[i].missing_label, CodeImageArr[i].Memory_address);
        }
    }
    if (file != NULL)
    {
        fclose(file);
    }
}
void Creating_ob_File(CodeImage* CodeImageArr, int IC, DataImage* DataImageArr, int DC, char* FileName)
{
    char fullFileName[FILENAME_MAX];
    FILE* file = NULL;
    int i;
    char are_letter;

    /* יוצרים את שם הקובץ ופותחים אותו לכתיבה */
    sprintf(fullFileName, "%s.ob", FileName);
    file = fopen(fullFileName, "w");

    /* רשת ביטחון קטנה למקרה שהקובץ לא נפתח */
    if (file == NULL) {
        printf("Error: Could not create .ob file\n");
        return;
    }

    /* --- שלב 1: הדפסת שורת הכותרת --- */
    fprintf(file, "%d %d\n", IC, DC);

    /* --- שלב 2: הדפסת מערך הקוד --- */
    for (i = 0; i < IC; i++) {
        /* שולחים את המילה לפונקציה שלך שתחלץ את האות המתאימה */
        are_letter = Letter_extraction(CodeImageArr[i].machine_code);

        /* * הדפסה לפי הפורמט:
         * %04d = כתובת בעשרוני (4 ספרות)
         * %03X = מילת המכונה בהקסדצימלי (3 ספרות). עושים & 0xFFF כדי להדפיס רק 12 ביט
         * %c = האות שחילצנו
         */
        fprintf(file, "%04d %03X %c\n", CodeImageArr[i].Memory_address, (CodeImageArr[i].machine_code & 0xFFF), are_letter);
    }

    /* --- שלב 3: הדפסת מערך הנתונים (אם יש כזה) --- */
    for (i = 0; i < DC; i++) {
        /* בנתונים האות היא תמיד 'A' כי הם מוחלטים, אז מדפיסים אותה ישירות */
        fprintf(file, "%04d %03X A\n", DataImageArr[i].MemoryAddress, (DataImageArr[i].machine_code & 0xFFF));
    }

    /* סוגרים את הקובץ בסיום */
    fclose(file);
}