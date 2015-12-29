#include <stdio.h>
#include "utils.h"

struct RobotData
{
    char command[8];
    void *priv;
};


struct FIFOData
{
    char dataLen;
    char sourceFlag;
    struct RobotData RobotData;
};

#define ROBOT_DATA_MAX_LEN                      256
#define ROBOT_DATA_LENF_OFFSET                _M_OFFSET(dataLen, struct FIFOData)
#define ROBOT_DATA_SRCF_OFFSET                 _M_OFFSET(sourceFlag, struct FIFOData)
#define ROBOT_DATA_COMM_OFFSET             _M_OFFSET(RobotData, struct FIFOData)
#define ROBOT_DATA_PRIV_OFFSET                 (ROBOT_DATA_COMM_OFFSET + _M_OFFSET(priv, struct RobotData))

#define ROBOT_PRIV_LEN                                  (ROBOT_DATA_MAX_LEN - ROBOT_DATA_PRIV_OFFSET + 1)


