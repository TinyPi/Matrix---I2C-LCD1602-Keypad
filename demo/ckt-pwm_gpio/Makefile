#CROSS_COMPILE = arm-linux-gnueabihf-
CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc

EXTRA_CFLAGS += -I$(KERNELSRC)/include/
EXTRA_CFLAGS += -Wall
OBJNAME := pwm_gpio
obj-m := $(OBJNAME).o
#ifneq ($(KERNELRELEASE),)
#Kbuild part of makefile
$(OBJNAME)-objs := pwm_gpio_ctrl.o


#else
PWD := $(shell pwd)
KERNELSRC := /home/qaoy/work/exsdb0/project/nanopi/NanoPi_2/Debian/linux-3.4.y


modules:
	@echo "FLAGS: $(EXTRA_CFLAGS)"
	$(MAKE) -C $(KERNELSRC) CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) $@
	$(CROSS_COMPILE)strip -g $(OBJNAME).ko

clean:
	@echo Cleaning
	@rm -rf *.o *~ core .depend .*.cmd  *.mod.c .tmp_versions *.ko *.symvers
#endif
