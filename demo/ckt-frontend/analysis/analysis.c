#include <stdio.h>
#include <memory.h>
#include "analysis.h"


int DataFront2Bg(void *Front, void *bg)
{
    return DataBuf2FIFOData(Front, bg);
}

int DataBuf2FIFOData(void *source, void *dist)
{
    int RobotDataLen = 0;

    memcpy(&FIFOData4R.dataLen, buf + BUF_LENF_OFFSET, sizeof(FIFOData4R.dataLen));
    memcpy(&FIFOData4R.sourceFlag, buf + BUF_SRCF_OFFSET, sizeof(FIFOData4R.sourceFlag));
    memcpy(&FIFOData4R.RobotData.command, buf + BUF_COMM_OFFSET, sizeof(FIFOData4R.RobotData.command));

    RobotDataLen = FIFOData4R.dataLen - BUF_PRIV_OFFSET+ 1;         // 2 
    memcpy(FIFOData4R.RobotData.priv, buf + BUF_PRIV_OFFSET, RobotDataLen);

    return 0;
}