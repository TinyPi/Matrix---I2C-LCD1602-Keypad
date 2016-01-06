#ifndef _TO_H_
#define _TO_H_
#include "from.h"
#include "utils.h"

#define LCDBUF_LEN      (ROBOT_COM_LEN + ROBOT_PRIV_LEN)

struct LCDData
{
    uchar privLen;
    uchar command[8];
    void *priv;
};

struct FIFOData;

int SocketEnvInit(void);
void SocketEnvDeInit(void);
int SendData2LCD(struct FIFOData);
int GetDataFromLCD(void);
int buffer2LCD(struct LCDData *);

#endif