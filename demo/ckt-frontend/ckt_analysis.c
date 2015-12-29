#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "libfahw.h"
#include "utils.h"
#include "fifo_p.h"
#include "ckt_analysis.h"

#define FIFO_READ_NAME "/tmp/fifo_1"
#define FIFO_WRITE_NAME "/tmp/fifo_2"


static int fd_read, fd_write;
static int client_sockfd;
static pthread_t ReadId, WriteId;
static char DataBuffer4R[256];
static char DataBuffer4W[256];

static struct FIFOData FIFOData4R;
static struct FIFOData FIFOData4W;


void signalFIFO(int signal)
{
    switch (signal)
    {
        case SIGPIPE:
            printf("All %s write ports have been closed!!!\n", FIFO_READ_NAME);
    }
}

static int DataBuf2FIFOData(char *buf)
{
    int RobotDataLen = 0;

    memcpy(&FIFOData4R.dataLen, buf + ROBOT_DATA_LENF_OFFSET, sizeof(FIFOData4R.dataLen));
    memcpy(&FIFOData4R.sourceFlag, buf + ROBOT_DATA_SRCF_OFFSET, sizeof(FIFOData4R.sourceFlag));
    memcpy(&FIFOData4R.RobotData.command, buf + ROBOT_DATA_COMM_OFFSET, sizeof(FIFOData4R.RobotData.command));

    RobotDataLen = FIFOData4R.dataLen - ROBOT_DATA_PRIV_OFFSET + 1;         // 2 
    memcpy(FIFOData4R.RobotData.priv, buf + ROBOT_DATA_PRIV_OFFSET, RobotDataLen);

    return 0;
}

static int FIFOData2DataBuf(struct FIFOData FIFOData)
{
    memcpy(DataBuffer4W + ROBOT_DATA_LENF_OFFSET, &FIFOData.dataLen, sizeof(FIFOData.dataLen));
    memcpy(DataBuffer4W + ROBOT_DATA_SRCF_OFFSET, &FIFOData.sourceFlag, sizeof(FIFOData.sourceFlag));
    memcpy(DataBuffer4W + ROBOT_DATA_COMM_OFFSET, &FIFOData.RobotData.command, 
            sizeof(FIFOData.RobotData.command));
    memcpy(DataBuffer4W + ROBOT_DATA_PRIV_OFFSET, FIFOData.RobotData.priv, 
            strlen(FIFOData.RobotData.priv));

    return 0;
}

static int DataTransfer2LCD(struct RobotData RobotData)
{
    char *toLCD;
    int dataLen;

    dataLen = sizeof(RobotData.command) + strlen(RobotData.priv);
    toLCD = (char *)malloc(dataLen);
    memset(toLCD, 0, dataLen);

    memcpy(toLCD, RobotData.command, sizeof(RobotData.command));
    memcpy(toLCD + sizeof(RobotData.command), RobotData.priv, strlen(RobotData.priv));
//send data to frontend
    send(client_sockfd, toLCD, dataLen, 0);

    free(toLCD);
    return 0;
}

static int DataRecieveFromLCD()
{
    char *FromLCD;
    int dataLen;

    FromLCD = (char *)malloc(ROBOT_DATA_MAX_LEN);
    memset(FromLCD, 0, ROBOT_DATA_MAX_LEN);
    recv(client_sockfd, FromLCD, ROBOT_DATA_MAX_LEN, 0);

    memcpy(FIFOData4W.RobotData.command, FromLCD, sizeof(FIFOData4W.RobotData.command));
    memcpy(FIFOData4W.RobotData.priv, FromLCD + sizeof(FIFOData4W.RobotData.command), ROBOT_DATA_MAX_LEN - 10);

    dataLen = strlen(FIFOData4W.RobotData.priv);

    FIFOData4W.dataLen = sizeof(FIFOData4W.sourceFlag) + sizeof(FIFOData4W.RobotData.command) + dataLen;

    FIFOData2DataBuf(FIFOData4W);

    free(FromLCD);

    return 0;
}

void FIFOReadThread()
{
    int ret;
    int num = sizeof(DataBuffer4R);
    do
    {
        ret = read_p(fd_read, DataBuffer4R, num);
        if(0 > ret)
        {
            printf("fifo read error!!!!!\n");
            continue;
        }

        ret = DataBuf2FIFOData(DataBuffer4R);
        if (0 != ret)
        {
            printf("%s: DataBuf2FIFOData error!!\n", __FILE__);
            exit(-1);
        }

        ret = DataTransfer2LCD(FIFOData4R.RobotData);
        if (0 != ret)
        {
            printf("%s: DataTransfer error!!\n", __FILE__);
            exit(-1);
        }

    }while (0 != ret);

}

void FIFOWriteThread()
{
    int ret = 0;

    do
    {
        ret = DataRecieveFromLCD();
        if (0 != ret)
        {
            printf("%s: Data recieve for LCD error!!\n", __FILE__);
            exit(-1);
        }

        ret = write_p(fd_write, DataBuffer4W, sizeof(DataBuffer4W));
        if(0 > ret)
        {
            printf("%s: fifo write error!!\n", __FILE__);
            exit(-1);
        }
    }while (0 != ret);

}

int FIFOEnvInit()
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

int SocketEnvInit()
{
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(18000);

    if((client_sockfd=socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket\n");
        return -1;
    }

    printf("connected to server\n");
    if(connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct 
        sockaddr)) < 0)
    {
        perror("connect\n");
        return -1;
    }

    return 0;
}


void FIFOEnvDeInit()
{
    pthread_join(ReadId, NULL);
    pthread_join(WriteId, NULL);

    close(fd_read);
    close(fd_write);
}

void SocketEnvDeInit()
{
    close(client_sockfd);
}

int EnvIinit()
{
    if (-1 == FIFOEnvInit())
    {
        printf("FIFO environment init error!\n");
        return -1;
    }
    if (-1 == SocketEnvInit())
    {
        printf("Socket environment init error!\n");
        return -1;
    }

    FIFOData4R.RobotData.priv= (char *)malloc(ROBOT_PRIV_LEN);
    memset(FIFOData4R.RobotData.priv, 0 , ROBOT_PRIV_LEN);

    FIFOData4W.RobotData.priv= (char *)malloc(ROBOT_PRIV_LEN);
    memset(FIFOData4W.RobotData.priv, 0 , ROBOT_PRIV_LEN);

    return 0;
}

void EnvDeIinit()
{
    FIFOEnvDeInit();
    SocketEnvDeInit();

    free(FIFOData4R.RobotData.priv);
    free(FIFOData4R.RobotData.priv);
}


int main(int argc, char *argv[])
{
//dont exit this process when read the fifo but there is not any write port 
// on it
    signal(SIGPIPE, signalFIFO);

    if (0 > EnvIinit())
    {
        printf("Envinit error!\n");
        return -1;
    }

    EnvDeIinit();

    printf("exit!!!!!!!!!\n");
    return 0;
}
