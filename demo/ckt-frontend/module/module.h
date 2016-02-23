#ifndef _MODULE_H_
#define _MODULE_H_

#include "list.h"
#include "utils.h"

struct control_module
{
//module name
    char* name;
    int fd;
//open module
    int (*open)(char* name);
//read data from module
    int (*read)(int fd, char* buf);
//write data to module
    int (*write)(int fd, char* buf);
//iocontrol
    int (*ioctr)(int fd, uchar* control);
//module close
    int (*close)(int fd);
//other normal  method to control module
    int (*normal)(void);

    struct list_head list_head;
};

int ctr_module_init(void);
void ctr_module_deinit(void);
int module_register(struct control_module*);
int module_unregister(struct control_module*);
struct control_module* get_module(char*);

#endif //#ifndef _MODULE_H_
