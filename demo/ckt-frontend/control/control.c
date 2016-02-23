#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <linux/sockios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include "control.h"
#include "pwmctr.h"

static uint module_exist = 0x0;

int add_module_into_system(struct control_module* module, int (*mod_func)())
{
     if (0 != mod_func(module))
    {
        return -1;
    }

     return module_register(module);

}

int ctr_env_init(void)
{
    struct control_module* module = NULL;
    if (0 != ctr_module_init())
    {
        PDEBUG(LERR, "module_init error~~");
        return -1;
    }

    if(0 == access(PWM_FILE, F_OK))
    {
        if(0 == add_module_into_system( module, pwm_module_init))
            {
                module_exist = module_exist | ME_PWM;
                module->fd = module->open(PWM_FILE);
            }
    }

//1 TBD ULTRASOUND
    return 0;
}

void ctr_env_deinit(void)
{
    struct control_module* control_m = NULL;

    if(1 == (module_exist & ME_PWM))
    {
        control_m = get_module(PWM_MODULE_NAME);
        if(control_m)
        {
            control_m->close(control_m->fd);
        }
    }

//1 TBD ULTRASOUND
    if(1 == (module_exist & ME_ULTRAS))
    {
        control_m = get_module("TBD");
        if(control_m)
        {
            control_m->close(control_m->fd);
        }
    }

//free malloc
    ctr_module_deinit();
}

int do_move_command( control_data control_d)
{
    uchar data[256] = {'0'};
    struct control_module* control_m = NULL;
    control_m = get_module(PWM_MODULE_NAME);

    data[0] = control_d.axis.x;
    data[1] = control_d.axis.y;

    if (0 == (control_m->ioctr(control_m->fd, data)))
    {
        return 0;
    }

    return -1;
}

int do_auto_move_command(control_data control_d)
{
    return 0;
}

int do_control(control_str* control_str)
{
    if(1 != (control_str->module_req & module_exist))
    {
        PDEBUG(LERR, "cmd[%s] need modules, but can be found!!", control_str->cmd);
        return -1;
    }

    if(control_str->control_func)
    {
        if(!control_str->control_func(control_str->control_d))
        {
            return 0;
        }
        else
        {
            PDEBUG(LERR, "CMD[%s] error!!", control_str->cmd);
            return -1;
        }
    }
    else
        {
            PDEBUG(LERR, "No command function can be found!!!");
            return -1;
        }
}
