#include "memory_manager.h"
int isItInLabelTable(char* isItLabel, label* labelsArr, int labelsArrLength);
unsigned short Push_Bit(unsigned int REA_number);
void looking_for_entry(label* labelsArr, FILE* amFile, int labelsArrLength, char* FileName, int* error_flag);
void Complete_Table(label* labelsArr, CodeImage* CodeImageArr, int labelsArrLength, int CodeImageArrLength, char* FileName, int* error_flag);