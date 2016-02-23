#ifndef _PWMCTR_H_
#define _PWMCTR_H_

#define PWM_MODULE_NAME "pwm_module"

enum pwm_cmd
{
    PWM_CMD_MOVE = 0,
    PWM_CMD_MAX
};

int pwm_module_init(struct control_module*);

#endif  //#ifndef _PWMCTR_H_
