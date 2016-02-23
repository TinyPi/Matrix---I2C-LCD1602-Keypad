#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_
#include "utils.h"
#include "unpack.h"

#define CMD_MOVE                            "MOVE"
#define CMD_AUTO_MOVE            "AUTO_M"

enum cmd_type
{
    UNKNOW_CMD = 0,
    MOVE,
    AUTO_MOVE,
    CMD_MAX
};

int analysis_package(package, control_str*);
#endif  //#ifndef _ANALYSIS_H_