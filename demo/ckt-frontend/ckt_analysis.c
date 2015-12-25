#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include "libfahw.h"
#include "utils.h"
#include "fifo_p.h"
#include "ckt_analysis.h"

#define FIFO_READ_NAME "/temp/fifo/fifo_1"
#define FIFO_WRITE_NAME "/temp/fifo/fifo_2"
#define ROBOT_DATA_MAX_LEN 256

static int fd_read, fd_write;
pthread_t ReadId, WriteId;
char DataBuffer[256];

void signalFIFO(int signal)
{
    switch (signal)
    {
        case SIGPIPE:
            printf("All %s write ports have been closed!!!\n", FIFO_READ_NAME);
    }
}
int DataAnalysis(char *buf)
{
    char command[8] = {'0'};            //for 8 bytes command
    char *data;
    int len = 0, i = 0;

    len = buf[0];
    memcpy(command, buf + 1, 8);
    for(; '\0' != command[i]; ++i)
        {
            printf("command is %c\n", command[i]);
        }
//1 DATA TBD
    data = buf + (len - 8 - 1);

    return 0;
}

void FIFOReadThread()
{
    int ret;
    int num = sizeof(DataBuffer);
    do
    {
        ret = read_p(fd_read, DataBuffer, num);
        if(0 != ret)
        {
            printf("fifo read error!!!!!\n");
            continue;
        }

        ret = DataAnalysis(DataBuffer);
        if (0 != ret)
            {
                printf("%s:error!!\n", __FILE__);
                exit(-1);
            }
    }while (-1 == ret);

}

void FIFOWriteThread()
{
    //write();
}

int EnvInit()
{
    int ret;
    if(-1 == (access( FIFO_READ_NAME, F_OK)))
    {
        if (0 > (mkfifo(FIFO_READ_NAME, 0666)) && errno != EEXIST)
        {
            printf("Create %s error\n", FIFO_READ_NAME);
            goto ORE;;
        }
    }
    if(-1 == (fd_read = open(FIFO_READ_NAME, O_RDONLY) ))
    {
        printf("open %s fifo error!\n", FIFO_READ_NAME);
        goto ORE;
    }

    if( -1 == (access( FIFO_WRITE_NAME, F_OK)))
    {
        if (0 > (mkfifo(FIFO_WRITE_NAME, 0666)) && errno != EEXIST)
        {
            printf("Create %s error\n", FIFO_WRITE_NAME);
            goto OWE;
        }
    }

    if(-1 == (fd_write= open(FIFO_WRITE_NAME, O_WRONLY)))
    {
        printf("open %s fifo error!\n", FIFO_WRITE_NAME);
        goto OWE;
    }

    ret=pthread_create(&ReadId,NULL,(void *) FIFOReadThread,NULL);
    if(0 != ret)
    {
        printf("Create read thread error!\n");
        goto ERR;
    }

    ret=pthread_create(&WriteId,NULL,(void *) FIFOWriteThread,NULL);
    if(0 != ret)
    {
        printf("Create wirte thread error!\n");
        goto ERR;
    }

ORE:
    return -1;

OWE:
    close(fd_read);
    return -1;

ERR:
    close(fd_read);
    close(fd_write);
    return -1;
}

void EnvReInit()
{
    close(fd_read);
    close(fd_write);

    pthread_join(ReadId, NULL);
    pthread_join(WriteId, NULL);
}

int main(int argc, char *argv[])
{
    char *originalData;
    //RobotDada data;

//dont exit this process when i read the fifo but there is not any write port 
// on it
    signal(SIGPIPE, signalFIFO);

    if (-1 ==EnvInit())
    {
        printf("Environment inti error!\n");
        return -1;
    }
    //get data which is from FIFO sent by backgrond
    //originalData = (char *)malloc(sizeof(char) * ROBOT_DATA_MAX_LEN);
    //originalData = read_p(FIFO_NAME, O_RDONLY);

    EnvReInit();
    return 0;
}
