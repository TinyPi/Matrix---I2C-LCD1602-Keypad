#ifndef _CONTROL_H_
#define _CONTROL_H_
#include "utils.h"
#include "module.h"

#define PWM_FILE    "/dev/pwm"
#define ULTRASOUND_FILE "/dev/ultrasound"

#define ME_PWM  0x01
#define ME_ULTRAS   0x2
#define ME_INRARED  0x4

typedef struct
{
    uint x;
    uint y;
}xy_axis;

typedef union
{
    xy_axis axis;
}control_data;

typedef struct
{
    uchar *cmd;
    uint module_req;    //command requres modules
    control_data control_d;
    int (*control_func)(control_data control_d);
}control_str;

int ctr_env_init(void);
void ctr_env_deinit(void);
int do_move_command(control_data);
int do_auto_move_command(control_data);
int do_control(control_str*);

#endif
