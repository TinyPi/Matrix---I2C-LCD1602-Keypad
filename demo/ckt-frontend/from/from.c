#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include "fifo_p.h"
#include "fifo_p.h"
#include "from.h"

#define FIFO_READ_NAME "/tmp/fifo_1"
#define FIFO_WRITE_NAME "/tmp/fifo_2"

static int read_fd, write_fd;
static char *databuff4R = NULL;
static char *databuff4W = NULL;
//static struct FIFOData FIFOData4R;
static struct FIFOData FIFOData4W;

void signalFIFO(int signal)
{
    switch (signal)
    {
        case SIGPIPE:
            printf("All %s write ports have been closed!!!\n", FIFO_READ_NAME);
    }
}

int FIFOReadEnvInit(void)
{

    printf("%s  begin!!!\n", __func__);
    if(-1 == (access( FIFO_READ_NAME, F_OK)))
    {
        if (0 > (mkfifo(FIFO_READ_NAME, 0666)) && errno != EEXIST)
        {
            printf("Create %s error\n", FIFO_READ_NAME);
            return -1;;
        }
    }
    if(-1 == (read_fd = open(FIFO_READ_NAME, O_RDWR) ))
    {
        printf("open %s fifo error!\n", FIFO_READ_NAME);
        return -1;
    }

//dont exit this process when read the fifo but there is not any write port 
// on it
    signal(SIGPIPE, signalFIFO);

    databuff4R = (char*)malloc(ROBOT_DATA_MAX_LEN);
    if(NULL == databuff4R)
    {
        printf("databuff4R malloc error!!!!\n");
        return -1;
    }

    printf("%s  end!!!\n", __func__);
    return 0;

}


int FIFOWriteEnvInit(void)
{
    printf("%s  begin!!!\n", __func__);
    if( -1 == (access( FIFO_WRITE_NAME, F_OK)))
    {
        if (0 > (mkfifo(FIFO_WRITE_NAME, 0666)) && errno != EEXIST)
        {
            printf("Create %s error\n", FIFO_WRITE_NAME);
            return -1;
        }
    }

    if(-1 == (write_fd= open(FIFO_WRITE_NAME, O_RDWR)))
    {
        printf("open %s fifo error!\n", FIFO_WRITE_NAME);
        return -1;
    }

//dont exit this process when read the fifo but there is not any write port 
// on it
    signal(SIGPIPE, signalFIFO);

    databuff4W = (char*)malloc(ROBOT_DATA_MAX_LEN);
    if(NULL == databuff4W)
    {
        printf("databuff4W malloc error!!!\n");
        goto ERR;
    }

    FIFOData4W.RobotData.priv= (char *)malloc(ROBOT_PRIV_LEN);
    if(NULL == FIFOData4W.RobotData.priv)
    {
        printf("FIFOData4W malloc error!!!\n");
        goto FAER;
    }
    printf("%s  end!!!\n", __func__);

    return 0;

FAER:
    free(databuff4W);

ERR:
    return -1;
}

void FIFOReadEnvDeInit()
{
    printf("%s\n", __func__);
    free(databuff4R);
    close(read_fd);
}

void FIFOWriteEnvDeInit()
{
    printf("%s\n", __func__);
    free(databuff4W);
    free(FIFOData4W.RobotData.priv);
    close(write_fd);
}

int GetDataFromFIFO()
{
    int ret;

    printf("%s\n", __func__);
    memset(databuff4R, 0, ROBOT_DATA_MAX_LEN);
    ret = read_p(read_fd, databuff4R, ROBOT_DATA_MAX_LEN);
    if(0 > ret)
    {
        printf("fifo read error!!!!!\n");
        return -1;
    }

    printf("From FIFO Data is %s", databuff4R);
    return ret;
}

static void FIFO2Buf()
{
    printf("%s\n", __func__);
    memcpy(databuff4W+ ROBOT_DATA_LENF_OFFSET, &FIFOData4W.dataLen, sizeof(FIFOData4W.dataLen));
    memcpy(databuff4W + ROBOT_DATA_SRCF_OFFSET, &FIFOData4W.sourceFlag, sizeof(FIFOData4W.sourceFlag));
    memcpy(databuff4W + ROBOT_DATA_COMM_OFFSET, FIFOData4W.RobotData.command, 
            NUM(FIFOData4W.RobotData.command));
    memcpy(databuff4W + ROBOT_DATA_PRIV_OFFSET, FIFOData4W.RobotData.priv, 
            strlen(FIFOData4W.RobotData.priv));

}

int SendData2Bg()
{
    int ret;

    FIFO2Buf();

    printf("%s\n", __func__);
    ret = write_p(write_fd, databuff4W, *databuff4W);           //The first byte of databuff4W is data length
    if(0 > ret)
    {
        printf("fifo write error!!\n");
        return -1;
    }

    return 0;
}

int Buffer2FIFO(struct FIFOData *FIFOData)
{
    int RobotDataLen = 0;

    printf("%s\n", __func__);
    memcpy(&(FIFOData->dataLen), databuff4R + ROBOT_DATA_LENF_OFFSET, sizeof(FIFOData->dataLen));
    memcpy(&(FIFOData->sourceFlag), databuff4R + ROBOT_DATA_SRCF_OFFSET, sizeof(FIFOData->sourceFlag));
    memcpy(FIFOData->RobotData.command, databuff4R + ROBOT_DATA_COMM_OFFSET, NUM(FIFOData->RobotData.command));

    RobotDataLen = FIFOData->dataLen - ROBOT_DATA_SRCF_OFFSET;         // 2 
    memcpy((char *)FIFOData->RobotData.priv, databuff4R + ROBOT_DATA_PRIV_OFFSET, RobotDataLen);

    return 0;
}

int LCD2FIFO(struct LCDData LCDData)
{
    printf("%s\n", __func__);
    memcpy(FIFOData4W.RobotData.command, LCDData.command, NUM(LCDData.command));

    memset(FIFOData4W.RobotData.priv, 0, ROBOT_PRIV_LEN);
    memcpy(FIFOData4W.RobotData.priv, LCDData.priv, strlen(LCDData.priv));
    FIFOData4W.sourceFlag = 1;              //1 TBD
    FIFOData4W.dataLen = sizeof(FIFOData4W.sourceFlag) + NUM(FIFOData4W.RobotData.command) + 
            strlen(FIFOData4W.RobotData.priv);

    return 0;
}
