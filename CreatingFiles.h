#include "second_pass.h"

unsigned short Push_Bit_For_Letter_extraction(unsigned short REA_number);
char  Letter_extraction(unsigned short BinaryNumber);
void Creating_ent_File(label* labelsArr, int labelsArrLength, char* FileName);
void Creating_ext_File(CodeImage* CodeImageArr, int CodeImageArrLength, char* FileName, label* labelsArr, int labelsArrLength);
void Creating_ob_File(CodeImage* CodeImageArr, int IC, DataImage* DataImageArr, int DC, char* FileName);