#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include "utils.h"
#include "module.h"
#include "pwmctr.h"

int pwm_open(char * filename)
{
    int fd = open(filename, O_RDWR);
    return fd;
}

int pwm_ioctr(int fd, char* control)
{
    char control_d[3] = {'0'};
    int flag = control[1] >= 0? 1: -1;
    int cmd = CONTROL_IOCTL_CMD_MAKE(PWM_CMD_MOVE);

    PDEBUG(LDEBUG, "X[%d]Y[%d]DIRC[%d]", control[0], control[1], control[2]);
//forward or backward
    control_d[0] = control[1] * 10;
    control_d[1] = control[1] * 10;

    ioctl(fd, PWM_CMD_MOVE, control_d);
    usleep(control[3] * 10);
    memset(control_d, 0, sizeof(control_d));
    ioctl(fd, PWM_CMD_MOVE, control_d);

//direction
    control_d[0] = control[2] * 50 * -flag;
    control_d[1] = -control[2] * 50 * -flag;
    ioctl(fd, PWM_CMD_MOVE, control_d);
    usleep(control[4] * 10);
    memset(control_d, 0, sizeof(control_d));
    ioctl(fd, PWM_CMD_MOVE, control_d);

    return 0;
}

int pwm_close(int fd)
{
    close(fd);
    return 0;
}

int pwm_module_init(struct control_module* pwm_module)
{
    pwm_module = (struct control_module*)malloc(sizeof(struct control_module));
    if (NULL == pwm_module)
    {
        PDEBUG(LERR, "pwm_module malloc error!!!");
        return -1;
    }

    pwm_module->name = PWM_MODULE_NAME;
    pwm_module->open = pwm_open;
    pwm_module->ioctr = pwm_ioctr;
    pwm_module->close = pwm_close;

    return 0;
}
