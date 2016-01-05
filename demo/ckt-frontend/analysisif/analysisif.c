#include <memory.h>
#include "analysisif.h"
#include "../from/from.h"
#include "../to/to.h"

char *FIFOprivData = NULL;
char *RobotprivData = NULL;

int AnaIfInit()
{
    FIFOprivData = (char*)malloc(ROBOT_PRIV_LEN);
    RobotprivData = (char*)malloc(ROBOT_PRIV_LEN);

    return 0;
}

void AnaIfDeInit()
{
    free(FIFOprivData);
    free(RobotprivData);
}

int FromBgEnvInit()
{
    return FIFOReadEnvInit();
}

int ToBgEnvInit()
{
    return FIFOWriteEnvInit();
}

int FromFrontEnvInit()
{
    return SocketEnvInit();
}

int ToFrontEnvInit()
{
    return 0;
}


void FromBgEnvDeInit()
{
    FIFOReadEnvDeInit();
}

void ToBgEnvDeInit()
{
    FIFOWriteEnvDeInit();
}

void FromFrontEnvDeInit()
{
    SocketEnvDeInit();
}

void ToFrontEnvDeInit()
{
    ;
}

int GetDataFromBg()
{
    return GetDataFromFIFO();
}

int GetDataFromFront()
{
    return GetDataFromLCD();
}

int DataTransfer2Front()
{
    int privLen = 0;
    struct FIFOData FIFOData;
    memset(FIFOprivData, 0, ROBOT_PRIV_LEN);
    FIFOData.RobotData.priv = FIFOprivData;
    Buffer2FIFO(&FIFOData);
    if (0 == (privLen = FIFO2LCDBuf(FIFOData)))
    {
        printf("The length of private data of FIFO data to lcd is 0!!!\n");
    }
    if (0 != SendData2LCD(privLen + NUM(FIFOData.RobotData.command)))
    {
        printf("Send to LCD error!!!\n");
        return -1;
    }

    return 0;
}

int DataTransfer2Bg()
{
    struct LCDData LCDData;
    memset(RobotprivData, 0, ROBOT_PRIV_LEN);
    LCDData.priv = RobotprivData;
    buffer2LCD(&LCDData);
    LCD2FIFOBuf(LCDData);
    if (0 != SendData2Bg())
        {
            printf("Send to fifo error!!!!\n");
            return -1;
        }

    return 0;
}
