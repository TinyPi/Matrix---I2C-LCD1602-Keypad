#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include "fifo_p.h"
#include "fifo_p.h"
#include "from.h"

#define FIFO_READ_NAME "/tmp/fifo_1"
#define FIFO_WRITE_NAME "/tmp/fifo_2"

#define FIFO_READ_MODE      O_RDONLY
#define FIFO_WRITE_MODE     O_WRONLY

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
    if(-1 == (read_fd = open(FIFO_READ_NAME, FIFO_READ_MODE) ))
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

    if(-1 == (write_fd= open(FIFO_WRITE_NAME, FIFO_WRITE_MODE)))
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
    int i =0;

    printf("%s\n", __func__);
    memset(databuff4R, 0, ROBOT_DATA_MAX_LEN);
    ret = read_p(read_fd, databuff4R, ROBOT_DATA_MAX_LEN);
    if(0 > ret)
    {
        printf("**************fifo read error!!!!!\n");
        return -1;
    }

    printf("From FIFO Data is(%d):", ret);
    for(; i < ret; ++i)
    {
        printf("[%x]", databuff4R[i]);
    }
    printf("\n");

    return ret;
}


int SendData2Bg()
{
    int ret;
    int i =0;

    printf("%s\n", __func__);
    printf("Write ot FIFO Data is(%d):", *databuff4W);
    for(; i < *databuff4W; ++i)
    {
        printf("[%x]", databuff4W[i]);
    }
    printf("\n");

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
    int i = 0;

    printf("%s\n", __func__);
    memcpy(&(FIFOData->dataLen), databuff4R + BUF_LENF_OFFSET, sizeof(FIFOData->dataLen));
    memcpy(&(FIFOData->sourceFlag), databuff4R + BUF_SRCF_OFFSET, sizeof(FIFOData->sourceFlag));
    memcpy(FIFOData->RobotData.command, databuff4R + BUF_COMM_OFFSET, NUM(FIFOData->RobotData.command));

    RobotDataLen = FIFOData->dataLen - sizeof(FIFOData->sourceFlag) - NUM(FIFOData->RobotData.command);         // 2 
    memcpy((char *)FIFOData->RobotData.priv, databuff4R + BUF_PRIV_OFFSET, RobotDataLen);

    printf("Robot command offset:%d\n", BUF_COMM_OFFSET);
    printf("FIFOData->dataLen:%x\n", FIFOData->dataLen);
    printf("FIFOData->sourceFlag:%x\n", FIFOData->sourceFlag);
    printf("FIFOData->RobotData.command:");
    for(; i < NUM(FIFOData->RobotData.command); ++i)
    {
        printf("[%x]", FIFOData->RobotData.command[i]);
    }
    printf("\n");

    return 0;
}

int LCD2FIFOBuf(struct LCDData LCDData)
{
    int dataLen = 0;
    char srcFlag = 0;

    printf("%s\n", __func__);

    dataLen = sizeof(char) + NUM(LCDData.command) + LCDData.privLen;
    memcpy(databuff4W+ BUF_LENF_OFFSET, &dataLen, sizeof(FIFOData4W.dataLen));
    memcpy(databuff4W + BUF_SRCF_OFFSET, &(srcFlag), sizeof(char));          //1 TBD
    memcpy(databuff4W + BUF_COMM_OFFSET, LCDData.command, NUM(LCDData.command));
    memcpy(databuff4W + BUF_PRIV_OFFSET, LCDData.priv, LCDData.privLen);

    return 0;
}
