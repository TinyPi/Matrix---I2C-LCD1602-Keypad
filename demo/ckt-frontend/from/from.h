#ifndef _FROM_H_
#define _FROM_H_

#include "to.h"
#include "utils.h"

struct RobotData
{
    uchar privLen;
    uchar command[8];
    void *priv;
};



struct FIFOData
{
    uchar dataLen;
    uchar sourceFlag;
    struct RobotData RobotData;
};

struct LCDData;

#define ROBOT_DATA_MAX_LEN                      256
#define ROBOT_COM_LEN                                8
#define ROBOT_DATA_LENF_OFFSET                _M_OFFSET(dataLen, struct FIFOData)
#define ROBOT_DATA_SRCF_OFFSET                 _M_OFFSET(sourceFlag, struct FIFOData)
#define ROBOT_DATA_COMM_OFFSET             _M_OFFSET(RobotData, struct FIFOData)
#define ROBOT_DATA_PRIV_OFFSET                 (ROBOT_DATA_COMM_OFFSET + _M_OFFSET(priv, struct RobotData))

#define ROBOT_PRIV_LEN                                  (ROBOT_DATA_MAX_LEN - ROBOT_DATA_PRIV_OFFSET + 1)

#define BUF_BEGIN                               0
#define BUF_LENF_OFFSET                    (BUF_BEGIN + 0)
#define BUF_SRCF_OFFSET                     (BUF_LENF_OFFSET + sizeof(char))
#define BUF_COMM_OFFSET                 (BUF_SRCF_OFFSET + sizeof(char))
#define BUF_PRIV_OFFSET                 (BUF_COMM_OFFSET + 8)               //8 for 8 bytes command

int FIFOReadEnvInit(void);
int FIFOWriteEnvInit(void);
void FIFOReadEnvDeInit(void);
void FIFOWriteEnvDeInit(void);

int GetDataFromFIFO();
int SendData2Bg(struct LCDData);
int Buffer2FIFO(struct FIFOData *);


#endif
