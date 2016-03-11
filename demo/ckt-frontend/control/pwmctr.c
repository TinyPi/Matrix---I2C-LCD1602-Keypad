#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>
#include "utils.h"
#include "module.h"
#include "pwmctr.h"

static void xy_2_pwm(int x, int y, int* pwm_l, int* pwm_r)
{
    if(x >= 0 && y > 0)
    {
        *pwm_l = sqrt(x *x + y * y) * 10;
        *pwm_r = (*pwm_l - 2 * x) *  10;
    }
    if(x < 0 && y > 0)
    {
        *pwm_l = (y + x) *  10;
        *pwm_r = sqrt(x *x + y * y) * 10;
    }
    if(x < 0 && y < 0) 
    {
        *pwm_l = (y - x) *  10;
        *pwm_r = -sqrt(x *x + y * y) * 10;
    }
    if(x > 0 && y < 0)
    {
        *pwm_l = -sqrt(x *x + y * y) * 10;
        *pwm_r = (y + x) *  10;
    }
}

int pwm_open(char * filename, int flag)
{
    PDEBUG(LDEBUG, "begin");
    int fd = open(filename, flag);
    return fd;
}

int pwm_ioctr(int fd, char* control)
{
    char control_d[2] = {'0'};
    int flag = (int)control[0] >= 0? 1: -1;
    //int cmd = CONTROL_IOCTL_CMD_MAKE(1);
    int cmd = PWM_CMD_MOVE;

    PDEBUG(LDEBUG, "X[%d]Y[%d]DIRC[%d]MOVETIME[%d]SWERVETIME[%d]", control[0], control[1], 
                                    control[2], control[3], control[4]);
#if 0
//forward or backward
    control_d[0] = control[1] * 10;
    control_d[1] = control[1] * 10;

    ioctl(fd, cmd, control_d);
    usleep(control[3] * 10000);
    memset(control_d, 0, sizeof(control_d));
    ioctl(fd, cmd, control_d);

//direction
    if(0 != control[2])
    {
        control_d[0] = control[2] * 10 * -flag;
        control_d[1] = -control[2] * 10 * -flag;
        ioctl(fd, cmd, control_d);
        usleep(control[4] * 10000);
        memset(control_d, 0, sizeof(control_d));
        ioctl(fd, cmd, control_d);
    }
#else   //for debug
    if(is_need_compensation())
    {
        PDEBUG(LDEBUG, "compensation!!!!!!!!!!!!!!!!");
        control_d[0] = 10 * 10 * flag;
        control_d[1] = 10 * 10 * flag;
        ioctl(fd, cmd, control_d);
        usleep(control[3] * 1000 * 10);
    }
    control_d[0] = control[0] * 10;
    control_d[1] = control[1] * 10;
    ioctl(fd, cmd, control_d);
#endif
    return 0;
}

int pwm_close(int fd)
{
    close(fd);
    return 0;
}

int pwm_module_init(struct control_module** pwm_module)
{
    PDEBUG(LDEBUG, "*pwm_module addr: %p", *pwm_module);
    *pwm_module = (struct control_module*)malloc(sizeof(struct control_module));
    if (NULL == *pwm_module)
    {
        PDEBUG(LERR, "pwm_module malloc error!!!");
        return -1;
    }

    PDEBUG(LDEBUG, "Begin init pwm mode!!!");
    memcpy((*pwm_module)->name, PWM_MODULE_NAME, sizeof((*pwm_module)->name));
    (*pwm_module)->open = pwm_open;
    (*pwm_module)->ioctr = pwm_ioctr;
    (*pwm_module)->close = pwm_close;

    return 0;
}
