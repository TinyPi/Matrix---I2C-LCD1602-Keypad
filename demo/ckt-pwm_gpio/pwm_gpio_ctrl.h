#ifndef __PWM_H__
#define __PWM_H__

//#include <stdio.h>
#include <linux/fcntl.h>
#include <linux/string.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/syscalls.h>
#include <linux/stat.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/kernel.h>

#include <linux/types.h>  
#include <linux/fs.h>  
#include <linux/proc_fs.h>  
#include <linux/device.h>  
  
#include <asm/uaccess.h>  

//#define PWM0    "/sys/devices/platform/pwm/pwm.0"
//#define PWM1    "/sys/devices/platform/pwm/pwm.1"
#define PWM0   0
#define PWM1   1

//#define DEVICE_FILE_NAME "pwm_dev_f"
#define DEVICE_FILE_NAME   "pwm"
enum {
     PAD_GPIO_A      = (0 * 32),
     PAD_GPIO_B      = (1 * 32),
     PAD_GPIO_C      = (2 * 32),
     PAD_GPIO_D      = (3 * 32),
     PAD_GPIO_E      = (4 * 32),
     PAD_GPIO_ALV    = (5 * 32),
 };

struct pwm_dev {
    int val;
    struct cdev cdev;
};

/*
#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO26EN    "/sys/class/gpio/gpio58/direction" //GPIOB26 ---> IN1
#define GPIO27EN    "/sys/class/gpio/gpio59/direction" //GPIOB27 ---> IN2
#define GPIO30EN    "/sys/class/gpio/gpio62/direction" //GPIOB30 ---> IN5
#define GPIO31EN    "/sys/class/gpio/gpio63/direction" //GPIOB31 ---> IN6

#define GPIO26VA    "/sys/class/gpio/gpio58/value"
#define GPIO27VA    "/sys/class/gpio/gpio59/value"
#define GPIO30VA    "/sys/class/gpio/gpio62/value"
#define GPIO31VA    "/sys/class/gpio/gpio63/value"

char gpio_num[] = {"58", "59", "62", "63"};
*/

#define GPIOB_26    PAD_GPIO_B+26
#define GPIOB_27    PAD_GPIO_B+27
#define GPIOB_30    PAD_GPIO_B+30
#define GPIOB_31    PAD_GPIO_B+31


//#define IOCTL_SET_MOVE _IOR(MAJOR_NUM, 0, char *)
//#define IOCTL_GET_MOVE _IOR(MAJOR_NUM, 1, char *)
//#define CONTROL_DRV_MAGICNUM 'p'
//#define PWM_CMD_MOVE     ( _IO( CONTROL_DRV_MAGICNUM, 1) ) 

enum pwm_cmd
{
    PWM_CMD_MOVE = 0,
    PWM_CMD_MAX
};

#define CONTROL_DRV_MAGICNUM 1234
#define CONTROL_IOCTL_CMD_MAKE(cmd)     ( _IO( CONTROL_DRV_MAGICNUM, cmd) )


int gpio_set(const char *, const char *);
int gpio_init();

extern unsigned long nxp_soc_pwm_set_frequency(int, unsigned int, unsigned int);
extern void nxp_soc_pwm_get_frequency(int, unsigned int *, unsigned int *);

extern void nxp_soc_gpio_set_io_dir(unsigned int, int);
extern void nxp_soc_gpio_set_out_value(unsigned int, int);
extern int nxp_soc_gpio_get_out_value(unsigned int);

//extern int nxp_soc_gpio_get_in_value(unsigned int);
extern int nxp_soc_gpio_get_io_dir(unsigned int);



#endif

