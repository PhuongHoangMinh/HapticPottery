#ifndef HDPROC_H__20071025__INCED
#define HDPROC_H__20071025__INCED

#include <HD/hd.h>




int HDInit();


void GeneratePotteryHIP(int pType, double height, double radius);


void DeletePotteryHIP();


HDCallbackCode HDCALLBACK GetHIPPosition(void *data);


void HapticProc();


#endif    
