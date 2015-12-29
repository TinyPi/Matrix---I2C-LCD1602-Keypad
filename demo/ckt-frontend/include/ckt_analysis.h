#include <stdio.h>

#define ROBOT_DATA_MAX_LEN                      256
#define ROBOT_DATA_LENF_OFFSET                0
#define ROBOT_DATA_SRCF_OFFSET                 1
#define ROBOT_DATA_COMM_OFFSET             2
#define ROBOT_DATA_PRIV_OFFSET                 9

#define ROBOT_PRIV_LEN                                  (ROBOT_DATA_MAX_LEN - ROBOT_DATA_PRIV_OFFSET + 1)

struct RobotData
{
    char dataLen;
    char sourceFlag;     //for source controller flag
    char command[8];
    void *priv;
};


