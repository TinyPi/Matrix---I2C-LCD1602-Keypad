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

#define FIFO_READ_NAME "/temp/fifo/fifo_1"
#define FIFO_WRITE_NAME "/temp/fifo/fifo_2"
#define ROBOT_DATA_MAX_LEN 256

static int fd_read, fd_write;
static int client_sockfd;
static pthread_t ReadId, WriteId;
static char DataBuffer[256];

static struct RobotData RobotData;

void signalFIFO(int signal)
{
    switch (signal)
    {
        case SIGPIPE:
            printf("All %s write ports have been closed!!!\n", FIFO_READ_NAME);
    }
}

static int DataTransfer()
{
    send(client_sockfd, RobotData.command, strlen(RobotData.command), 0);

    return 0;
}

static int DataAnalysis(char *buf)
{
    int len = 0, i = 0;

    len = buf[0];
    memcpy(RobotData.command, buf + 1, 8);

    if(0 == strlen(RobotData.command))
    {
        printf("command length error!!!!!\n");
        return -1;
    }
    printf("Robot command is:");
    for(; '\0' != RobotData.command[i]; ++i)
    {
        printf("%c", RobotData.command[i]);
    }
    printf("\n");

//1 DATA TBD
    RobotData.priv= (char *) (buf + (len - 8 - 1));

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
            printf("%s: DataAnalysis error!!\n", __FILE__);
            exit(-1);
        }

        ret = DataTransfer();
        if (0 != ret)
        {
            printf("%s: DataTransfer error!!\n", __FILE__);
            exit(-1);
        }

    }while (-1 == ret);

}

void FIFOWriteThread()
{
    //write();
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

int main(int argc, char *argv[])
{
//dont exit this process when read the fifo but there is not any write port 
// on it
    signal(SIGPIPE, signalFIFO);

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

    FIFOEnvDeInit();
    SocketEnvDeInit();
    return 0;
}
