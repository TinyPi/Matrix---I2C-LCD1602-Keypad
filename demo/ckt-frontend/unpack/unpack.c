#include <stdio.h>
#include <errno.h>
#include <signal.h>

#include "fifo_p.h"
#include "fifo_p.h"
#include "unpack.h"
#include "control.h"

#define FIFO_READ_NAME "/tmp/fifo_1"
#define FIFO_WRITE_NAME "/tmp/fifo_2"

#define FIFO_READ_MODE      O_RDWR
#define FIFO_WRITE_MODE     O_RDWR

void signalFIFO(int signal)
{
    switch (signal)
    {
        case SIGPIPE:
            printf("All %s write ports have been closed!!!\n", FIFO_READ_NAME);
    }
}

static int FIFO_read_env_init(int* read_fd)
{

    PDEBUG(LINFO, "begin!!!!\n");
    if(-1 == (access( FIFO_READ_NAME, F_OK)))
    {
        if (0 > (mkfifo(FIFO_READ_NAME, 0666)) && errno != EEXIST)
        {
           PDEBUG(LERR, "Create %s error\n", FIFO_READ_NAME);
            return -1;
        }
    }
    if(-1 == (*read_fd = open(FIFO_READ_NAME, FIFO_READ_MODE) ))
    {
        PDEBUG(LERR, "open %s fifo error!\n", FIFO_READ_NAME);
        return -1;
    }

//dont exit this process when read the fifo but there is not any write port 
// on it
    signal(SIGPIPE, signalFIFO);
    PDEBUG(LINFO, "end!!!!\n");
    return 0;

}


static int FIFO_write_env_init(int* write_fd)
{
    PDEBUG(LINFO, "Begin!!!\n");
    if( -1 == (access( FIFO_WRITE_NAME, F_OK)))
    {
        if (0 > (mkfifo(FIFO_WRITE_NAME, 0666)) && errno != EEXIST)
        {
            PDEBUG(LERR, "Create %s error\n", FIFO_WRITE_NAME);
            return -1;
        }
    }

    if(-1 == (*write_fd = open(FIFO_WRITE_NAME, FIFO_WRITE_MODE)))
    {
        PDEBUG(LERR, "open %s fifo error!\n", FIFO_WRITE_NAME);
        return -1;
    }

//dont exit this process when read the fifo but there is not any write port 
// on it
    signal(SIGPIPE, signalFIFO);

    PDEBUG(LINFO, "end!!!\n");

    return 0;
}

int unpack_env_int(int* read_fd, int* write_fd)
{
    return FIFO_read_env_init(read_fd) || FIFO_write_env_init(write_fd);
}

void FIFO_read_env_deinit(int read_fd)
{
    close(read_fd);
}

void FIFO_write_env_deinit(int write_fd)
{
    close(write_fd);
}

void unpack_env_deinit(int read_fd, int write_fd)
{
    FIFO_read_env_deinit(read_fd);
    FIFO_write_env_deinit(write_fd);
}

int get_data_from_FIFO(int read_fd, uchar *databuff4R)
{
    int ret;
    int i =0;

    memset(databuff4R, 0, ROBOT_DATA_MAX_LEN);
    ret = read_p(read_fd, (char *)databuff4R, ROBOT_DATA_MAX_LEN);
    if(0 > ret)
    {
        PDEBUG(LERR, "**************fifo read error!!!!!\n");
        return -1;
    }

    PDEBUG(LINFO, "From FIFO Data is(%d):", ret);
    for(; i < ret; ++i)
    {
        PDEBUG(LINFO, "[%x]", databuff4R[i]);
    }
    PDEBUG(LINFO, "\n");

    return 0;
}

#if 0
int SendData2Bg(struct LCDData LCDData)
{
    int ret;
    int i =0;

    printf("%s\n", __func__);

    LCD2FIFOBuf(LCDData);

    printf("Write ot FIFO Data is(%d):", *databuff4W);
    for(; i <= *databuff4W; ++i)
    {
        printf("[%x]", databuff4W[i]);
    }
    printf("\n");

    ret = write_p(write_fd, (char *)databuff4W, (*databuff4W) + 1);           //The first byte of databuff4W is data length
    if(0 > ret)
    {
        printf("fifo write error!!\n");
        return -1;
    }

    return 0;
}
#endif
int get_package(int read_fd, package* package)
{
    uint privdata_len = 0;
    int i = 0;
    uchar databuff4R[ROBOT_DATA_MAX_LEN] = { 0 };

    if (0 != get_data_from_FIFO(read_fd, databuff4R))
    {
        PDEBUG(LERR, "Get data from fifo error!!!\n");
        return -1;
    }

    PDEBUG(LDEBUG, "memcpy datalen");
    memcpy(&(package->dataLen), databuff4R + BUF_LENF_OFFSET, sizeof(package->dataLen));
    PDEBUG(LDEBUG, "memcpy sourceflag");
    memcpy(&(package->sourceFlag), databuff4R + BUF_SRCF_OFFSET, sizeof(package->sourceFlag));
    PDEBUG(LDEBUG, "memcpy command");
    memcpy(package->priv_data.command, databuff4R + BUF_COMM_OFFSET, NUM(package->priv_data.command));

    privdata_len = package->dataLen - sizeof(package->sourceFlag) - NUM(package->priv_data.command);         // 2 
    PDEBUG(LDEBUG, "memcpy priv,priv len[%d]", privdata_len);
    memcpy(package->priv_data.priv, databuff4R + BUF_PRIV_OFFSET, privdata_len);
    package->priv_data.priv_len= privdata_len;

    PDEBUG(LINFO, "Robot command offset:%d\n", BUF_PRIV_OFFSET);
    PDEBUG(LDEBUG, "package->dataLen:%d\n", package->dataLen);
    PDEBUG(LDEBUG, "package->sourceFlag:%x\n", package->sourceFlag);
    PDEBUG(LDEBUG, "package->RobotData.command:");
    for(; i < NUM(package->priv_data.command); ++i)
    {
        PDEBUG(LDEBUG, "[%x]", package->priv_data.command[i]);
    }
    PDEBUG(LDEBUG, "\n");

    return 0;
}
#if 0
static int LCD2FIFOBuf(struct LCDData LCDData)
{
    int dataLen = 0;
    uchar srcFlag = 0;
    uchar databuff4W[ROBOT_DATA_MAX_LEN] = { 0 };


    dataLen = sizeof(char) + NUM(LCDData.command) + LCDData.privLen;
    memcpy(databuff4W+ BUF_LENF_OFFSET, &dataLen, sizeof(FIFOData4W.dataLen));
    memcpy(databuff4W + BUF_SRCF_OFFSET, &(srcFlag), sizeof(char));          //1 TBD
    memcpy(databuff4W + BUF_COMM_OFFSET, LCDData.command, NUM(LCDData.command));
    memcpy(databuff4W + BUF_PRIV_OFFSET, LCDData.priv, LCDData.privLen);

    return 0;
}
#endif
