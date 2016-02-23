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

int pwm_ioctr(int fd, uchar* control)
{
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

    memcpy(pwm_module->name, PWM_MODULE_NAME, strlen(PWM_MODULE_NAME));
    pwm_module->open = pwm_open;
    pwm_module->ioctr = pwm_ioctr;
    pwm_module->close = pwm_close;

    return 0;
}