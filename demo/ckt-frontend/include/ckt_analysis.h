#include <stdio.h>

typedef struct RobotDada
{
    char PrefixType:2;                //type:0x00:command;0x01:data;
    char PrefixReserve:6;
    void *priv;
}RobotDada;


