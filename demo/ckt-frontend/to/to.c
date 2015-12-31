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
static struct LCDData toLCD;
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
    toLCD.priv = (char *)malloc(ROBOT_PRIV_LEN);
    if(NULL == toLCD.priv)
    {
        printf("toLCD.priv malloc error!!!\n");
        goto ERR;
    }

    fromLCD.priv = (char *)malloc(ROBOT_PRIV_LEN);
    if(NULL == fromLCD.priv)
    {
        printf("fromLCD.priv malloc error!!!\n");
        goto FAER;
    }

    memset(toLCD.priv, 0, ROBOT_PRIV_LEN);
    memset(fromLCD.priv, 0, ROBOT_PRIV_LEN);
    return 0;
FAER:
    free(toLCD.priv);

ERR:
    return -1;
}

void SocketEnvDeInit()
{
    printf("%s\n", __func__);
    free(toLCD.priv);
    free(fromLCD.priv);

    close(clientSockfd);
}

int FIFO2LCD(struct RobotData RobotData)
{
    printf("%s\n", __func__);
    memset(toLCD.priv, 0, ROBOT_PRIV_LEN);
    memcpy(toLCD.command, RobotData.command, NUM(RobotData.command));
    memcpy(toLCD.priv, RobotData.priv, strlen(RobotData.priv));

    return 0;
}

static void LCD2BUF()
{
    printf("%s\n", __func__);
    memset(toLCDBuf, 0 ,sizeof(toLCDBuf));
    memcpy(toLCDBuf, toLCD.command, NUM(toLCD.command));
    memcpy(toLCDBuf + NUM(toLCD.command), toLCD.priv, strlen(toLCD.priv));
}

int SendData2LCD()
{
    int dataLen, errno = 0;

    LCD2BUF();
    printf("%s\n", __func__);
    dataLen = NUM(toLCD.command) + strlen(toLCD.priv);

    if(0 > send(clientSockfd, toLCDBuf, dataLen, 0))
    {
        printf("send data error: %s(errno: %d)\n", strerror(errno), errno);
        return -1;
    }

    return 0;
}

int GetDataFromLCD()
{
    int recLen;

    printf("%s\n", __func__);
if((recLen = recv(clientSockfd, fromLCDBuf, ROBOT_DATA_MAX_LEN, 0)) == -1)
    {
       perror("recv error");
       return -1;
    }

    return 0;
}

int buffer2LCD(struct LCDData *LCDData)
{
    int dataLen = 0;
    char *fromLCDTem = NULL;

    printf("%s\n", __func__);
    memcpy(LCDData->command, fromLCDBuf, NUM(LCDData->command));

    fromLCDTem = fromLCDBuf+ NUM(LCDData->command);
    dataLen = strlen(fromLCDTem);
    memcpy(LCDData->priv, fromLCDBuf+ NUM(LCDData->command), dataLen);

    return 0;
}
