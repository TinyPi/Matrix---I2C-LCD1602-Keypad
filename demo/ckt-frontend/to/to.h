#ifndef _TO_H_
#define _TO_H_
#include "../from/from.h"

#define LCDBUF_LEN      (ROBOT_COM_LEN + ROBOT_PRIV_LEN)

struct LCDData
{
    char command[8];
    void *priv;
};

struct RobotData;

int SocketEnvInit(void);
void SocketEnvDeInit(void);
int FIFO2LCD(struct RobotData);
int SendData2LCD(void);
int GetDataFromLCD(void);
int buffer2LCD(struct LCDData *);

#endif