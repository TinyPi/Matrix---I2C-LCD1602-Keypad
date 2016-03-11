#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <linux/sockios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<sys/types.h>
#include <sys/time.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include "control.h"
#include "pwmctr.h"

#define FILE_FLAG O_RDWR

static uint module_exist = 0x0;
static struct itimerval time;
static BOOL PWM_first_start = TRUE;
static BOOL PWM_compensation = FALSE;

void sig_close_pwm(int signo)
{
    char data[2] = {'0'};
    struct control_module* control_m = NULL;
    control_m = get_module(PWM_MODULE_NAME);

    PDEBUG(LDEBUG, "SIGALARM:close pwm****************");
    data[0] = 0;
    data[1] = 0;
    if (0 == (control_m->ioctr(control_m->fd, data)))
    {
        PDEBUG(LDEBUG, "Control pwm successfully");
    }

    PWM_first_start = TRUE;
}

void set_time(int sec, int usec)
{
    int sec_tmp = 0;

    if(usec >= 100)
    {
        sec_tmp= usec / 100;
        usec = 0;
    }
    sec += sec_tmp;
    
    time.it_value.tv_sec = sec;
    time.it_value.tv_usec = usec * 1000 * 10;
    time.it_interval.tv_sec = 0;
    time.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &time, NULL);
}

extern BOOL is_need_compensation(char* data)
{
    PDEBUG(LDEBUG, "PWM_compensation is [%d], PWM_first_start is [%d]", PWM_compensation, 
                                        PWM_first_start);
    return PWM_compensation || PWM_first_start;
}

int add_module_into_system(struct control_module* module, int (*mod_func)())
{
     if (0 != mod_func(&module))
    {
        PDEBUG(LERR, "ERROR!!!");
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
    if(0 == access(PWM_MODULE_FILE, F_OK))
    {
        char temp[256] = { 0 };
        PDEBUG(LDEBUG, "insmod pwm module");
        snprintf(temp, sizeof(temp), "insmod %s", PWM_MODULE_FILE);
        system(temp);

        if(0 == access(PWM_FILE, F_OK))
        {
            PDEBUG(LDEBUG, "There is pwm file here");
            if(0 == add_module_into_system( module, pwm_module_init))
            {
                module_exist = module_exist | ME_PWM;
            }
        }
    }
    signal(SIGALRM, sig_close_pwm);


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

int do_around_move(void)
{
    char data[2] = {'0'};
    struct control_module* control_m = NULL;
    control_m = get_module(PWM_MODULE_NAME);

    if(0 == control_m->fd)
    {
        control_m->fd = control_m->open(PWM_FILE, FILE_FLAG);
        if(0 > control_m->fd)
        {
            PDEBUG(LERR, "Open [%s] file error!!", PWM_FILE);
            return -1;
        }
    }

    data[0] = 9;
    data[1] = -9;
    if (0 != (control_m->ioctr(control_m->fd, data)))
    {
        return -1;
    }
    usleep(700 * 1000);
    data[0] = 0;
    data[1] = 0;
    if (0 != (control_m->ioctr(control_m->fd, data)))
    {
        return -1;
    }
    usleep(300 * 1000);
    data[0] = -9;
    data[1] = 9;
    if (0 != (control_m->ioctr(control_m->fd, data)))
    {
        return -1;
    }
    usleep(700 * 1000);
    data[0] = 0;
    data[1] = 0;
    if (0 != (control_m->ioctr(control_m->fd, data)))
    {
        return -1;
    }

    return 0;
}

int do_move_command( control_data control_d)
{
    char data[8] = {'0'};
    static int PWM_opposite = FALSE;
    BOOL PWM_opposite_tmp = FALSE;
    struct control_module* control_m = NULL;
    control_m = get_module(PWM_MODULE_NAME);

    set_time(0, control_d.axis.direction);

    PDEBUG(LDEBUG, "open pwm file");
    if(0 == control_m->fd)
    {
        control_m->fd = control_m->open(PWM_FILE, FILE_FLAG);
        if(0 > control_m->fd)
        {
            PDEBUG(LERR, "Open [%s] file error!!", PWM_FILE);
            return -1;
        }
    }
    PDEBUG(LDEBUG, "pwm iotrl begin!!");
    data[0] = control_d.axis.x;
    data[1] = control_d.axis.y;
    data[2] = control_d.axis.direction;
    data[3] = control_d.axis.forward_t;
    data[4] = control_d.axis.swerve_t;

    PWM_opposite_tmp = PWM_opposite;
    PWM_opposite = (int)data[0] >= 0? FALSE: TRUE;
    if((PWM_opposite != PWM_opposite_tmp) && (!PWM_first_start))
    {
        PDEBUG(LDEBUG, "PWM need compensation.");
        PWM_compensation = TRUE;
    }

    if (0 == (control_m->ioctr(control_m->fd, data)))
    {
        PWM_first_start = FALSE;
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
        PDEBUG(LERR, "cmd[%s] need modules, but can't be found!!", control_str->cmd);
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
