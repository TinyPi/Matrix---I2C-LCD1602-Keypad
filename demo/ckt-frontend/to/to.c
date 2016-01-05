#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <linux/sockios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include "utils.h"
#include "to.h"
#include "../from/from.h"

static int clientSockfd = -1;
static struct LCDData fromLCD;
static char toLCDBuf[ROBOT_DATA_MAX_LEN] = {'0'};
static char fromLCDBuf[ROBOT_DATA_MAX_LEN] = {'0'};

int SocketEnvInit()
{
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(8000);

    if((clientSockfd=socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket\n");
        return -1;
    }

    printf("connected to server\n");
    if(connect(clientSockfd, (struct sockaddr *)&remote_addr, sizeof(struct 
        sockaddr)) < 0)
    {
        perror("connect\n");
        return -1;
    }

    printf("connected to server 127.0.0.1 successfully\n");

    fromLCD.priv = (char *)malloc(ROBOT_PRIV_LEN);
    if(NULL == fromLCD.priv)
    {
        printf("fromLCD.priv malloc error!!!\n");
        goto ERR;
    }

    memset(fromLCD.priv, 0, ROBOT_PRIV_LEN);
    return 0;

ERR:
    return -1;
}

void SocketEnvDeInit()
{
    printf("%s\n", __func__);
    free(fromLCD.priv);

    close(clientSockfd);
}

int FIFO2LCDBuf(struct FIFOData FIFOData)
{
    printf("%s\n", __func__);
    int privLen = FIFOData.dataLen - sizeof(FIFOData.sourceFlag) - NUM(FIFOData.RobotData.command);
    memset(toLCDBuf, 0 ,sizeof(toLCDBuf));
    memcpy(toLCDBuf, FIFOData.RobotData.command, NUM(FIFOData.RobotData.command));
    memcpy(toLCDBuf + NUM(FIFOData.RobotData.command), FIFOData.RobotData.priv, privLen);

    return privLen;
}

int SendData2LCD(int DataLen)
{
    int errno = 0;
    int i = 0;

    printf("%s\n", __func__);

    printf("Send to LCD data:");
    for(; i < DataLen; ++i)
    {
        printf("[%x]", *(toLCDBuf+ i));
    }
    printf("\n");

    if(0 > send(clientSockfd, toLCDBuf, DataLen, 0))
    {
        printf("send data error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    return 0;
}

int GetDataFromLCD()
{
    int recLen = 0;
    int i = 0;

    printf("%s\n", __func__);

    fromLCD.privLen = 0;
    memset(fromLCDBuf, 0, ROBOT_DATA_MAX_LEN);
if((recLen = recv(clientSockfd, fromLCDBuf, ROBOT_DATA_MAX_LEN, 0)) == -1)
    {
       perror("recv error");
       return -1;
    }

    fromLCD.privLen = recLen - NUM(fromLCD.command);
    printf("From LCD Data is(%d):", fromLCD.privLen);
    for(; i < recLen; ++i)
    {
        printf("[%c]", *(fromLCDBuf + i));
    }
    printf("\n");

    return 0;
}

int buffer2LCD(struct LCDData *LCDData)
{
    printf("%s\n", __func__);
    memcpy(&(LCDData->privLen), &(fromLCD.privLen), sizeof(fromLCD.privLen));
    memset(LCDData->command, 0, NUM(LCDData->command));
    memcpy(LCDData->command, fromLCDBuf, NUM(LCDData->command));

    memcpy(LCDData->priv, fromLCDBuf+ NUM(LCDData->command), fromLCD.privLen);

    return 0;
}
