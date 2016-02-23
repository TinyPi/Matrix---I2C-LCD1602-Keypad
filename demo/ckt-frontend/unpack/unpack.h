#ifndef _UNPACK_H_
#define _UNPACK_H_

#include "control.h"
#include "utils.h"

#define ROBOT_DATA_MAX_LEN                      256
#define ROBOT_COM_LEN                                8

#define ROBOT_PRIV_LEN                                  (ROBOT_DATA_MAX_LEN - ROBOT_DATA_PRIV_OFFSET + 1)

#define BUF_BEGIN                               0
#define BUF_LENF_OFFSET                    (BUF_BEGIN + 0)
#define BUF_SRCF_OFFSET                     (BUF_LENF_OFFSET + sizeof(char))
#define BUF_COMM_OFFSET                 (BUF_SRCF_OFFSET + sizeof(char))
#define BUF_PRIV_OFFSET                 (BUF_COMM_OFFSET + 8)               //8 for 8 bytes command

typedef struct
{
    uchar priv_len;
    uchar command[8];
    void *priv;
}priv_data;

typedef struct
{
    uchar dataLen;
    uchar sourceFlag;
    priv_data priv_data;
}package;



int unpack_env_int(int, int);
void unpack_env_deinit(int, int);
int get_package(int, package*);

#endif
