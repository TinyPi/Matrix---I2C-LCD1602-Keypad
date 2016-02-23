#include <stdio.h>
#include <memory.h>
#include "control.h"
#include "analysis.h"

static int get_cmd_type(uchar *cmd)
{
    if(cmd == NULL)
    {
        PDEBUG(LERR, "CMD is null, please check it!!!");
        return -1;
    }

    if(0 == strcmp((char*)cmd, CMD_MOVE))
    {
        return MOVE;
    }
    else if(0 == strcmp((char*)cmd, CMD_AUTO_MOVE))
    {
        return AUTO_MOVE;
    }
    else
    {
        PDEBUG(LWARN, "Unknow cmd!!!");
        return UNKNOW_CMD;
    }
}

int analysis_package(package package, control_str* control_str)
{
    uchar cmd[8] = { 0 };
    uchar *data = NULL;

    memcpy(cmd, package.priv_data.command, NUM(package.priv_data.command));

    PDEBUG(LDEBUG, "CMD:%s", cmd);
    data = package.priv_data.priv;
    switch (get_cmd_type(cmd))
    {
        case UNKNOW_CMD:
            PDEBUG(LWARN, "unknow command!!!!Do nothing!");
            break;

        case MOVE:
            if(NULL == data)
            {
                PDEBUG(LERR, "It's move cmd, but there isn't data!!!error!!");
                return -1;
            }
            memcpy(control_str->cmd, cmd, sizeof(cmd));
            control_str->control_d.axis.x = *(data + 0);
            control_str->control_d.axis.y = *(data + 1);
            control_str->module_req |= ME_PWM;  //move command needs pwm moudule
            control_str->control_func= do_move_command;
            break;

        case AUTO_MOVE:
            break;

        default:
            PDEBUG(LWARN, "Can't identify cmd[%s]", cmd);
            break;
    }

    return 0;
}
