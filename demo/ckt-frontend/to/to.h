#ifndef _TO_H_
#define _TO_H_
#include "../from/from.h"

#define LCDBUF_LEN      (ROBOT_COM_LEN + ROBOT_PRIV_LEN)

struct LCDData
{
    char privLen;
    char command[8];
    void *priv;
};

struct FIFOData;

int SocketEnvInit(void);
void SocketEnvDeInit(void);
int FIFO2LCDBuf(struct FIFOData);
int SendData2LCD(int);
int GetDataFromLCD(void);
int buffer2LCD(struct LCDData *);

#endif